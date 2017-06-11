namespace Manager.Domain
{
    public class FSEvent
    {
        public int EventID { get; set; }

        public string ProcessPath { get; set; }
        public int PID { get; set; }
        public string FilePath { get; set; }

        public int RequiredOperations { get; set; }
        public int DeniedOperations { get; set; }
        public int RemainingOperations { get; set; }

        public int MatchedRuleID { get; set; }
        public int EventTime { get; set; }
    }
}
