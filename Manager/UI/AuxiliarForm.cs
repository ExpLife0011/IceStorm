using Manager.Domain;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Windows.Forms.ListBox;

namespace Manager.UI
{
    public enum AuxFormType
    {
        SetOption,
        AddAppCtrl,
        UpdateAppCtrl,
        AddFSScan,
        UpdateFSScan
    }

    public enum AuxFormResult
    {
        Completed,
        Canceled,
        Error
    }

    public partial class AuxiliarForm : Form
    {
        private AuxFormType formType;
        private int nrOfRows;
        private Control[] controls;
        private Label[] labels;
        private AppCtrlRule appRule;
        private FSRule fsRule;

        public AuxFormResult Result { get; private set; }
        public string[] InitialValues { private get; set; }
        public string[] Values { get; private set; }
        public Action<string[]> ValidateFunction { get; set; }

        public AuxiliarForm(AuxFormType formType)
        {
            this.formType = formType;
            Result = AuxFormResult.Canceled;
            InitializeComponent();
            CreateFields();
        }

        public AuxiliarForm(AuxFormType formType, AppCtrlRule appRule)
        {
            this.appRule = appRule;
            this.formType = formType;
            Result = AuxFormResult.Canceled;
            InitializeComponent();
            CreateFields();
        }

        public AuxiliarForm(AuxFormType formType, FSRule fsRule)
        {
            this.fsRule = fsRule;
            this.formType = formType;
            Result = AuxFormResult.Canceled;
            InitializeComponent();
            CreateFields();
        }

        private void CreateFields()
        {
            CreateLabels();

            controls = new Control[nrOfRows];

            switch (formType)
            {
                case AuxFormType.SetOption:
                    controls[0] = new NumericUpDown();
                    controls[1] = new NumericUpDown();
                    break;

                case AuxFormType.AddAppCtrl:
                case AuxFormType.UpdateAppCtrl:
                    controls[0] = CreateIceStringMatcherSelect();
                    controls[1] = new TextBox();
                    controls[1].Anchor = AnchorStyles.Right;
                    controls[2] = new NumericUpDown();
                    (controls[2] as NumericUpDown).Maximum = int.MaxValue;
                    controls[3] = CreateIceStringMatcherSelect();
                    controls[4] = new TextBox();
                    controls[4].Anchor = AnchorStyles.Right;
                    controls[5] = new NumericUpDown();
                    (controls[5] as NumericUpDown).Maximum = int.MaxValue;
                    controls[6] = CreateIceScanVerdictSelect();
                    break;

                case AuxFormType.AddFSScan:
                case AuxFormType.UpdateFSScan:
                    controls[0] = CreateIceStringMatcherSelect();
                    controls[1] = new TextBox();
                    controls[1].Anchor = AnchorStyles.Right;
                    controls[2] = new NumericUpDown();
                    (controls[2] as NumericUpDown).Maximum = int.MaxValue;
                    controls[3] = CreateIceStringMatcherSelect();
                    controls[4] = new TextBox();
                    controls[4].Anchor = AnchorStyles.Right;
                    controls[5] = CreateIceFSOperationsSelect();
                    mainLayout.RowStyles[5].Height = 90;
                    mainLayout.RowStyles[6].Height = 10;
                    mainLayout.RowStyles[7].Height = 10;
                    break;
                
                default:
                    return;
            }

            if (formType == AuxFormType.UpdateAppCtrl)
            {
                InitialValues = new string[] {
                    appRule.ProcessPathMatcher.ToString(),
                    appRule.ProcessPath,
                    appRule.PID.ToString(),
                    appRule.ParentPathMatcher.ToString(),
                    appRule.ParentPath,
                    appRule.ParentPID.ToString(),
                    appRule.Verdict.ToString()
                };

                (controls[0] as ComboBox).SelectedIndex = InitialValues[0].Equals("Equal") ? 0 : 1;
                controls[1].Text = InitialValues[1];
                controls[2].Text = InitialValues[2];
                (controls[3] as ComboBox).SelectedIndex = InitialValues[3].Equals("Equal") ? 0 : 1;
                controls[4].Text = InitialValues[4];
                controls[5].Text = InitialValues[5];
                (controls[6] as ComboBox).SelectedIndex = InitialValues[6].Equals("Allow") ? 0 : 1;
            }
            else if (formType == AuxFormType.UpdateFSScan)
            {
                InitialValues = new string[] {
                    fsRule.ProcessPathMatcher.ToString(),
                    fsRule.ProcessPath,
                    fsRule.PID.ToString(),
                    fsRule.FilePathMatcher.ToString(),
                    fsRule.FilePath,
                    fsRule.DeniedOperations.ToString()
                };

                (controls[0] as ComboBox).SelectedIndex = InitialValues[0].Equals("Equal") ? 0 : 1;
                controls[1].Text = InitialValues[1];
                controls[2].Text = InitialValues[2];
                (controls[3] as ComboBox).SelectedIndex = InitialValues[3].Equals("Equal") ? 0 : 1;
                controls[4].Text = InitialValues[4];
                SetListBoxSelectedIndex((controls[5] as ListBox), int.Parse(InitialValues[5]));
            }

            for (int i = 0; i < nrOfRows; i++)
            {
                controls[i].Anchor |= AnchorStyles.Left;
                mainLayout.Controls.Add(controls[i], 1, i);
            }
        }

