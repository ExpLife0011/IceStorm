namespace Manager.Domain
{
    public class FSRule
    {
        public int RuleID { get; set; }
        public int DeniedOperations { get; set; }

        public IceStringMatcher ProcessPathMatcher { get; set; }
        public string ProcessPath { get; set; }
        public int PID { get; set; }

        public IceStringMatcher FilePathMatcher { get; set; }
        public string FilePath { get; set; }

        public int AddTime { get; set; }
    }
}
