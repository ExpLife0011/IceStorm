namespace Manager.Domain
{
    public class Client
    {
        public int ClientID { get; set; }

        public string Name { get; set; }
        public string IP { get; set; }
        public string MAC { get; set; }
        public string OS { get; set; }
        public string Platform { get; set; }

        public AppCtrlRule[] AppCtrlRules { get; set;  }
        public AppCtrlEvent[] AppCtrlEvents { get; set; }
        public FSRule[] FSRules { get; set; }
        public FSEvent[] FSEvents { get; set; }

        public bool IsAppCtrlEnabled { get; set; }
        public bool IsFSScanEnabled { get; set; }
    }
}
