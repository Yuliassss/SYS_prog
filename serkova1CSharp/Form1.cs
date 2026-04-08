using System;
using System.Diagnostics;
using System.IO;
using System.Threading;
using System.Windows.Forms;

namespace serkova1CSharp
{
    public partial class Form1 : Form
    {
        Process childProcess = null;
        System.Threading.EventWaitHandle startEvent;
        System.Threading.EventWaitHandle stopEvent;
        System.Threading.EventWaitHandle confirmEvent;
        System.Threading.EventWaitHandle exitEvent;

        public Form1()
        {
            InitializeComponent();

            startEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "StartEvent");
            stopEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "StopEvent");
            confirmEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "ConfirmEvent");
            exitEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "ExitEvent");

            
        }

        private void OnProcessExited(object sender, EventArgs e)
        {
            
            if (this.InvokeRequired)
            {
                this.Invoke(new Action(() => {
                    listBox.Items.Clear();
                    childProcess = null;
                }));
            }
            else
            {
                listBox.Items.Clear();
                childProcess = null;
            }
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            if (childProcess == null || childProcess.HasExited)
            {
              
                string exePath = Path.Combine(Application.StartupPath, "..\\..\\..\\x64\\Debug\\serkova1CPP.exe");

                childProcess = Process.Start(exePath);
                childProcess.EnableRaisingEvents = true;
                childProcess.Exited += OnProcessExited;

                // Ждем первое подтверждение
                confirmEvent.WaitOne();

                listBox.Items.Add("Все потоки");
                listBox.Items.Add("Главный поток");
                listBox.SelectedIndex = 0;
            }
            else
            {
                int N = (int)numericUpDown.Value;

                for (int i = 0; i < N; i++)
                {
                    startEvent.Set();           
                    confirmEvent.WaitOne();     
                    listBox.Items.Add($"Thread {listBox.Items.Count - 2}"); 
                }

                listBox.SelectedIndex = listBox.Items.Count - 1;
            }
        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            if (childProcess == null || childProcess.HasExited)
                return;

            if (listBox.Items.Count > 2)
            {
                stopEvent.Set();                 
                confirmEvent.WaitOne();          
                listBox.Items.RemoveAt(listBox.Items.Count - 1); 
                listBox.SelectedIndex = listBox.Items.Count - 1;
            }

        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (childProcess != null && !childProcess.HasExited)
            {
                exitEvent.Set();                  
                confirmEvent.WaitOne(1000);       
                childProcess = null;
            }

            
            startEvent?.Dispose();
            stopEvent?.Dispose();
            confirmEvent?.Dispose();
            exitEvent?.Dispose();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

    }
}