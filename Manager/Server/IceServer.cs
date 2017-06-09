using Manager.Log;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Manager.Server
{
    public class IceServer
    {
        private ManualResetEvent stopEvent;
        private Thread threadListen;
        private Logger log = Logger.Instance;

        public void StartServer()
        {
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

        private void ListenThread()
        {
            log.Info("ListenThread started");

            while (true)
            {
                if (stopEvent.WaitOne(1000)) break;
                log.Info("nu");
            }

            log.Info("ListenThread stopped");
        }
    }
}
