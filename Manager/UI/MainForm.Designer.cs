namespace Manager.UI
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.mainLayout = new System.Windows.Forms.TableLayoutPanel();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.tabAppCtrlRules = new System.Windows.Forms.TabPage();
            this.tabFSRules = new System.Windows.Forms.TabPage();
            this.tabAppCtrlEvents = new System.Windows.Forms.TabPage();
            this.tabFSEvents = new System.Windows.Forms.TabPage();
            this.listClients = new System.Windows.Forms.ListView();
            this.groupBoxClient = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.lblName = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.lblIP = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.lblMAC = new System.Windows.Forms.Label();
            this.listAppRules = new System.Windows.Forms.ListView();
            this.listFSRules = new System.Windows.Forms.ListView();
            this.listAppEvents = new System.Windows.Forms.ListView();
            this.listFSEvents = new System.Windows.Forms.ListView();
            this.chAR1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chAR2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chAR3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chAR4 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chAR5 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chAR6 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chAR7 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chAR8 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chAR9 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.mainLayout.SuspendLayout();
            this.tabControl.SuspendLayout();
            this.tabAppCtrlRules.SuspendLayout();
            this.tabFSRules.SuspendLayout();
            this.tabAppCtrlEvents.SuspendLayout();
            this.tabFSEvents.SuspendLayout();
            this.groupBoxClient.SuspendLayout();
            this.SuspendLayout();
            // 
            // mainLayout
            // 
            this.mainLayout.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.mainLayout.AutoScroll = true;
            this.mainLayout.AutoSize = true;
            this.mainLayout.ColumnCount = 2;
            this.mainLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.mainLayout.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 80F));
            this.mainLayout.Controls.Add(this.tabControl, 1, 0);
            this.mainLayout.Controls.Add(this.listClients, 0, 0);
            this.mainLayout.Controls.Add(this.groupBoxClient, 0, 1);
            this.mainLayout.Location = new System.Drawing.Point(0, 0);
            this.mainLayout.Name = "mainLayout";
            this.mainLayout.RowCount = 2;
            this.mainLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.mainLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.mainLayout.Size = new System.Drawing.Size(1384, 660);
            this.mainLayout.TabIndex = 1;
            // 
            // tabControl
            // 
            this.tabControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl.Controls.Add(this.tabAppCtrlRules);
            this.tabControl.Controls.Add(this.tabFSRules);
            this.tabControl.Controls.Add(this.tabAppCtrlEvents);
            this.tabControl.Controls.Add(this.tabFSEvents);
            this.tabControl.Location = new System.Drawing.Point(279, 3);
            this.tabControl.Margin = new System.Windows.Forms.Padding(3, 3, 20, 3);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(1085, 324);
            this.tabControl.TabIndex = 1;
            // 
            // tabAppCtrlRules
            // 
            this.tabAppCtrlRules.Controls.Add(this.listAppRules);
            this.tabAppCtrlRules.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.tabAppCtrlRules.Location = new System.Drawing.Point(4, 22);
            this.tabAppCtrlRules.Name = "tabAppCtrlRules";
            this.tabAppCtrlRules.Padding = new System.Windows.Forms.Padding(3);
            this.tabAppCtrlRules.Size = new System.Drawing.Size(1077, 298);
            this.tabAppCtrlRules.TabIndex = 0;
            this.tabAppCtrlRules.Text = "AppCtrl Rules";
            this.tabAppCtrlRules.UseVisualStyleBackColor = true;
            // 
            // tabFSRules
            // 
            this.tabFSRules.Controls.Add(this.listFSRules);
            this.tabFSRules.Location = new System.Drawing.Point(4, 22);
            this.tabFSRules.Name = "tabFSRules";
            this.tabFSRules.Padding = new System.Windows.Forms.Padding(3);
            this.tabFSRules.Size = new System.Drawing.Size(1077, 298);
            this.tabFSRules.TabIndex = 1;
            this.tabFSRules.Text = "FS Rules";
            this.tabFSRules.UseVisualStyleBackColor = true;
            // 
            // tabAppCtrlEvents
            // 
            this.tabAppCtrlEvents.Controls.Add(this.listAppEvents);
            this.tabAppCtrlEvents.Location = new System.Drawing.Point(4, 22);
            this.tabAppCtrlEvents.Name = "tabAppCtrlEvents";
            this.tabAppCtrlEvents.Padding = new System.Windows.Forms.Padding(3);
            this.tabAppCtrlEvents.Size = new System.Drawing.Size(1077, 298);
            this.tabAppCtrlEvents.TabIndex = 2;
            this.tabAppCtrlEvents.Text = "AppCtrl Events";
            this.tabAppCtrlEvents.UseVisualStyleBackColor = true;
            // 
            // tabFSEvents
            // 
            this.tabFSEvents.Controls.Add(this.listFSEvents);
            this.tabFSEvents.Location = new System.Drawing.Point(4, 22);
            this.tabFSEvents.Name = "tabFSEvents";
            this.tabFSEvents.Padding = new System.Windows.Forms.Padding(3);
            this.tabFSEvents.Size = new System.Drawing.Size(767, 298);
            this.tabFSEvents.TabIndex = 3;
            this.tabFSEvents.Text = "FS Events";
            this.tabFSEvents.UseVisualStyleBackColor = true;
            // 
            // listClients
            // 
            this.listClients.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listClients.AutoArrange = false;
            this.listClients.FullRowSelect = true;
            this.listClients.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None;
            this.listClients.Location = new System.Drawing.Point(3, 3);
            this.listClients.MultiSelect = false;
            this.listClients.Name = "listClients";
            this.listClients.Size = new System.Drawing.Size(270, 324);
            this.listClients.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.listClients.TabIndex = 2;
            this.listClients.UseCompatibleStateImageBehavior = false;
            this.listClients.View = System.Windows.Forms.View.Details;
            this.listClients.SelectedIndexChanged += new System.EventHandler(this.listClients_SelectedIndexChanged);
            // 
            // groupBoxClient
            // 
            this.groupBoxClient.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBoxClient.Controls.Add(this.lblMAC);
            this.groupBoxClient.Controls.Add(this.label5);
            this.groupBoxClient.Controls.Add(this.lblIP);
            this.groupBoxClient.Controls.Add(this.label3);
            this.groupBoxClient.Controls.Add(this.lblName);
            this.groupBoxClient.Controls.Add(this.label1);
            this.groupBoxClient.Location = new System.Drawing.Point(3, 333);
            this.groupBoxClient.Margin = new System.Windows.Forms.Padding(3, 3, 3, 50);
            this.groupBoxClient.Name = "groupBoxClient";
            this.groupBoxClient.Size = new System.Drawing.Size(270, 277);
            this.groupBoxClient.TabIndex = 3;
            this.groupBoxClient.TabStop = false;
            this.groupBoxClient.Text = "Client Info";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 31);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(38, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Name:";
            // 
            // lblName
            // 
            this.lblName.AutoSize = true;
            this.lblName.Location = new System.Drawing.Point(53, 31);
            this.lblName.Name = "lblName";
            this.lblName.Size = new System.Drawing.Size(10, 13);
            this.lblName.TabIndex = 1;
            this.lblName.Text = "-";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(27, 49);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(20, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "IP:";
            // 
            // lblIP
            // 
            this.lblIP.AutoSize = true;
            this.lblIP.Location = new System.Drawing.Point(53, 49);
            this.lblIP.Name = "lblIP";
            this.lblIP.Size = new System.Drawing.Size(10, 13);
            this.lblIP.TabIndex = 3;
            this.lblIP.Text = "-";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(14, 66);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(33, 13);
            this.label5.TabIndex = 4;
            this.label5.Text = "MAC:";
            // 
            // lblMAC
            // 
            this.lblMAC.AutoSize = true;
            this.lblMAC.Location = new System.Drawing.Point(53, 66);
            this.lblMAC.Name = "lblMAC";
            this.lblMAC.Size = new System.Drawing.Size(10, 13);
            this.lblMAC.TabIndex = 5;
            this.lblMAC.Text = "-";
            // 
            // listAppRules
            // 
            this.listAppRules.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listAppRules.AutoArrange = false;
            this.listAppRules.BackColor = System.Drawing.SystemColors.Window;
            this.listAppRules.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chAR1,
            this.chAR2,
            this.chAR3,
            this.chAR4,
            this.chAR5,
            this.chAR6,
            this.chAR7,
            this.chAR8,
            this.chAR9});
            this.listAppRules.FullRowSelect = true;
            this.listAppRules.Location = new System.Drawing.Point(0, 0);
            this.listAppRules.MultiSelect = false;
            this.listAppRules.Name = "listAppRules";
            this.listAppRules.Size = new System.Drawing.Size(1077, 298);
            this.listAppRules.TabIndex = 0;
            this.listAppRules.UseCompatibleStateImageBehavior = false;
            this.listAppRules.View = System.Windows.Forms.View.Details;
            // 
            // listFSRules
            // 
            this.listFSRules.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listFSRules.AutoArrange = false;
            this.listFSRules.FullRowSelect = true;
            this.listFSRules.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.listFSRules.Location = new System.Drawing.Point(0, 0);
            this.listFSRules.MultiSelect = false;
            this.listFSRules.Name = "listFSRules";
            this.listFSRules.Size = new System.Drawing.Size(1077, 298);
            this.listFSRules.TabIndex = 0;
            this.listFSRules.UseCompatibleStateImageBehavior = false;
            this.listFSRules.View = System.Windows.Forms.View.Details;
            // 
            // listAppEvents
            // 
            this.listAppEvents.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listAppEvents.AutoArrange = false;
            this.listAppEvents.FullRowSelect = true;
            this.listAppEvents.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.Nonclickable;
            this.listAppEvents.Location = new System.Drawing.Point(0, 0);
            this.listAppEvents.MultiSelect = false;
            this.listAppEvents.Name = "listAppEvents";
            this.listAppEvents.Size = new System.Drawing.Size(1077, 298);
            this.listAppEvents.TabIndex = 0;
            this.listAppEvents.UseCompatibleStateImageBehavior = false;
            this.listAppEvents.View = System.Windows.Forms.View.Details;
            // 
            // listFSEvents
            // 
            this.listFSEvents.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listFSEvents.AutoArrange = false;
            this.listFSEvents.FullRowSelect = true;
            this.listFSEvents.Location = new System.Drawing.Point(0, 0);
            this.listFSEvents.MultiSelect = false;
            this.listFSEvents.Name = "listFSEvents";
            this.listFSEvents.Size = new System.Drawing.Size(767, 298);
            this.listFSEvents.TabIndex = 0;
            this.listFSEvents.UseCompatibleStateImageBehavior = false;
            this.listFSEvents.View = System.Windows.Forms.View.Details;
            // 
            // chAR1
            // 
            this.chAR1.Text = "ID";
            this.chAR1.Width = 30;
            // 
            // chAR2
            // 
            this.chAR2.Text = "Proc Matcher";
            this.chAR2.Width = 80;
            // 
            // chAR3
            // 
            this.chAR3.Text = "Process Path";
            this.chAR3.Width = 300;
            // 
            // chAR4
            // 
            this.chAR4.Text = "PID";
            this.chAR4.Width = 80;
            // 
            // chAR5
            // 
            this.chAR5.Text = "Parent Path Matcher";
            // 
            // chAR6
            // 
            this.chAR6.Text = "Parent Path";
            this.chAR6.Width = 300;
            // 
            // chAR7
            // 
            this.chAR7.Text = "Parent PID";
            // 
            // chAR8
            // 
            this.chAR8.Text = "Verdict";
            // 
            // chAR9
            // 
            this.chAR9.Text = "Add Time";
            this.chAR9.Width = 100;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1384, 661);
            this.Controls.Add(this.mainLayout);
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "IceStorm Manager";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.mainLayout.ResumeLayout(false);
            this.tabControl.ResumeLayout(false);
            this.tabAppCtrlRules.ResumeLayout(false);
            this.tabFSRules.ResumeLayout(false);
            this.tabAppCtrlEvents.ResumeLayout(false);
            this.tabFSEvents.ResumeLayout(false);
            this.groupBoxClient.ResumeLayout(false);
            this.groupBoxClient.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TableLayoutPanel mainLayout;
        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage tabAppCtrlRules;
        private System.Windows.Forms.TabPage tabFSRules;
        private System.Windows.Forms.TabPage tabAppCtrlEvents;
        private System.Windows.Forms.TabPage tabFSEvents;
        private System.Windows.Forms.ListView listClients;
        private System.Windows.Forms.GroupBox groupBoxClient;
        private System.Windows.Forms.Label lblMAC;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label lblIP;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lblName;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ListView listAppRules;
        private System.Windows.Forms.ListView listFSRules;
        private System.Windows.Forms.ListView listAppEvents;
        private System.Windows.Forms.ListView listFSEvents;
        private System.Windows.Forms.ColumnHeader chAR1;
        private System.Windows.Forms.ColumnHeader chAR2;
        private System.Windows.Forms.ColumnHeader chAR3;
        private System.Windows.Forms.ColumnHeader chAR4;
        private System.Windows.Forms.ColumnHeader chAR5;
        private System.Windows.Forms.ColumnHeader chAR6;
        private System.Windows.Forms.ColumnHeader chAR7;
        private System.Windows.Forms.ColumnHeader chAR8;
        private System.Windows.Forms.ColumnHeader chAR9;
    }
}