using Manager.Domain;
using Manager.Log;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Manager.Server
{
    public class IceServer
    {
        private const string HANDSHAKE = "168494987!#())-=+_IceStorm153_))((--85*";

        private Logger log = Logger.Instance;

        private ManualResetEvent stopEvent;
        private Thread threadListen;
        private Socket listener;
        private SocketHelper soc;

        private string serverIp = "";
        private int port = 0;

        private object syncClients = new object();
        private Client[] clients;
        public Action<Client[]> ClientsChangedCallback { get; set; }

        public IceServer()
        {
            clients = new Client[0];
        }

        public void StartServer()
        {
            LoadConfig();
            threadListen = new Thread(ListenThread);
            stopEvent = new ManualResetEvent(false);
            soc = new SocketHelper(ref stopEvent);

            threadListen.Start();

            log.Info("Server started");
        }

        private void LoadConfig()
        {
            serverIp = "192.168.194.1";
            port = 12345;
        }
        
        private IPEndPoint GetIPEndPoint()
        {
            IPAddress ipAddress = null;// ipHostInfo.AddressList[0];
            foreach (IPAddress ipAddr in Dns.GetHostEntry(Dns.GetHostName()).AddressList)
            {
                if (ipAddr.ToString().Equals(serverIp))
                {
                    ipAddress = ipAddr;
                    break;
                }
            }

            return new IPEndPoint(ipAddress, port);
        }

        public void StopServer()
        {
            stopEvent.Set();

            threadListen.Join(5000);
            threadListen = null;

            if (clients != null)
            {
                foreach (Client c in clients)
                {
                    if (null != c.Socket)
                    {
                        try
                        {
                            c.Socket.Shutdown(SocketShutdown.Both);
                            c.Socket.Close();
                            c.Socket = null;
                        }
                        catch
                        {

                        }
                    }
                }
            }

            log.Info("Server stopped");
        }

        private void ListenThread()
        {
            if (stopEvent.WaitOne(1000)) return;

            log.Info("ListenThread started");
            int errors = 0;
            bool restartServer = false;
            
            if (!CreateSocket()) return;

            log.Info("Waiting for clients...");
            while (true)
            {
                try
                {
                    Socket clientSocket = null;
                    
                    try
                    {
                        clientSocket = listener.Accept();
                    }
                    catch (SocketException ex)
                    {
                        if (ex.ErrorCode != 10035)
                        {
                            log.Error(ex.Message);
                            throw;
                        }
                    }
                    
                    if (null == clientSocket)
                    {
                        if (stopEvent.WaitOne(200)) break;
                        continue;
                    }

                    if (stopEvent.WaitOne(1)) break;
                    log.Info(string.Format("Client {0} connected", clientSocket.RemoteEndPoint));

                    clientSocket.Blocking = false;

                    int idx = 0;
                    lock (syncClients)
                    {
                        idx = clients.Length;
                        Array.Resize(ref clients, idx + 1);
                        clients[idx] = new Client();
                        clients[idx].Socket = clientSocket;
                        clients[idx].IP = clients[idx].Socket.RemoteEndPoint.ToString();

                        if (!GetHanshakePackege(clients[idx]))
                        {
                            clients[idx].Socket.Shutdown(SocketShutdown.Both);
                            clients[idx].Socket.Close();
                        }
                        else
                        {
                            clients[idx].Name = soc.RecvString(clients[idx].Socket);
                            clients[idx].OS = soc.RecvString(clients[idx].Socket);
                            clients[idx].Platform = soc.RecvString(clients[idx].Socket);
                            clients[idx].NrOfProcessors = soc.RecvDWORD(clients[idx].Socket);

                            NotifyClientsChange();
                        }
                    }

                    errors = 0;
                }
                catch (Exception ex)
                {
                    log.Error("Exception in listen loop: " + ex.Message);
                    errors++;

                    if (errors == 10)
                    {
                        restartServer = true;
                        break;
                    }
                }
                
                if (stopEvent.WaitOne(100)) break;
            }

            if (restartServer && !stopEvent.WaitOne(50))
            {
                log.Info("Will restart the server");
                StartServer();
            }

            log.Info("ListenThread stopped");
        }

        private bool GetHanshakePackege(Client client)
        {
            int result = 1;
            string handshakePacket = soc.RecvString(client.Socket);

            if (!HANDSHAKE.Equals(handshakePacket)) result = 0;
            
            log.Info(string.Format("Client {0} sent {1} handshake.", client.IP, result == 1 ? "good" : "bad"));

            soc.SendDWORD(client.Socket, result);

            return result == 1;
        }

        private bool CreateSocket()
        {
            while (true)
            {
                try
                {
                    IPEndPoint localEndPoint = GetIPEndPoint();
                    listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

                    listener.Blocking = false;
                    listener.Bind(localEndPoint);
                    listener.Listen(100);

                    log.Info("Listen socket created with success");
                    return true;
                }
                catch (Exception ex)
                {
                    log.Error(ex.Message);
                }

                if (stopEvent.WaitOne(200)) return false;
            }
        }

        private void NotifyClientsChange()
        {
            if (null == ClientsChangedCallback) return;

            RefreshClients();

            ClientsChangedCallback(clients);
        }

        private void RefreshClients()
        {
            Client[] newClients = new Client[0];
            int newLen = 0;

            log.Info("Refreshing clients list");

            foreach (Client c in clients)
            {
                if (!CheckIfClientIsStillConnected(c))
                {
                    c.Socket.Shutdown(SocketShutdown.Both);
                    c.Socket.Close();
                    continue;
                }

                Array.Resize(ref newClients, newLen + 1);
                newClients[newLen] = c;

                newLen++;
            }

            clients = newClients;
        }

        private bool CheckIfClientIsStillConnected(Client c)
        {
            Socket s = c.Socket;
            bool isConnected = false;

            try
            {
                isConnected = PingClient(c);
            }
            catch (Exception ex)
            {
                log.Error(ex.Message);
                isConnected = false;
            }

            return isConnected;
        }

        private bool PingClient(Client c)
        {
            bool isConnected = false;

            try
            {
                lock (c.SyncAccess)
                {
                    soc.SendDWORD(c.Socket, (int)IceServerCommand.Ping);
                    int r = soc.RecvDWORD(c.Socket);
                    isConnected = r == (int)IceServerCommandResult.Success;
                }
            }
            catch (Exception ex)
            {
                log.Error("Ping on " + c.Socket.RemoteEndPoint + " failed: " + ex.Message);
            }

            return isConnected;
        }


        /****************************************************************/
        /* Public function                                              */
        /****************************************************************/
        public AppCtrlRule[] GetAppCtrlRules(Client client)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.GetAppCtrlRules);
            
            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to get AppCtrl Rules for client " + client.Name);

            int len = soc.RecvDWORD(client.Socket);

            if (len == 0) return new AppCtrlRule[0];

            AppCtrlRule[] appRules = new AppCtrlRule[len];

            for (int i = 0; i < len; i++)
            {
                appRules[i] = new AppCtrlRule();

                appRules[i].RuleID = soc.RecvDWORD(client.Socket);
                appRules[i].ProcessPathMatcher = (IceStringMatcher) soc.RecvDWORD(client.Socket);
                appRules[i].ProcessPath = soc.RecvString(client.Socket);
                appRules[i].PID = soc.RecvDWORD(client.Socket);
                appRules[i].ParentPathMatcher = (IceStringMatcher) soc.RecvDWORD(client.Socket);
                appRules[i].ParentPath = soc.RecvString(client.Socket);
                appRules[i].ParentPID = soc.RecvDWORD(client.Socket);
                appRules[i].Verdict = (IceScanVerdict)soc.RecvDWORD(client.Socket);
                appRules[i].AddTime = soc.RecvDWORD(client.Socket);
            }

            client.AppCtrlRules = appRules;
            return appRules;
        }

        public FSRule[] GetFSRules(Client client)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.GetFSScanRules);

            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to get FSScan Rules for client " + client.Name);

            int len = soc.RecvDWORD(client.Socket);

            if (len == 0) return new FSRule[0];

            FSRule[] fsRules = new FSRule[len];

            for (int i = 0; i < len; i++)
            {
                fsRules[i] = new FSRule();

                fsRules[i].RuleID = soc.RecvDWORD(client.Socket);
                fsRules[i].ProcessPathMatcher = (IceStringMatcher)soc.RecvDWORD(client.Socket);
                fsRules[i].ProcessPath = soc.RecvString(client.Socket);
                fsRules[i].PID = soc.RecvDWORD(client.Socket);
                fsRules[i].FilePathMatcher = (IceStringMatcher)soc.RecvDWORD(client.Socket);
                fsRules[i].FilePath = soc.RecvString(client.Socket);
                fsRules[i].DeniedOperations = soc.RecvDWORD(client.Socket);
                fsRules[i].AddTime = soc.RecvDWORD(client.Socket);
            }

            client.FSRules = fsRules;
            return fsRules;
        }

        public AppCtrlEvent[] GetAppCtrlEvents(Client client, int lastID)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.GetAppCtrlEvents);
            soc.SendDWORD(client.Socket, lastID);

            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to get AppCtrl Events for client " + client.Name);

            int len = soc.RecvDWORD(client.Socket);

            if (len == 0) return new AppCtrlEvent[0];

            AppCtrlEvent[] appEvents = new AppCtrlEvent[len];

            for (int i = 0; i < len; i++)
            {
                appEvents[i] = new AppCtrlEvent();

                appEvents[i].EventID = soc.RecvDWORD(client.Socket);
                appEvents[i].ProcessPath = soc.RecvString(client.Socket);
                appEvents[i].PID = soc.RecvDWORD(client.Socket);
                appEvents[i].ParentPath = soc.RecvString(client.Socket);
                appEvents[i].ParentPID = soc.RecvDWORD(client.Socket);
                appEvents[i].Verdict = (IceScanVerdict) soc.RecvDWORD(client.Socket);
                appEvents[i].MatchedRuleID = soc.RecvDWORD(client.Socket);
                appEvents[i].EventTime = soc.RecvDWORD(client.Socket);
            }
            
            return appEvents;
        }

        public FSEvent[] GetFSEvents(Client client, int lastID)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.GetFSScanEvents);
            soc.SendDWORD(client.Socket, lastID);

            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to get FSScan Events for client " + client.Name);

            int len = soc.RecvDWORD(client.Socket);

            if (len == 0) return new FSEvent[0];

            FSEvent[] fsEvents = new FSEvent[len];

            for (int i = 0; i < len; i++)
            {
                fsEvents[i] = new FSEvent();

                fsEvents[i].EventID = soc.RecvDWORD(client.Socket);
                fsEvents[i].ProcessPath = soc.RecvString(client.Socket);
                fsEvents[i].PID = soc.RecvDWORD(client.Socket);
                fsEvents[i].FilePath = soc.RecvString(client.Socket);
                fsEvents[i].RequiredOperations = soc.RecvDWORD(client.Socket);
                fsEvents[i].DeniedOperations = soc.RecvDWORD(client.Socket);
                fsEvents[i].RemainingOperations = soc.RecvDWORD(client.Socket);
                fsEvents[i].MatchedRuleID = soc.RecvDWORD(client.Socket);
                fsEvents[i].EventTime = soc.RecvDWORD(client.Socket);
            }

            return fsEvents;
        }

        public int GetAppCtrlStatus(Client client)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.GetAppCtrlStatus);

            IceServerCommandResult cmdResult = (IceServerCommandResult) soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to get AppCtrl Status for client " + client.Name);

            int appCtrlStatus = soc.RecvDWORD(client.Socket);
            
            client.IsAppCtrlEnabled = (appCtrlStatus == 1);

            return appCtrlStatus;
        }

        public int EnableAppCtrl(Client client, int enable)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.SetAppCtrlStatus);
            soc.SendDWORD(client.Socket, enable);

            IceServerCommandResult cmdResult = (IceServerCommandResult) soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to set AppCtrl Status on " + enable  + " for client " + client.Name);

            return 1;
        }

        public int EnableFSScan(Client client, int enable)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.SetFSScanStatus);
            soc.SendDWORD(client.Socket, enable);

            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to set FSScan Status on " + enable + " for client " + client.Name);

            return 1;
        }

        public int GetFSScanStatus(Client client)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.GetFSScanStatus);

            IceServerCommandResult cmdResult = (IceServerCommandResult) soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to get FSScan Status for client " + client.Name);

            int fsScanStatus = soc.RecvDWORD(client.Socket);

            client.IsFSScanEnabled = (fsScanStatus == 1);

            return fsScanStatus;
        }

        public int SendSetOption(Client client, int option, int value)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.SetOption);
            soc.SendDWORD(client.Socket, option);
            soc.SendDWORD(client.Socket, value);

            int errorCode = soc.RecvDWORD(client.Socket); ;
            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception(
                    string.Format("Failed to Send Set Option {0} {1} client {2} (Error: {3})", option, value, client.Name, errorCode));

            return 1;
        }

        public int DeleteFSScanRule(Client client, int id)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.DeleteFSScanRule);

            soc.SendDWORD(client.Socket, id);

            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to Delete FSScan Rule " + id + " for client " + client.Name);

            return 1;
        }

        public int AddAppCtrlRule(Client client, AppCtrlRule rule)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.AddAppCtrlRule);

            soc.SendDWORD(client.Socket, (int)rule.ProcessPathMatcher);
            soc.SendString(client.Socket, rule.ProcessPath);
            soc.SendDWORD(client.Socket, rule.PID);
            soc.SendDWORD(client.Socket, (int)rule.ParentPathMatcher);
            soc.SendString(client.Socket, rule.ParentPath);
            soc.SendDWORD(client.Socket, rule.ParentPID);
            soc.SendDWORD(client.Socket, (int)rule.Verdict);

            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to Add AppCtrl Rule for client " + client.Name);

            int ruleId = soc.RecvDWORD(client.Socket);

            log.Info("AppCtrl rule was added: " + ruleId);
            return ruleId;
        }

        public int AddFSScanRule(Client client, FSRule rule)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.AddFSScanRule);

            soc.SendDWORD(client.Socket, (int)rule.ProcessPathMatcher);
            soc.SendString(client.Socket, rule.ProcessPath);
            soc.SendDWORD(client.Socket, rule.PID);
            soc.SendDWORD(client.Socket, (int)rule.FilePathMatcher);
            soc.SendString(client.Socket, rule.FilePath);
            soc.SendDWORD(client.Socket, rule.DeniedOperations);

            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to Add FSScan Rule for client " + client.Name);

            int ruleId = soc.RecvDWORD(client.Socket);

            log.Info("FSScan rule was added: " + ruleId);
            return ruleId;
        }

        public int DeleteAppCtrlRule(Client client, int id)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.DeleteAppCtrlRule);

            soc.SendDWORD(client.Socket, id);
            
            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to Delete AppCtrl Rule " + id + " for client " + client.Name);

            return 1;
        }

        public int UpdateAppCtrlRule(Client client, AppCtrlRule rule)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.UpdateAppCtrlRule);
            soc.SendDWORD(client.Socket, rule.RuleID);

            soc.SendDWORD(client.Socket, (int)rule.ProcessPathMatcher);
            soc.SendString(client.Socket, rule.ProcessPath);
            soc.SendDWORD(client.Socket, rule.PID);
            soc.SendDWORD(client.Socket, (int)rule.ParentPathMatcher);
            soc.SendString(client.Socket, rule.ParentPath);
            soc.SendDWORD(client.Socket, rule.ParentPID);
            soc.SendDWORD(client.Socket, (int)rule.Verdict);

            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to Update AppCtrl Rule " + rule.RuleID + "  for client " + client.Name);

            return 1;
        }

        public int UpdateFSScanRule(Client client, FSRule rule)
        {
            soc.SendDWORD(client.Socket, (int)IceServerCommand.UpdateFSScanRule);
            soc.SendDWORD(client.Socket, rule.RuleID);

            soc.SendDWORD(client.Socket, (int)rule.ProcessPathMatcher);
            soc.SendString(client.Socket, rule.ProcessPath);
            soc.SendDWORD(client.Socket, rule.PID);
            soc.SendDWORD(client.Socket, (int)rule.FilePathMatcher);
            soc.SendString(client.Socket, rule.FilePath);
            soc.SendDWORD(client.Socket, rule.DeniedOperations);

            IceServerCommandResult cmdResult = (IceServerCommandResult)soc.RecvDWORD(client.Socket);
            if (cmdResult != IceServerCommandResult.Success)
                throw new Exception("Failed to Update FSScan Rule " + rule.RuleID + "  for client " + client.Name);

            return 1;
        }
    }
}
