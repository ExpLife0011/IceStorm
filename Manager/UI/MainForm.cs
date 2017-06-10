using System;
using System.Text;
using System.Windows.Forms;
using Manager.Controller;
using Manager.Domain;
using Manager.Log;

namespace Manager.UI
{
    public partial class MainForm : Form
    {
        private Logger log = Logger.Instance;

        private ClientController ctrl;
        private Client[] clients;

        private delegate void ClientsListChangedCallback(Client[] clients, object context);

        public MainForm(ClientController ctrl)
        {
            this.ctrl = ctrl;

            InitializeComponent();
            InitFields();
        }

        private void MainForm_Load(object sender, EventArgs e)
        {
            InitCallbacks();
        }

        private void InitFields()
        {
            mainLayout.Width = Width;
            mainLayout.Height = Height;

            InitHeaders();
        }

        private void InitHeaders()
        {
            ColumnHeader chClients = new ColumnHeader();
            chClients.Width = mainLayout.GetControlFromPosition(0, 0).Width - 10;
            listClients.Columns.Add(chClients);
        }

        private void InitCallbacks()
        {
            ctrl.SetClientsChangedCallback(ClientsListChanged, null);
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            ctrl.SetClientsChangedCallback(null, null);
        }

        private void ClientsListChanged(Client[] clients, object context)
        {
            if (!IsHandleCreated) return;
            
            try
            {
                this.clients = clients;

                BeginInvoke(new ClientsListChangedCallback((Client[] lst, object ctx) =>
                    {
                        listClients.Items.Clear();
                        ResetClientInfo();
                        ResetAllLists();

                        foreach (Client c in lst)
                        {
                            listClients.Items.Add(new ListViewItem(c.Name));
                        }
                    }), 
                    clients, context);
            }
            catch (Exception ex)
            {
                log.Error("ClientsListChanged->BeginInvoke: " + ex.Message);
            }
            
        }

        private void listClients_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listClients.SelectedIndices.Count == 0)
            {
                ResetClientInfo();
                ResetAllLists();
            }
            else
            {
                SetClientInfo();
                GetNeededList();
            }
        }

        private void SetClientInfo()
        {
            Client c = clients[listClients.SelectedIndices[0]];
            Label[] lblLst = { lblName, lblMAC, lblIP };
            string[] txtLst = { c.Name, c.MAC, c.Ip };

            for (int i = 0; i < lblLst.Length; i++)
            { 
                lblLst[i].Text = string.IsNullOrEmpty(txtLst[i]) ? "-" : txtLst[i];
            }
        }

        private void ResetClientInfo()
        {
            Label[] lst = { lblName, lblMAC, lblIP };
            foreach (Label l in lst)
            {
                l.Text = "-";
            }
        }

        private void GetNeededList()
        {
            switch (tabControl.SelectedIndex)
            {
                case 0:
                    GetAppCtrlRulesList();
                    break;
                case 1:
                    
                    break;
                case 2:
                    
                    break;
                case 3:
                    
                    break;
                default:
                    break;
            }
        }

        private void ResetAllLists()
        {
            ListView[] lst = { listAppRules, listFSRules, listAppEvents, listFSEvents };
            foreach (ListView lv in lst)
            {
                lv.Items.Clear();
            }
        }

        private void GetAppCtrlRulesList()
        {
            AppCtrlRule[] fakeAppRules = new AppCtrlRule[10];
            
            for (int i = 0; i < fakeAppRules.Length; i++)
            {
                fakeAppRules[i] = new AppCtrlRule();

                fakeAppRules[i].RuleId = i + 1;
                fakeAppRules[i].ProcessPath = "C:\\path\\proces" + i + ".exe";
                fakeAppRules[i].ParentPath =  "C:\\path\\parinte" + i + ".exe";
                fakeAppRules[i].ParentPID = (i + 1) * 50 + i;
                fakeAppRules[i].Pid = (i + 1) * 50;
                fakeAppRules[i].ProcessPathMatcher = ((i % 2) == 1) ? IceStringMatcher.Equal : IceStringMatcher.Wildmat;
                fakeAppRules[i].ParentPathMatcher = ((i % 2) == 1) ? IceStringMatcher.Equal : IceStringMatcher.Wildmat;
                fakeAppRules[i].AddTime = 6000 + i;
                fakeAppRules[i].Verdict = ((i % 2) == 1) ? IceScanVerdict.Allow : IceScanVerdict.Deny;
            }

            SetAppCtrlList(fakeAppRules);
        }

        private void SetAppCtrlList(AppCtrlRule[] appRules)
        {
            listAppRules.Items.Clear();
            foreach (AppCtrlRule r in appRules)
            {
                string[] row = {
                    r.RuleId.ToString(), r.ProcessPathMatcher.ToString(), r.ProcessPath, r.Pid.ToString(),
                    r.ParentPathMatcher.ToString(), r.ParentPath.ToString(), r.ParentPID.ToString(), 
                    r.Verdict.ToString(), r.AddTime.ToString() 
                };
                ListViewItem lvi = new ListViewItem(row);
                listAppRules.Items.Add(lvi);
            }
        }
    }
}
