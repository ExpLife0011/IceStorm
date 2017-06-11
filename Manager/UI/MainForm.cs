using System;
using System.Text;
using System.Windows.Forms;
using Manager.Controller;
using Manager.Domain;
using Manager.Log;
using System.ComponentModel;
using System.Threading;

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
                GetNeededList(clients[listClients.SelectedIndices[0]]);
            }
        }

        private void tabControl_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listClients.SelectedIndices.Count == 0)
            {
                ResetAllLists();
            }
            else
            {
                GetNeededList(clients[listClients.SelectedIndices[0]]);
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

        private void GetNeededList(Client client)
        {
            switch (tabControl.SelectedIndex)
            {
                case 0:
                    GetAppCtrlRulesList(client);
                    break;
                case 1:
                    GetFSRulesList(client);
                    break;
                case 2:
                    GetAppCtrlEventsList(client);
                    break;
                case 3:
                    GetFSEventsList(client);
                    break;
                default:
                    ResetAllLists();
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

        private void GetAppCtrlRulesList(Client client)
        {
            BackgroundWorker bw = new BackgroundWorker();

            bw.WorkerReportsProgress = false;
            bw.WorkerSupportsCancellation = false;

            bw.DoWork += new DoWorkEventHandler((object sender, DoWorkEventArgs e) =>
            {
                e.Result = ctrl.GetAppCtrlRules(client);
            });

            bw.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender, RunWorkerCompletedEventArgs e) =>
            {
                if (e.Error != null)
                {
                    MessageBox.Show(e.Error.Message, "Error");
                    return;
                }

                SetAppCtrlRulesList(e.Result as AppCtrlRule[]);
            });

            bw.RunWorkerAsync();
        }

        private void SetAppCtrlRulesList(AppCtrlRule[] appRules)
        {
            listAppRules.Items.Clear();
            foreach (AppCtrlRule r in appRules)
            {
                string[] row = {
                    r.RuleId.ToString(), r.ProcessPathMatcher.ToString(), r.ProcessPath, r.PID.ToString(),
                    r.ParentPathMatcher.ToString(), r.ParentPath.ToString(), r.ParentPID.ToString(), 
                    r.Verdict.ToString(), r.AddTime.ToString() 
                };
                ListViewItem lvi = new ListViewItem(row);
                listAppRules.Items.Add(lvi);
            }
        }

        private void GetFSRulesList(Client client)
        {
            BackgroundWorker bw = new BackgroundWorker();

            bw.WorkerReportsProgress = false;
            bw.WorkerSupportsCancellation = false;

            bw.DoWork += new DoWorkEventHandler((object sender, DoWorkEventArgs e) =>
            {
                e.Result = ctrl.GetFSRules(client);
            });

            bw.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender, RunWorkerCompletedEventArgs e) =>
            {
                if (e.Error != null)
                {
                    MessageBox.Show(e.Error.Message, "Error");
                    return;
                }

                SetFSRulesList(e.Result as FSRule[]);
            });

            bw.RunWorkerAsync();
        }

        private void SetFSRulesList(FSRule[] fsRules)
        {
            listFSRules.Items.Clear();
            foreach (FSRule r in fsRules)
            {
                string[] row = {
                    r.RuleId.ToString(), r.ProcessPathMatcher.ToString(), r.ProcessPath, r.PID.ToString(),
                    r.FilePathMatcher.ToString(), r.FilePath.ToString(), r.DeniedOperations.ToString(), r.AddTime.ToString()
                };
                ListViewItem lvi = new ListViewItem(row);
                listFSRules.Items.Add(lvi);
            }
        }

        private void GetAppCtrlEventsList(Client client)
        {
            BackgroundWorker bw = new BackgroundWorker();

            bw.WorkerReportsProgress = false;
            bw.WorkerSupportsCancellation = false;

            bw.DoWork += new DoWorkEventHandler((object sender, DoWorkEventArgs e) =>
            {
                e.Result = ctrl.GetAppCtrlEvents(client);
            });

            bw.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender, RunWorkerCompletedEventArgs e) =>
            {
                if (e.Error != null)
                {
                    MessageBox.Show(e.Error.Message, "Error");
                    return;
                }

                SetAppCtrlEventsList(e.Result as AppCtrlEvent[]);
            });

            bw.RunWorkerAsync();
        }

        private void SetAppCtrlEventsList(AppCtrlEvent[] appEvents)
        {
            listAppEvents.Items.Clear();
            foreach (AppCtrlEvent e in appEvents)
            {
                string[] row = {
                    e.EventId.ToString(), e.ProcessPath, e.PID.ToString(), e.ParentPath, e.ParentPID.ToString(),
                    e.Verdict.ToString(), e.MatchedRuleId.ToString(), e.EventTime.ToString()
                };
                ListViewItem lvi = new ListViewItem(row);
                listAppEvents.Items.Add(lvi);
            }
        }

        private void GetFSEventsList(Client client)
        {
            BackgroundWorker bw = new BackgroundWorker();

            bw.WorkerReportsProgress = false;
            bw.WorkerSupportsCancellation = false;

            bw.DoWork += new DoWorkEventHandler((object sender, DoWorkEventArgs e) =>
            {
                e.Result = ctrl.GetFSEvents(client);
            });

            bw.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender, RunWorkerCompletedEventArgs e) =>
            {
                if (e.Error != null)
                {
                    MessageBox.Show(e.Error.Message, "Error");
                    return;
                }

                SetFSEventList(e.Result as FSEvent[]);
            });

            bw.RunWorkerAsync();
        }

        private void SetFSEventList(FSEvent[] fsEvent)
        {
            listFSEvents.Items.Clear();
            foreach (FSEvent e in fsEvent)
            {
                string[] row = {
                    e.EventID.ToString(), e.ProcessPath, e.PID.ToString(), e.FilePath.ToString(),
                    e.RequiredOperations.ToString(), e.DeniedOperations.ToString(), e.RequiredOperations.ToString(),
                    e.MatchedRuleId.ToString(), e.EventTime.ToString()
                };
                ListViewItem lvi = new ListViewItem(row);
                listFSEvents.Items.Add(lvi);
            }
        }

    }
}
