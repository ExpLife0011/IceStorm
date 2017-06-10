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
                            clients[j].Name = "Machine " + j;
                            clients[j].Ip = string.Format("{0}.{0}.{0}.{0}", j);
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
                
                if (stopEvent.WaitOne(5000)) break;
                log.Info("nu");
            }

            if (restartServer && !stopEvent.WaitOne(50))
            {
                log.Info("Will restart the server");
                StartServer();
            }

            log.Info("ListenThread stopped");
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
