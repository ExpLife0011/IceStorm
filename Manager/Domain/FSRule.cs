namespace Manager.Domain
{
    public class FSRule
    {
        public int RuleId { get; set; }
        public int DeniedOperations { get; set; }

        public IceStringMatcher ProcessPathMatcher { get; set; }
        public string ProcessPath { get; set; }
        public int Pid { get; set; }

        public IceStringMatcher FilePathMatcher { get; set; }
        public string FilePath { get; set; }

        public int AddTime { get; set; }
    }
}
