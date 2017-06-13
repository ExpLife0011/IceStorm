using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Manager.Domain
{
    public enum IceFSFlags
    {
        None    = 0x0,
        Create  = 0x1,
        Open    = 0x2,
        Write   = 0x4,
        Read    = 0x8,
        Delete  = 0x10
    }
}
