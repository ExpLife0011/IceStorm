using Manager.Log;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Manager.Server
{
    class SocketHelper
    {
        private ManualResetEvent stopEvent;
        private Logger log = Logger.Instance;

        public SocketHelper(ref ManualResetEvent stopEvent)
        {
            this.stopEvent = stopEvent;
        }

        public string RecvString(Socket s)
        {
            byte[] buffer = RecvMessage(s, sizeof(int));
            int size = BitConverter.ToInt32(buffer, 0);

            if (size == 0) return null;

            buffer = RecvMessage(s, size);
            return Encoding.Unicode.GetString(buffer);
        }

        public int RecvDWORD(Socket s)
        {
            byte[] buffer = RecvMessage(s, sizeof(int));
            int value = BitConverter.ToInt32(buffer, 0);

            return value;
        }

        private byte[] RecvMessage(Socket s, int size)
        {
            byte[] buffer = new byte[size];
            int totalRecvSize = 0;
            int remainingSize = size;
            int recvSize = 0;

            while (true)
            {
                try
                {
                    recvSize = s.Receive(buffer, totalRecvSize, remainingSize, SocketFlags.None);

                    remainingSize -= recvSize;
                    totalRecvSize += recvSize;
                    
                    log.Info(string.Format("Recieved {0} ({1}) / {2} from {3}", recvSize, totalRecvSize, size, s.RemoteEndPoint));

                    if (remainingSize == 0) break;
                }
                catch (SocketException ex)
                {
                    if (ex.ErrorCode != 10035)
                    {
                        log.Error("RecvMessage: " + ex.Message);
                        throw;
                    }
                }
                catch (Exception ex)
                {
                    log.Error("RecvMessage: " + ex.Message);
                    throw;
                }

                if (stopEvent.WaitOne(10))
                {
                    throw new OperationCanceledException("Stop event signaled");
                }
            }

            return buffer;
        }


        public void SendDWORD(Socket s, int value)
        {
            byte[] buffer = BitConverter.GetBytes(value);
            SendMessage(s, buffer, sizeof(int));
        }

        private void SendMessage(Socket s, byte[] buffer, int size)
        {
            int totalSendSize = 0;
            int remainingSize = size;
            int sentSize = 0;

            while (true)
            {
                try
                {
                    sentSize = s.Send(buffer, totalSendSize, remainingSize, SocketFlags.None);

                    remainingSize -= sentSize;
                    totalSendSize += sentSize;

                    log.Info(string.Format("Sent {0} ({1}) / {2} to {3}", sentSize, totalSendSize, size, s.RemoteEndPoint));

                    if (remainingSize == 0) break;
                }
                catch (SocketException ex)
                {
                    if (ex.ErrorCode != 10035)
                    {
                        log.Error("SendMessage: " + ex.Message);
                        throw;
                    }
                }
                catch (Exception ex)
                {
                    log.Error("SendMessage: " + ex.Message);
                    throw;
                }

                if (stopEvent.WaitOne(10))
                {
                    throw new OperationCanceledException("Stop event signaled");
                }
            }
        }
    }
}
