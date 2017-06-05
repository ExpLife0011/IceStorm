using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Manager.ui
{
    public partial class WaitForServerStartForm : Form
    {
        private string serverIp = "";
        private int port = 0;

        public WaitForServerStartForm()
        {
            InitializeComponent();

            LoadConfig();
            StartServer();
        }

        private void LoadConfig()
        {
            serverIp = "192.168.194.1";
            port = 12345;
        }

        private void StartServer()
        {
            IPEndPoint localEndPoint = GetIPEndPoint();
            Socket listener = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

            try
            {
                listener.Bind(localEndPoint);
                listener.Listen(100);

                while (true)
                {
                    Console.WriteLine("Waiting for a connection...");
                    Socket handler = listener.Accept();

                    Console.WriteLine("Client connected");

                    handler.Shutdown(SocketShutdown.Both);
                    handler.Close();
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
        }

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
