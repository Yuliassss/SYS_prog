using System.Drawing;
using System.Windows.Forms;
using System.Xml.Linq;
using static System.Net.Mime.MediaTypeNames;

namespace serkova1CSharp
{
    partial class Form1
    {
        private System.ComponentModel.IContainer components = null;
        private ListBox listBox;
        private NumericUpDown numericUpDown;
        private Button buttonStart;
        private Button buttonStop;


        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        private void InitializeComponent()
        {
            listBox = new ListBox();
            numericUpDown = new NumericUpDown();
            buttonStart = new Button();
            buttonStop = new Button();
          
            ((System.ComponentModel.ISupportInitialize)numericUpDown).BeginInit();
            SuspendLayout();
            // 
            // listBox
            // 
            listBox.Dock = DockStyle.Left;
            listBox.FormattingEnabled = true;
            listBox.Location = new Point(0, 0);
            listBox.Name = "listBox";
            listBox.Size = new Size(187, 433);
            listBox.TabIndex = 0;
            // 
            // numericUpDown
            // 
            numericUpDown.Location = new Point(193, 12);
            numericUpDown.Name = "numericUpDown";
            numericUpDown.Size = new Size(58, 27);
            numericUpDown.TabIndex = 1;
            numericUpDown.TextAlign = HorizontalAlignment.Right;
            numericUpDown.Value = new decimal(new int[] { 2, 0, 0, 0 });
            // 
            // buttonStart
            // 
            buttonStart.Location = new Point(193, 45);
            buttonStart.Name = "buttonStart";
            buttonStart.Size = new Size(94, 29);
            buttonStart.TabIndex = 2;
            buttonStart.Text = "Start";
            buttonStart.UseVisualStyleBackColor = true;
            buttonStart.Click += buttonStart_Click;
            // 
            // buttonStop
            // 
            buttonStop.Location = new Point(293, 45);
            buttonStop.Name = "buttonStop";
            buttonStop.Size = new Size(94, 29);
            buttonStop.TabIndex = 3;
            buttonStop.Text = "Stop";
            buttonStop.UseVisualStyleBackColor = true;
            buttonStop.Click += buttonStop_Click;
           
            // 
            // Form1
            // 
            AutoScaleDimensions = new SizeF(8F, 20F);
            AutoScaleMode = AutoScaleMode.Font;
            ClientSize = new Size(622, 433);
            Controls.Add(buttonStop);
            Controls.Add(buttonStart);
            Controls.Add(numericUpDown);
            Controls.Add(listBox);
            MinimumSize = new Size(640, 480);
            Name = "Form1";
            Text = "Серкова АС-23-05 ЛР1";
            FormClosed += Form1_FormClosed;
            ((System.ComponentModel.ISupportInitialize)numericUpDown).EndInit();
            ResumeLayout(false);
            PerformLayout();
        }
    }
}