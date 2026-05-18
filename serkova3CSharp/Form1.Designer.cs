using System.Drawing;
using System.Windows.Forms;

namespace serkova3CSharp
{
    partial class Form1
    {
        private System.ComponentModel.IContainer components = null;
        private ListBox listBox;
        private NumericUpDown numericUpDown;
        private Button buttonStart;
        private Button buttonStop;
        private TextBox textBox;
        private Button buttonSend;

        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null)) components.Dispose();
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            listBox       = new ListBox();
            numericUpDown = new NumericUpDown();
            buttonStart   = new Button();
            buttonStop    = new Button();
            textBox       = new TextBox();
            buttonSend    = new Button();

            ((System.ComponentModel.ISupportInitialize)numericUpDown).BeginInit();
            SuspendLayout();

            listBox.Dock = DockStyle.Left;
            listBox.Size = new Size(200, 433);
            listBox.FormattingEnabled = true;

            numericUpDown.Location = new Point(215, 12);
            numericUpDown.Size = new Size(60, 27);
            numericUpDown.Minimum = 1;
            numericUpDown.Maximum = 20;
            numericUpDown.Value = 2;
            numericUpDown.TextAlign = HorizontalAlignment.Right;

            buttonStart.Location = new Point(215, 45);
            buttonStart.Size = new Size(94, 29);
            buttonStart.Text = "Start";
            buttonStart.Click += buttonStart_Click;

            buttonStop.Location = new Point(315, 45);
            buttonStop.Size = new Size(94, 29);
            buttonStop.Text = "Stop";
            buttonStop.Enabled = false;
            buttonStop.Click += buttonStop_Click;

            textBox.Location = new Point(215, 90);
            textBox.Size = new Size(330, 27);
            textBox.Enabled = false;

            buttonSend.Location = new Point(215, 125);
            buttonSend.Size = new Size(94, 29);
            buttonSend.Text = "Send";
            buttonSend.Enabled = false;
            buttonSend.Click += buttonSend_Click;

            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(622, 433);
            Controls.Add(buttonSend);
            Controls.Add(textBox);
            Controls.Add(buttonStop);
            Controls.Add(buttonStart);
            Controls.Add(numericUpDown);
            Controls.Add(listBox);
            MinimumSize = new Size(640, 480);
            Name = "Form1";
            Text = "Серкова АС-23-05 ЛР3";
            FormClosed += Form1_FormClosed;

            ((System.ComponentModel.ISupportInitialize)numericUpDown).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }
    }
}
