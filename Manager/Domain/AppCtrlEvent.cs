namespace Manager.Domain
{
    public class AppCtrlEvent
    {
        public int EventID { get; set; }

        public string ProcessPath { get; set; }
        public int PID { get; set; }

        public string ParentPath { get; set; }
        public int ParentPID { get; set; }

        public IceScanVerdict Verdict { get; set; }
        public int MatchedRuleID { get; set; }

        public int EventTime { get; set; }
    }
}
