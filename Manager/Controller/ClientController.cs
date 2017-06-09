using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Manager.Server;

namespace Manager.Controller
{
    public class ClientController
    {
        private IceServer server;

        public ClientController(IceServer server)
        {
            this.server = server;
        }
    }
}
