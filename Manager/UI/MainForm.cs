using System;
using System.Windows.Forms;
using Manager.Controller;
using Manager.Domain;
using Manager.Log;
using System.ComponentModel;

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
            SetRuleInfo();
            ResetClientInfo();
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
            SetRuleInfo();
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
            SetRuleInfo();
        }

        private void SetClientInfo()
        {
            Client c = clients[listClients.SelectedIndices[0]];
            Label[] lblLst = { lblName, lblMAC, lblIP, lblOS, lblPlatform };
            string[] txtLst = { c.Name, c.MAC, c.IP, c.OS, c.Platform };

            for (int i = 0; i < lblLst.Length; i++)
            {
                lblLst[i].Text = string.IsNullOrEmpty(txtLst[i]) ? "-" : txtLst[i];
            }

            Control[] lst2 = { lblAppStatus, lblFSStatus, btnAppStatus, btnFSScanStatus, btnSetOption };
            foreach (Control ct in lst2)
            {
                ct.Visible = true;
            }

            SetAppCtrlStatus(c.IsAppCtrlEnabled);
            SetFSScanStatus(c.IsFSScanEnabled);
        }

        private void ResetClientInfo()
        {
            Label[] lst = { lblName, lblMAC, lblIP, lblOS, lblPlatform };
            foreach (Label l in lst)
            {
                l.Text = "-";
            }

            Control[] lst2 = { lblAppStatus, lblFSStatus, btnAppStatus, btnFSScanStatus, btnSetOption };
            foreach (Control c in lst2)
            {
                c.Visible = false;
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
                SetRuleInfo();
            });

            bw.RunWorkerAsync();
        }

        private void SetAppCtrlRulesList(AppCtrlRule[] appRules)
        {
            listAppRules.Items.Clear();
            foreach (AppCtrlRule r in appRules)
            {
                string[] row = {
                    r.RuleID.ToString(), r.ProcessPathMatcher.ToString(), r.ProcessPath, r.PID.ToString(),
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
                SetRuleInfo();
            });

            bw.RunWorkerAsync();
        }

        private void SetFSRulesList(FSRule[] fsRules)
        {
            listFSRules.Items.Clear();
            foreach (FSRule r in fsRules)
            {
                string[] row = {
                    r.RuleID.ToString(), r.ProcessPathMatcher.ToString(), r.ProcessPath, r.PID.ToString(),
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
                    e.EventID.ToString(), e.ProcessPath, e.PID.ToString(), e.ParentPath, e.ParentPID.ToString(),
                    e.Verdict.ToString(), e.MatchedRuleID.ToString(), e.EventTime.ToString()
                };
                ListViewItem lvi = new ListViewItem(row);
                if (e.Verdict == IceScanVerdict.Deny) lvi.ForeColor = System.Drawing.Color.Red;
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
                    e.MatchedRuleID.ToString(), e.EventTime.ToString()
                };
                ListViewItem lvi = new ListViewItem(row);
                if (e.DeniedOperations != 0) lvi.ForeColor = System.Drawing.Color.Red;
                listFSEvents.Items.Add(lvi);
            }
        }

        private void listAppRules_SelectedIndexChanged(object sender, EventArgs e)
        {
            SetRuleInfo();
            if (listAppRules.SelectedIndices.Count == 0) return;
            groupBoxRule.Text = "Application Control Rule";
        }

        private void listFSRules_SelectedIndexChanged(object sender, EventArgs e)
        {
            SetRuleInfo();
            if (listFSRules.SelectedIndices.Count == 0) return;
            groupBoxRule.Text = "File System Rule";
        }

        private void listAppEvents_SelectedIndexChanged(object sender, EventArgs e)
        {
            SetRuleInfo();
            if (listAppEvents.SelectedIndices.Count == 0) return;
            groupBoxRule.Text = "Application Control Events";
        }

        private void listFSEvents_SelectedIndexChanged(object sender, EventArgs e)
        {
            SetRuleInfo();
            if (listFSEvents.SelectedIndices.Count == 0) return;
            groupBoxRule.Text = "File System Event";
        }

        private void SetRuleInfo()
        {
            ListView lv;
            if (listClients.SelectedIndices.Count == 0)
            {
                groupBoxRule.Visible = false;
            }
            else
            {
                groupBoxRule.Visible = true;
                btnAdd.Enabled = false;
                btnUpdate.Enabled = false;
                btnDelete.Enabled = false;
            }

            switch (tabControl.SelectedIndex)
            {
                case 0:
                    lv = listAppRules;
                    break;
                case 1:
                    lv = listFSRules;
                    break;
                default:
                    return;
            }

            btnAdd.Enabled = true;

            if (lv.SelectedIndices.Count == 0) return;

            btnUpdate.Enabled = true;
            btnDelete.Enabled = true;
        }
        
        private void SetAppCtrlStatus(bool isEnabled)
        {
            lblAppStatus.Text = "AppCtrl: " + (isEnabled ? "Enabled" : "Disabled");
            lblAppStatus.ForeColor = (isEnabled ? System.Drawing.Color.Green : System.Drawing.Color.Red);
            btnAppStatus.Text = (isEnabled ? "Disable AppCtrl" : "Enable AppCtrl");
        }

        private void SetFSScanStatus(bool isEnabled)
        {
            lblFSStatus.Text = "FSScan: " + (isEnabled ? "Enabled" : "Disabled");
            lblFSStatus.ForeColor = (isEnabled ? System.Drawing.Color.Green : System.Drawing.Color.Red);
            btnFSScanStatus.Text = (isEnabled ? "Disable FSScan" : "Enable FSScan");
        }

        private void btnAppStatus_Click(object sender, EventArgs e)
        {
            Client client = clients[listClients.SelectedIndices[0]];
            BackgroundWorker bw = new BackgroundWorker();
            
            bw.DoWork += new DoWorkEventHandler((object sender2, DoWorkEventArgs e2) =>
            {
                e2.Result = ctrl.EnableAppCtrl(client, client.IsAppCtrlEnabled ? 0 : 1);
            });

            bw.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender2, RunWorkerCompletedEventArgs e2) =>
            {
                if (e2.Error != null)
                {
                    MessageBox.Show(e2.Error.Message, "Error");
                    return;
                }
                
                BackgroundWorker bw2 = new BackgroundWorker();
                bw2.DoWork += new DoWorkEventHandler((object sender3, DoWorkEventArgs e3) =>
                {
                    e3.Result = ctrl.GetAppCtrlStatus(client);
                });

                bw2.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender3, RunWorkerCompletedEventArgs e3) =>
                {
                    if (e3.Error != null)
                    {
                        MessageBox.Show(e2.Error.Message, "Error");
                        return;
                    }

                    bool isEnabled = (int) e3.Result == 1 ? true : false;
                    client.IsAppCtrlEnabled = isEnabled;
                    SetAppCtrlStatus(isEnabled);
                });

                bw2.RunWorkerAsync();
                bool hadSuccess = (int)e2.Result == 1 ? true : false;
                MessageBox.Show("AppCtrl status change " + (hadSuccess ? "had success" : "failed"), hadSuccess ? "Success" : "Error");
            });

            bw.RunWorkerAsync();
        }

        private void btnFSScanStatus_Click(object sender, EventArgs e)
        {
            Client client = clients[listClients.SelectedIndices[0]];
            BackgroundWorker bw = new BackgroundWorker();

            bw.DoWork += new DoWorkEventHandler((object sender2, DoWorkEventArgs e2) =>
            {
                e2.Result = ctrl.EnableFSScan(client, client.IsFSScanEnabled ? 0 : 1);
            });

            bw.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender2, RunWorkerCompletedEventArgs e2) =>
            {
                if (e2.Error != null)
                {
                    MessageBox.Show(e2.Error.Message, "Error");
                    return;
                }

                BackgroundWorker bw2 = new BackgroundWorker();
                bw2.DoWork += new DoWorkEventHandler((object sender3, DoWorkEventArgs e3) =>
                {
                    e3.Result = ctrl.GetFSScanStatus(client);
                });

                bw2.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender3, RunWorkerCompletedEventArgs e3) =>
                {
                    if (e3.Error != null)
                    {
                        MessageBox.Show(e2.Error.Message, "Error");
                        return;
                    }

                    bool isEnabled = (int)e3.Result == 1 ? true : false;
                    client.IsFSScanEnabled = isEnabled;
                    SetFSScanStatus(isEnabled);
                });

                bw2.RunWorkerAsync();
                bool hadSuccess = (int)e2.Result == 1 ? true : false;
                MessageBox.Show("FSScan status change " + (hadSuccess ? "had success" : "failed"), hadSuccess ? "Success" : "Error");
            });

            bw.RunWorkerAsync();
        }

        private void btnSetOption_Click(object sender, EventArgs e)
        {
            AuxiliarForm auxForm = new AuxiliarForm(AuxFormType.SetOption);
            auxForm.Text = "Set Option";

            auxForm.ShowDialog();
            if (auxForm.Result != AuxFormResult.Completed) return;

            string[] values = auxForm.Values;

            try
            {
                int option = int.Parse(values[0]);
                int value = int.Parse(values[1]);
                Client client = clients[listClients.SelectedIndices[0]];

                BackgroundWorker bw = new BackgroundWorker();

                bw.DoWork += new DoWorkEventHandler((object sender2, DoWorkEventArgs e2) =>
                {
                    e2.Result = ctrl.SendSetOption(client, option, value);
                });

                bw.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender2, RunWorkerCompletedEventArgs e2) =>
                {
                    if (e2.Error != null)
                    {
                        MessageBox.Show(e2.Error.Message, "Error");
                        return;
                    }

                    int result = (int)e2.Result;
                    bool hadSuccess = result == 1;
                    MessageBox.Show(
                        string.Format("Set Option {0} {1} {2}", option, value, (hadSuccess ? "had success" : "failed")),
                        hadSuccess ? "Success" : "Error"
                        );
                });

                bw.RunWorkerAsync();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error");
            }
        }

        private void btnAdd_Click(object sender, EventArgs e)
        {
            string ruleType = tabControl.SelectedIndex == 0 ? "AppCtrl Rule" : "FSScan Rule";
            ListView lv = tabControl.SelectedIndex == 0 ? listAppRules : listFSRules;
            int selectedIndex = tabControl.SelectedIndex;
            AuxiliarForm auxForm = new AuxiliarForm(selectedIndex == 0 ? AuxFormType.AddAppCtrl : AuxFormType.AddFSScan);
            
            auxForm.Text = "Add Application Control Rule";
            auxForm.ShowDialog();

            if (auxForm.Result != AuxFormResult.Completed) return;

            string[] values = auxForm.Values;
            if (selectedIndex == 0) AddAppCtrlRule(values);
            else AddFSScanRule(values);

        }

        private void AddFSScanRule(string[] values)
        {
            
        }

        private void AddAppCtrlRule(string[] values)
        {
            try
            {
                Client client = clients[listClients.SelectedIndices[0]];
                AppCtrlRule rule = new AppCtrlRule();

                rule.ProcessPathMatcher = values[0].Equals("Equals") ? IceStringMatcher.Equal : IceStringMatcher.Wildmat;
                rule.ProcessPath = values[1];
                rule.PID = int.Parse(values[2]);
                rule.ParentPathMatcher = values[3].Equals("Equals") ? IceStringMatcher.Equal : IceStringMatcher.Wildmat;
                rule.ParentPath = values[4];
                rule.ParentPID = int.Parse(values[5]);
                rule.Verdict = values[6].Equals("Allow") ? IceScanVerdict.Allow : IceScanVerdict.Deny;

                BackgroundWorker bw = new BackgroundWorker();

                bw.DoWork += new DoWorkEventHandler((object sender2, DoWorkEventArgs e2) =>
                {
                    e2.Result = ctrl.AddAppCtrlRule(client, rule);
                });

                bw.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender2, RunWorkerCompletedEventArgs e2) =>
                {
                    if (e2.Error != null)
                    {
                        MessageBox.Show(e2.Error.Message, "Error");
                        return;
                    }


                    BackgroundWorker bw2 = new BackgroundWorker();
                    bw2.DoWork += new DoWorkEventHandler((object sender3, DoWorkEventArgs e3) =>
                    {
                        e3.Result = ctrl.GetAppCtrlRules(client);
                    });

                    bw2.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender3, RunWorkerCompletedEventArgs e3) =>
                    {
                        if (e3.Error != null)
                        {
                            MessageBox.Show(e3.Error.Message, "Error");
                            return;
                        }
                        SetAppCtrlRulesList(e3.Result as AppCtrlRule[]);
                    });

                    bw2.RunWorkerAsync();

                    int ruleId = (int)e2.Result;
                    MessageBox.Show(string.Format("AppCtrl rule with id {0} was added with success.", ruleId), "Success");
                });

                bw.RunWorkerAsync();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Error");
            }
        }

        private void btnUpdate_Click(object sender, EventArgs e)
        {

        }

        private void btnDelete_Click(object sender, EventArgs e)
        {
            string ruleType = tabControl.SelectedIndex == 0 ? "AppCtrl Rule" : "FSScan Rule";
            ListView lv = tabControl.SelectedIndex == 0 ? listAppRules : listFSRules;
            int selectedIndex = tabControl.SelectedIndex;
            int id = 0;
            int.TryParse(lv.SelectedItems[0].SubItems[0].Text, out id);

            var confirmResult = MessageBox.Show(string.Format("Are you sure to delete {0} with id {1}?", ruleType, id), "Delete " + ruleType, MessageBoxButtons.YesNo);
            if (confirmResult == DialogResult.No) return;

            Client client = clients[listClients.SelectedIndices[0]];
            BackgroundWorker bw = new BackgroundWorker();

            bw.DoWork += new DoWorkEventHandler((object sender2, DoWorkEventArgs e2) =>
            {
                e2.Result = selectedIndex == 0 ? ctrl.DeleteAppCtrlRule(client, id) : ctrl.DeleteFSScanRule(client, id);
            });
            
            bw.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender2, RunWorkerCompletedEventArgs e2) =>
            {
                if (e2.Error != null)
                {
                    MessageBox.Show(e2.Error.Message, "Error");
                    return;
                }
                
                if ((int)e2.Result != 1)
                {
                    MessageBox.Show(string.Format(string.Format("Delete of {0} with {0} failed."), ruleType, id), "Error");
                    return;
                }

                SetRuleInfo();
                if (selectedIndex == 0) GetAppCtrlRulesList(client);
                else GetFSRulesList(client);
                SetRuleInfo();

                //BackgroundWorker bw2 = new BackgroundWorker();

                //bw2.DoWork += new DoWorkEventHandler((object sender3, DoWorkEventArgs e3) =>
                //{
                //    if (selectedIndex == 0) e3.Result = ctrl.GetAppCtrlRules(client);
                //    else e3.Result = ctrl.GetFSRules(client);
                //});

                //bw2.RunWorkerCompleted += new RunWorkerCompletedEventHandler((object sender3, RunWorkerCompletedEventArgs e3) =>
                //{
                //    if (selectedIndex == 0)
                //    {
                //        SetAppCtrlRulesList(e3.Result as AppCtrlRule[]);
                //    }
                //    else
                //    {
                //        SetFSRulesList(e3.Result as FSRule[]);
                //    }
                //});

                //bw2.RunWorkerAsync();
            });

            bw.RunWorkerAsync();
        }
    }
}