        private void SetListBoxSelectedIndex(ListBox listBox, int flags)
        {
            for (int i = 0; i < 6; i++)
            {
                listBox.SetSelected(i, false);
            }

            if (flags == (int) IceFSFlags.None)
            {
                listBox.SetSelected(0, true);
            }

            if ((flags & (int) IceFSFlags.Create) != 0) listBox.SetSelected(1, true);
            if ((flags & (int)IceFSFlags.Open) != 0) listBox.SetSelected(2, true);
            if ((flags & (int)IceFSFlags.Write) != 0) listBox.SetSelected(3, true);
            if ((flags & (int)IceFSFlags.Read) != 0) listBox.SetSelected(4, true);
            if ((flags & (int)IceFSFlags.Delete) != 0) listBox.SetSelected(5, true);
        }

        private ListBox CreateIceFSOperationsSelect()
        {
            ListBox lbx = new ListBox();
            lbx.AllowDrop = true;
            lbx.SelectionMode = SelectionMode.MultiSimple;

            lbx.Items.AddRange(Enum.GetNames(typeof(IceFSFlags)));
            return lbx;
        }

        private ComboBox CreateIceScanVerdictSelect()
        {
            ComboBox cbx = new ComboBox();
            cbx.AllowDrop = true;
            cbx.DropDownStyle = ComboBoxStyle.DropDownList;
            
            cbx.Items.AddRange(Enum.GetNames(typeof(IceScanVerdict)));
            return cbx;
        }

        private ComboBox CreateIceStringMatcherSelect()
        {
            ComboBox cbx = new ComboBox();
            cbx.AllowDrop = true;
            cbx.DropDownStyle = ComboBoxStyle.DropDownList;
            
            cbx.Items.AddRange(Enum.GetNames(typeof(IceStringMatcher)));
            return cbx;
        }

