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
    }
}
