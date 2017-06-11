namespace Manager.Domain
{
    public class AppCtrlRule
    {
        public int RuleID { get; set; }
        public IceScanVerdict Verdict { get; set; }

        public IceStringMatcher ProcessPathMatcher { get; set; }
        public string ProcessPath { get; set; }
        public int PID { get; set; }

        public IceStringMatcher ParentPathMatcher { get; set; }
        public string ParentPath { get; set; }
        public int ParentPID { get; set; }

        public int AddTime { get; set; }
    }
}
