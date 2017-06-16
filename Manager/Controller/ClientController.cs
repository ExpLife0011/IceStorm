using System;
using Manager.Server;
using Manager.Domain;
using Manager.Log;

namespace Manager.Controller
{
    public class ClientController
    {
        private IceServer server;
        private Logger log = Logger.Instance;

        private object clientsChangedContext;
        private Action<Client[], object> clientsChangedCallback;

        public ClientController(IceServer server)
        {
            this.server = server;
            this.server.ClientsChangedCallback = ClientsListChanged;
        }
        
        public void SetClientsChangedCallback(Action<Client[], object> callback, object context)
        {
            clientsChangedCallback = callback;
            clientsChangedContext = context;
        }

        private void ClientsListChanged(Client[] clients)
        {
            if (null == clientsChangedCallback) return;

            clientsChangedCallback(clients, clientsChangedContext);
        }

        public AppCtrlRule[] GetAppCtrlRules(Client client)
        {
            lock (client.SyncAccess)
            {
                return server.GetAppCtrlRules(client);
            }
        }

        public FSRule[] GetFSRules(Client client)
        {
            lock (client.SyncAccess)
            {
                return server.GetFSRules(client);
            }
        }

        public AppCtrlEvent[] GetAppCtrlEvents(Client client)
        {
            lock (client.SyncAccess)
            {
                int lastId = 0;
                if (client.AppCtrlEvents != null && client.AppCtrlEvents.Length != 0)
                {
                    lastId = client.AppCtrlEvents[0].EventID;
                }
                else
                {
                    client.AppCtrlEvents = new AppCtrlEvent[0];
                }
                
                AppCtrlEvent[] newAppEvents = server.GetAppCtrlEvents(client, lastId);
                if (newAppEvents == null || newAppEvents.Length == 0)
                    return client.AppCtrlEvents;

                AppCtrlEvent[] oldAppEvents = client.AppCtrlEvents;
                int finalLen = oldAppEvents.Length + newAppEvents.Length;
                AppCtrlEvent[] finalAppEvents = new AppCtrlEvent[finalLen];
                
                Array.Copy(newAppEvents, 0, finalAppEvents, 0, newAppEvents.Length);
                Array.Copy(oldAppEvents, 0, finalAppEvents, newAppEvents.Length, oldAppEvents.Length);
                
                client.AppCtrlEvents = finalAppEvents;
                return finalAppEvents;
            }
        }

        public FSEvent[] GetFSEvents(Client client)
        {
            lock (client.SyncAccess)
            {
                return server.GetFSEvents(client);
            }
        }

        public int EnableAppCtrl(Client client, int enable)
        {
            lock (client.SyncAccess)
            {
                return server.EnableAppCtrl(client, enable);
            }
        }

        public int GetAppCtrlStatus(Client client)
        {
            lock (client.SyncAccess)
            {
                return server.GetAppCtrlStatus(client);
            }
        }

        public int EnableFSScan(Client client, int enable)
        {
            lock (client.SyncAccess)
            {
                return server.EnableFSScan(client, enable);
            }
        }

        public int GetFSScanStatus(Client client)
        {
            lock (client.SyncAccess)
            {
                return server.GetFSScanStatus(client);
            }
        }

        public int SendSetOption(Client client, int option, int value)
        {
            lock (client.SyncAccess)
            {
                return server.SendSetOption(client, option, value);
            }
        }

        public int DeleteAppCtrlRule(Client client, int id)
        {
            lock (client.SyncAccess)
            {
                return server.DeleteAppCtrlRule(client, id);
            }
        }

        public int DeleteFSScanRule(Client client, int id)
        {
            lock (client.SyncAccess)
            {
                return server.DeleteFSScanRule(client, id);
            }
        }

        public int AddAppCtrlRule(Client client, AppCtrlRule rule)
        {
            lock (client.SyncAccess)
            {
                return server.AddAppCtrlRule(client, rule);
            }
        }

        public int AddFSScanRule(Client client, FSRule rule)
        {
            lock (client.SyncAccess)
            {
                return server.AddFSScanRule(client, rule);
            }
        }

        public int UpdateAppCtrlRule(Client client, AppCtrlRule rule)
        {
            lock (client.SyncAccess)
            {
                return server.UpdateAppCtrlRule(client, rule);
            }
        }

        public int UpdateFSScanRule(Client client, FSRule rule)
        {
            lock (client.SyncAccess)
            {
                return server.UpdateFSScanRule(client, rule);
            }
        }
    }
}
