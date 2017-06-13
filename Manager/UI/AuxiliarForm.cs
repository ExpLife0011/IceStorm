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

        public AuxFormResult Result { get; private set; }
        public string[] Values { get; private set; }
        public Action<string[]> ValidateFunction { get; set; }

        public AuxiliarForm(AuxFormType formType)
        {
            this.formType = formType;
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
                    controls[0] = CreateIceStringMatcherSelect();
                    controls[1] = new TextBox();
                    controls[1].Anchor = AnchorStyles.Right;
                    controls[2] = new NumericUpDown();
                    controls[3] = CreateIceStringMatcherSelect();
                    controls[4] = new TextBox();
                    controls[4].Anchor = AnchorStyles.Right;
                    controls[5] = new NumericUpDown();
                    controls[6] = CreateIceScanVerdictSelect();
                    break;

                default:
                    return;
            }

            for (int i = 0; i < nrOfRows; i++)
            {
                controls[i].Anchor |= AnchorStyles.Left;
                mainLayout.Controls.Add(controls[i], 1, i);
            }
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
                    labels = new Label[nrOfRows];
                    break;

                case AuxFormType.AddAppCtrl:
                    lblText = new string[] { "Process Matcher", "Process Path", "PID", "Parent Matcher", "Parent Path", "Parent PID", "Verdict" };
                    nrOfRows = lblText.Length;
                    labels = new Label[nrOfRows];
                    break;

                default:
                    return;
            }

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

            for (int i = 0; i < nrOfRows; i++)
            {
                if (string.IsNullOrEmpty(controls[i].Text)) errors += "\"" + labels[i].Text.Remove(labels[i].Text.Length - 1) + "\", ";
                Values[i] = controls[i].Text;
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

        private void CompleteResultAndClose(AuxFormResult result)
        {
            Result = result;
            Close();
        }
    }
}
