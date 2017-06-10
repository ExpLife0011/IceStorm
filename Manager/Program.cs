using Manager.Controller;
using Manager.Log;
using Manager.Server;
using Manager.UI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Manager
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            Logger log = Logger.Instance;
            IceServer server = new IceServer();
            server.StartServer();

            try
            {
                ClientController ctrl = new ClientController(server);

                Application.Run(new MainForm(ctrl));
            }
            catch (Exception ex)
            {
                log.Error("Failed to start the program: " + ex.ToString());
            }

            server.StopServer();
            log.Close();
        }
    }
}
