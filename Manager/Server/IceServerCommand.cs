namespace Manager.Server
{
    enum IceServerCommand : int
    {
        Error               = 0x0,

        Ping                = 0x1,

        GetAppCtrlStatus    = 0x2,
        GetFSScanStatus     = 0x4,

        SetAppCtrlStatus    = 0x8,
        SetFSScanStatus     = 0x10,

        GetAppCtrlEvents    = 0x20,
        GetAppCtrlRules     = 0x40,
        AddAppCtrlRule      = 0x80,
        UpdateAppCtrlRule   = 0x100,
        DeleteAppCtrlRule   = 0x200,

        GetFSScanEvents     = 0x400,
        GetFSScanRules      = 0x800,
        AddFSScanRule       = 0x1000,
        UpdateFSScanRule    = 0x2000,
        DeleteFSScanRule    = 0x4000,
        
        SetOption           = 0x8000
    }

    enum IceServerCommandResult : int
    {
        Error               = 0x0,
        Success             = 0x1
    }
}