        private void CreateLabels()
        {
            string[] lblText;
            
            switch (formType)
            {
                case AuxFormType.SetOption:
                    lblText = new string[] { "Option", "Value" };
                    nrOfRows = lblText.Length;
                    break;

                case AuxFormType.AddAppCtrl:
                case AuxFormType.UpdateAppCtrl:
                    lblText = new string[] { "Process Matcher", "Process Path", "PID", "Parent Matcher", "Parent Path", "Parent PID", "Verdict" };
                    nrOfRows = lblText.Length;
                    break;

                case AuxFormType.AddFSScan:
                case AuxFormType.UpdateFSScan:
                    lblText = new string[] { "Process Matcher", "Process Path", "PID", "File Matcher", "File Path", "Denied Flags" };
                    nrOfRows = lblText.Length;
                    break;

                default:
                    return;
            }

            labels = new Label[nrOfRows];
            for (int i = 0; i < nrOfRows; i++)
            {
                labels[i] = new Label();
                labels[i].Anchor = AnchorStyles.Right;
                labels[i].Text = lblText[i] + ":";
                labels[i].AutoSize = false;
                labels[i].TextAlign = ContentAlignment.MiddleRight;
                mainLayout.Controls.Add(labels[i], 0, i);
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            CompleteResultAndClose(AuxFormResult.Canceled);
        }

        private void btnComplete_Click(object sender, EventArgs e)
        {
            Values = new string[nrOfRows];
            string errors = "";

            switch (formType)
            {
                case AuxFormType.AddAppCtrl:
                case AuxFormType.UpdateAppCtrl:
                    if (string.IsNullOrEmpty(controls[0].Text)) errors += "\"" + labels[0].Text.Remove(labels[0].Text.Length - 1) + "\", ";
                    if (string.IsNullOrEmpty(controls[2].Text)) errors += "\"" + labels[2].Text.Remove(labels[2].Text.Length - 1) + "\", ";
                    if (string.IsNullOrEmpty(controls[3].Text)) errors += "\"" + labels[3].Text.Remove(labels[3].Text.Length - 1) + "\", ";
                    if (string.IsNullOrEmpty(controls[5].Text)) errors += "\"" + labels[5].Text.Remove(labels[5].Text.Length - 1) + "\", ";
                    if (string.IsNullOrEmpty(controls[6].Text)) errors += "\"" + labels[6].Text.Remove(labels[6].Text.Length - 1) + "\", ";
                    break;

                case AuxFormType.AddFSScan:
                case AuxFormType.UpdateFSScan:
                    if (string.IsNullOrEmpty(controls[0].Text)) errors += "\"" + labels[0].Text.Remove(labels[0].Text.Length - 1) + "\", ";
                    if (string.IsNullOrEmpty(controls[2].Text)) errors += "\"" + labels[2].Text.Remove(labels[2].Text.Length - 1) + "\", ";
                    if (string.IsNullOrEmpty(controls[3].Text)) errors += "\"" + labels[3].Text.Remove(labels[3].Text.Length - 1) + "\", ";
                    if (string.IsNullOrEmpty(controls[5].Text)) errors += "\"" + labels[5].Text.Remove(labels[5].Text.Length - 1) + "\", ";
                    break;
            }

            for (int i = 0; i < nrOfRows; i++)
            {
                string text = controls[i].Text;
                if ((formType == AuxFormType.AddFSScan || formType == AuxFormType.UpdateFSScan) && i == 5) text = GetListBoxText(5);

                Values[i] = string.IsNullOrEmpty(text) ? null : text;
            }

            if (string.IsNullOrEmpty(errors))
            {
                try
                {
                    ValidateFunction?.Invoke(Values);
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Error");
                    return;
                }

                CompleteResultAndClose(AuxFormResult.Completed);
                return;
            }

            errors = errors.Remove(errors.Length - 2);
            errors = "The next fields must be filled: " + errors + ".";

            MessageBox.Show(errors, "Error");
        }

        private string GetListBoxText(int idx)
        {
            ListBox lbx = controls[idx] as ListBox;
            string str = "";

            foreach (int i in lbx.SelectedIndices)
            {
                str += lbx.Items[i].ToString();
            }

            if (string.IsNullOrEmpty(str)) return null;

            int flag = 0;
            string[] enumStr = Enum.GetNames(typeof(IceFSFlags));
            IceFSFlags[] enumVal = (IceFSFlags[]) Enum.GetValues(typeof(IceFSFlags));

            for (int i = 0; i < enumStr.Length; i++)
            {
                if (str.Contains(enumStr[i]))
                {
                    flag |= (int) enumVal[i];
                }
            }

            return flag.ToString();
        }

        private void CompleteResultAndClose(AuxFormResult result)
        {
            Result = result;
            Close();
        }
    }
}
