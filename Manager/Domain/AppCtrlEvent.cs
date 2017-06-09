namespace Manager.Domain
{
    public class AppCtrlEvent
    {
        public int EventId { get; set; }

        public string ProcessPath { get; set; }
        public string Pid { get; set; }

        public string ParentPath { get; set; }
        public int ParentPid { get; set; }

        public IceScanVerdict Verdict { get; set; }
        public int MatchedRuleId { get; set; }

        public int EventTime { get; set; }
    }
}
