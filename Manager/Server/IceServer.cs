using Manager.Domain;
using Manager.Log;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Manager.Server
{
    public class IceServer
    {
        private Logger log = Logger.Instance;

        private ManualResetEvent stopEvent;
        private Thread threadListen;

        private string serverIp = "";
        private int port = 0;

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
            threadListen.Start();

            log.Info("Server started");
        }

        public void StopServer()
        {
            stopEvent.Set();

            threadListen.Join(5000);
            threadListen = null;

            log.Info("Server stopped");
        }

        public Client[] GetClients()
        {
            return clients;
        }

        private void ListenThread()
        {
            if (stopEvent.WaitOne(2000)) return;

            log.Info("ListenThread started");
            int errors = 0;
            bool restartServer = false;
            int i = 0;

            while (true)
            {
                try
                {
                    if (i < 5)
                    {
                        i++;
                        clients = new Client[i];

                        for (int j = 0; j < i; j++)
                        {
                            clients[j] = new Client();
                            clients[j].ClientID = j + 1;
                            clients[j].Name = "Client " + j;
                            clients[j].IP = string.Format("{0}.{0}.{0}.{0}", j);
                            clients[j].OS = "Windows 10";
                            clients[j].Platform = "x64";
                        }

                        NotifyClientsChange();
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
                log.Info("nu");
            }

            if (restartServer && !stopEvent.WaitOne(50))
            {
                log.Info("Will restart the server");
                StartServer();
            }

            log.Info("ListenThread stopped");
        }

        public AppCtrlRule[] GetAppCtrlRules(Client client)
        {
            if (null != client.AppCtrlRules) return client.AppCtrlRules;

            AppCtrlRule[] fakeAppRules = new AppCtrlRule[10];

            for (int i = 0; i < fakeAppRules.Length; i++)
            {
                fakeAppRules[i] = new AppCtrlRule();

                fakeAppRules[i].RuleID = i + 1;
                fakeAppRules[i].ProcessPath = "C:\\path\\proces" + i + ".exe";
                fakeAppRules[i].ParentPath = "C:\\path\\parinte" + i + ".exe";
                fakeAppRules[i].ParentPID = (i + 1) * 50 + i;
                fakeAppRules[i].PID = (i + 1) * 50;
                fakeAppRules[i].ProcessPathMatcher = ((i % 2) == 1) ? IceStringMatcher.Equal : IceStringMatcher.Wildmat;
                fakeAppRules[i].ParentPathMatcher = ((i % 2) == 1) ? IceStringMatcher.Equal : IceStringMatcher.Wildmat;
                fakeAppRules[i].AddTime = 6000 + i;
                fakeAppRules[i].Verdict = ((i % 2) == 1) ? IceScanVerdict.Allow : IceScanVerdict.Deny;
            }

            client.AppCtrlRules = fakeAppRules;
            return fakeAppRules;
        }

        public FSRule[] GetFSRules(Client client)
        {
            if (null != client.FSRules) return client.FSRules;

            FSRule[] fakeFSRules = new FSRule[10];

            for (int i = 0; i < fakeFSRules.Length; i++)
            {
                fakeFSRules[i] = new FSRule();

                fakeFSRules[i].RuleID = i + 1;
                fakeFSRules[i].ProcessPathMatcher = ((i % 2) == 1) ? IceStringMatcher.Equal : IceStringMatcher.Wildmat;
                fakeFSRules[i].ProcessPath = "C:\\path\\proces" + i + ".exe";
                fakeFSRules[i].PID = (i + 1) * 20;
                fakeFSRules[i].FilePathMatcher = ((i % 2) == 1) ? IceStringMatcher.Equal : IceStringMatcher.Wildmat;
                fakeFSRules[i].FilePath = "C:\\fisiere\\file" + i + ".txt";
                fakeFSRules[i].DeniedOperations = i;
                fakeFSRules[i].AddTime = 5000 + i;
            }

            client.FSRules = fakeFSRules;
            return fakeFSRules;
        }

        public AppCtrlEvent[] GetAppCtrlEvents(Client client)
        {
            if (null != client.AppCtrlEvents) return client.AppCtrlEvents;

            AppCtrlEvent[] fakeAppEvents = new AppCtrlEvent[10];

            for (int i = 0; i < fakeAppEvents.Length; i++)
            {
                fakeAppEvents[i] = new AppCtrlEvent();

                fakeAppEvents[i].EventID = i + 1;
                fakeAppEvents[i].ProcessPath = "C:\\path\\proces" + i + ".exe";
                fakeAppEvents[i].PID = (i + 1) * 50;
                fakeAppEvents[i].ParentPath = "C:\\path\\parinte" + i + ".exe";
                fakeAppEvents[i].ParentPID = (i + 1) * 50 + i;
                fakeAppEvents[i].Verdict = ((i % 2) == 1) ? IceScanVerdict.Allow : IceScanVerdict.Deny;
                fakeAppEvents[i].MatchedRuleID = i * 2 +1;
                fakeAppEvents[i].EventTime = 6000 + i;
            }

            client.AppCtrlEvents = fakeAppEvents;
            return fakeAppEvents;
        }

        public FSEvent[] GetFSEvents(Client client)
        {
            if (null != client.FSEvents) return client.FSEvents;

            FSEvent[] fakeFSEvents = new FSEvent[10];

            for (int i = 0; i < fakeFSEvents.Length; i++)
            {
                fakeFSEvents[i] = new FSEvent();

                fakeFSEvents[i].EventID = i + 1;
                fakeFSEvents[i].ProcessPath = "C:\\path\\proces" + i + ".exe";
                fakeFSEvents[i].PID = (i + 1) * 20;
                fakeFSEvents[i].FilePath = "C:\\fisiere\\file" + i + ".txt";
                fakeFSEvents[i].RequiredOperations = i + 10;
                fakeFSEvents[i].DeniedOperations = i + 10 - 5;
                fakeFSEvents[i].RequiredOperations = i;
                fakeFSEvents[i].MatchedRuleID = i * 2 + 1;
                fakeFSEvents[i].EventTime = 7000 + i;
            }

            client.FSEvents = fakeFSEvents;
            return fakeFSEvents;
        }

        public int GetAppCtrlStatus(Client client)
        {
            return client.IsAppCtrlEnabled ? 1 : 0;
        }

        public int EnableAppCtrl(Client client, int enable)
        {
            client.IsAppCtrlEnabled = enable == 1;
            return 1;
        }

        public int EnableFSScan(Client client, int enable)
        {
            client.IsFSScanEnabled = enable == 1;
            return 1;
        }

        public int GetFSScanStatus(Client client)
        {
            return client.IsFSScanEnabled ? 1 : 0;
        }

        public int SendSetOption(Client client, int option, int value)
        {
            log.Info("Setoption " + option + " " + value);

            return 1;
        }

        public int DeleteFSScanRule(Client client, int id)
        {
            client.FSRules = client.FSRules.Where(rule => rule.RuleID != id).ToArray();
            return 1;
        }
        public int AddAppCtrlRule(Client client, AppCtrlRule rule)
        {
            rule.RuleID = client.AppCtrlRules.Length + 1;
            AppCtrlRule[] rules = client.AppCtrlRules;
            int len = client.AppCtrlRules.Length;

            Array.Resize(ref rules, rules.Length + 1);
            rules[len] = rule;

            client.AppCtrlRules = rules;
            return rule.RuleID;
        }

        public int DeleteAppCtrlRule(Client client, int id)
        {
            client.AppCtrlRules = client.AppCtrlRules.Where(rule => rule.RuleID != id).ToArray();
            return 1;
        }

        private void NotifyClientsChange()
        {
            if (null == ClientsChangedCallback) return;

            ClientsChangedCallback(clients);
        }

        private void LoadConfig()
        {
            serverIp = "192.168.194.1";
            port = 12345;
        }

        //private void StartServer()
        //{
        //    IPEndPoint localEndPoint = GetIPEndPoint();
        //    Socket listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        //    try
        //    {
        //        listener.Bind(localEndPoint);
        //        listener.Listen(100);

        //        while (true)
        //        {
        //            Console.WriteLine("Waiting for a connection...");
        //            Socket handler = listener.Accept();
        //            Console.WriteLine("Client connected");

        //            byte[] buffer = new byte[40];

        //            handler.Receive(buffer, 4, SocketFlags.None);
        //            int x = BitConverter.ToInt32(buffer, 0);

        //            handler.Receive(buffer, x, SocketFlags.None);
        //            string handshake = Encoding.ASCII.GetString(buffer);

        //            handler.Send(BitConverter.GetBytes(1));

        //            handler.Send(BitConverter.GetBytes(520));

        //            buffer = new byte[520];
        //            for (int i = 0; i < 520; i++)
        //            {
        //                buffer[i] = (byte)(i % 50);
        //            }

        //            handler.Send(buffer);

        //            handler.Shutdown(SocketShutdown.Both);
        //            handler.Close();
        //        }
        //    }
        //    catch (Exception ex)
        //    {
        //        Console.WriteLine(ex.ToString());
        //    }
        //}

        private IPEndPoint GetIPEndPoint()
        {
            IPAddress ipAddress = null;// ipHostInfo.AddressList[0];
            foreach (IPAddress ipAddr in Dns.GetHostEntry(Dns.GetHostName()).AddressList)
            {
                if (ipAddr.ToString().Equals(serverIp))
                {
                    ipAddress = ipAddr;
                }
            }

            return new IPEndPoint(ipAddress, port);
        }
    }
}
