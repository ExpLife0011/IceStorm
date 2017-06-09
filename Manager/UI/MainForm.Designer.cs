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
            this.listClients = new System.Windows.Forms.ListView();
            this.tabControl = new System.Windows.Forms.TabControl();
            this.tabAppCtrlRules = new System.Windows.Forms.TabPage();
            this.tabFSRules = new System.Windows.Forms.TabPage();
            this.tabAppCtrlEvents = new System.Windows.Forms.TabPage();
            this.tabFSEvents = new System.Windows.Forms.TabPage();
            this.mainLayout.SuspendLayout();
            this.tabControl.SuspendLayout();
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
            this.mainLayout.Controls.Add(this.listClients, 0, 0);
            this.mainLayout.Controls.Add(this.tabControl, 1, 0);
            this.mainLayout.Location = new System.Drawing.Point(0, 0);
            this.mainLayout.Name = "mainLayout";
            this.mainLayout.RowCount = 2;
            this.mainLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.mainLayout.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.mainLayout.Size = new System.Drawing.Size(984, 660);
            this.mainLayout.TabIndex = 1;
            // 
            // listClients
            // 
            this.listClients.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
            | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listClients.Location = new System.Drawing.Point(3, 3);
            this.listClients.Name = "listClients";
            this.listClients.Size = new System.Drawing.Size(187, 344);
            this.listClients.TabIndex = 0;
            this.listClients.UseCompatibleStateImageBehavior = false;
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
            this.tabControl.Location = new System.Drawing.Point(196, 3);
            this.tabControl.Name = "tabControl";
            this.tabControl.SelectedIndex = 0;
            this.tabControl.Size = new System.Drawing.Size(768, 344);
            this.tabControl.TabIndex = 1;
            // 
            // tabAppCtrlRules
            // 
            this.tabAppCtrlRules.Location = new System.Drawing.Point(4, 22);
            this.tabAppCtrlRules.Name = "tabAppCtrlRules";
            this.tabAppCtrlRules.Padding = new System.Windows.Forms.Padding(3);
            this.tabAppCtrlRules.Size = new System.Drawing.Size(760, 318);
            this.tabAppCtrlRules.TabIndex = 0;
            this.tabAppCtrlRules.Text = "AppCtrl Rules";
            this.tabAppCtrlRules.UseVisualStyleBackColor = true;
            // 
            // tabFSRules
            // 
            this.tabFSRules.Location = new System.Drawing.Point(4, 22);
            this.tabFSRules.Name = "tabFSRules";
            this.tabFSRules.Padding = new System.Windows.Forms.Padding(3);
            this.tabFSRules.Size = new System.Drawing.Size(760, 318);
            this.tabFSRules.TabIndex = 1;
            this.tabFSRules.Text = "FS Rules";
            this.tabFSRules.UseVisualStyleBackColor = true;
            // 
            // tabAppCtrlEvents
            // 
            this.tabAppCtrlEvents.Location = new System.Drawing.Point(4, 22);
            this.tabAppCtrlEvents.Name = "tabAppCtrlEvents";
            this.tabAppCtrlEvents.Padding = new System.Windows.Forms.Padding(3);
            this.tabAppCtrlEvents.Size = new System.Drawing.Size(760, 318);
            this.tabAppCtrlEvents.TabIndex = 2;
            this.tabAppCtrlEvents.Text = "AppCtrl Events";
            this.tabAppCtrlEvents.UseVisualStyleBackColor = true;
            // 
            // tabFSEvents
            // 
            this.tabFSEvents.Location = new System.Drawing.Point(4, 22);
            this.tabFSEvents.Name = "tabFSEvents";
            this.tabFSEvents.Padding = new System.Windows.Forms.Padding(3);
            this.tabFSEvents.Size = new System.Drawing.Size(760, 318);
            this.tabFSEvents.TabIndex = 3;
            this.tabFSEvents.Text = "FS Events";
            this.tabFSEvents.UseVisualStyleBackColor = true;
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(984, 661);
            this.Controls.Add(this.mainLayout);
            this.Name = "MainForm";
            this.Text = "IceStorm Manager";
            this.mainLayout.ResumeLayout(false);
            this.tabControl.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.TableLayoutPanel mainLayout;
        private System.Windows.Forms.ListView listClients;
        private System.Windows.Forms.TabControl tabControl;
        private System.Windows.Forms.TabPage tabAppCtrlRules;
        private System.Windows.Forms.TabPage tabFSRules;
        private System.Windows.Forms.TabPage tabAppCtrlEvents;
        private System.Windows.Forms.TabPage tabFSEvents;
    }
}