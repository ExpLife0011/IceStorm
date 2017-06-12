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
                
                default:
                    break;
            }

            for (int i = 0; i < nrOfRows; i++)
            {
                controls[i].Anchor = AnchorStyles.Left;
                mainLayout.Controls.Add(controls[i], 1, i);
            }
        }

        private void CreateLabels()
        {
            string[] lblText;
            
            switch (formType)
            {
                case AuxFormType.SetOption:
                    nrOfRows = 2;
                    labels = new Label[nrOfRows];
                    lblText = new string[] { "Option", "Value" };
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
                if (string.IsNullOrEmpty(controls[i].Text)) errors += "\"" + labels[i].Text + "\", ";
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
