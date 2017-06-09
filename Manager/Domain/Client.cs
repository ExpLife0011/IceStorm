namespace Manager.Domain
{
    class Client
    {
        public string Name { get; set; }
        public string Ip { get; set; }
        public string MAC { get; set; }

        public AppCtrlRule[] AppCtrlRules { get; set;  }
        public AppCtrlEvent[] AppCtrlEvents { get; set; }
        public FSRule[] FSRules { get; set; }
        public FSEvent[] FSEvents { get; set; }
    }
}
