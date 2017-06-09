using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;

namespace Manager.Log
{
    class Logger
    {
        private static volatile Logger instance;
        private static object syncInstance = new object();
        private static object syncFile = new object();
        private StreamWriter file;

        private Logger()
        {
            Directory.CreateDirectory("logs");
            file = new StreamWriter("logs\\IceManager.log", true);

            WriteStartMessage();

        }

        public static Logger Instance
        {
            get
            {
                if (instance == null)
                {
                    lock (syncInstance)
                    {
                        if (instance == null) instance = new Logger();
                    }
                }

                return instance;
            }
        }
        
        public void Info(string message)
        {
            WriteMessage("[INFO]", message);
        }

        public void Warning(string message)
        {
            WriteMessage("[WARNING]", message);
        }

        public void Error(string message)
        {
            WriteMessage("[ERROR]", message);
        }

        private void WriteMessage(string level, string message)
        {
            try
            {
                string messageToPrint = string.Format("MNG {0,-11} {1,-30} -- {2}", level, GetFunctionName(), message);

                lock (syncFile)
                {
                    file.WriteLine(messageToPrint);
                    file.Flush();
                }
            }
            catch
            {
                
            }
        }

        private string GetFunctionName()
        {
            try
            {
                MethodBase mt = (new StackTrace()).GetFrame(3).GetMethod();
                return mt.ReflectedType + "." + mt.ToString();
            }
            catch
            {
                return "Unknwon.Unknwon";
            }
        }

        private void WriteStartMessage()
        {
            string message = "\n\n------------------------ Log Started: " + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss") + " ------------------------\n";
            lock (syncFile)
            {
                file.WriteLine(message);
                file.Flush();
            }
        }
    }
}
