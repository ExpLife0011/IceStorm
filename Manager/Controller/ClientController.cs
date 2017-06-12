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

        public Client[] GetClients()
        {
            return server.GetClients();
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
            if (null == client)
            {
                throw new Exception("Invalid client.");
            }

            return server.GetAppCtrlRules(client);
        }

        public FSRule[] GetFSRules(Client client)
        {
            return server.GetFSRules(client);
        }

        public AppCtrlEvent[] GetAppCtrlEvents(Client client)
        {
            return server.GetAppCtrlEvents(client);
        }

        public FSEvent[] GetFSEvents(Client client)
        {
            return server.GetFSEvents(client);
        }

        public int EnableAppCtrl(Client client, int enable)
        {
            return server.EnableAppCtrl(client, enable);
        }

        public int GetAppCtrlStatus(Client client)
        {
            return server.GetAppCtrlStatus(client);
        }

        public int EnableFSScan(Client client, int enable)
        {
            return server.EnableFSScan(client, enable);
        }

        public int GetFSScanStatus(Client client)
        {
            return server.GetFSScanStatus(client);
        }
    }
}
