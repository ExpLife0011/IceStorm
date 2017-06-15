using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Manager.Server
{
    enum IceServerCommand : int
    {
        Error               = 0x0,

        Ping                = 0x1,

        GetAppCtrlStatus    = 0x2,
        GetFSScanStatus     = 0x4,

        SetAppCtrlStatus    = 0x8,
        SetFSScanStatus     = 0x10
    }

    enum IceServerCommandResult : int
    {
        Error               = 0x0,
        Success             = 0x1
    }
}
