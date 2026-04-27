using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.InteropServices;
using System.Threading;
using System.Windows.Forms;

namespace serkova1CSharp
{
    public partial class Form1 : Form
    {
        [DllImport("Lab2 DLL.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        private static extern void MapSendMessage(int addr, string str);

        Process? childProcess = null;
        EventWaitHandle startEvent;
        EventWaitHandle dataEvent;
        EventWaitHandle stopEvent;
        EventWaitHandle confirmEvent;
        EventWaitHandle exitEvent;

        public Form1()
        {
            InitializeComponent();

            startEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "StartEvent");
            dataEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "DataEvent");
            stopEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "StopEvent");
            confirmEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "ConfirmEvent");
            exitEvent = new EventWaitHandle(false, EventResetMode.AutoReset, "ExitEvent");
        }

        private void OnProcessExited(object sender, EventArgs e)
        {
            if (this.InvokeRequired)
            {
                this.Invoke(new Action(() =>
                {
                    listBox.Items.Clear();
                    buttonSend.Enabled = false;
                    textBox.Text = string.Empty;
                    textBox.Enabled = false;
                }));
            }
            else
            {
                listBox.Items.Clear();
                buttonSend.Enabled = false;
                textBox.Text = string.Empty;
                textBox.Enabled = false;
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

                confirmEvent.WaitOne();

                listBox.Items.Add("Все потоки");
                listBox.Items.Add("Главный поток");
                listBox.SelectedIndex = 0;

                buttonSend.Enabled = true;
                textBox.Enabled = true;
            }
            else
            {
                int N = (int)numericUpDown.Value;
                for (int i = 0; i < N; i++)
                {
                    startEvent.Set();
                    confirmEvent.WaitOne();
                    listBox.Items.Add($"Поток {listBox.Items.Count - 2}");
                }
                listBox.SelectedIndex = listBox.Items.Count - 1;
            }
        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            if (childProcess == null || childProcess.HasExited) return;

            if (listBox.Items.Count > 2)
            {
                stopEvent.Set();
                confirmEvent.WaitOne();
                listBox.Items.RemoveAt(listBox.Items.Count - 1);
                listBox.SelectedIndex = listBox.Items.Count - 1;
            }
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            if (childProcess == null || childProcess.HasExited) return;

            string message = textBox.Text;
            if (string.IsNullOrWhiteSpace(message)) return;

            int id = listBox.SelectedIndex - 2;
            MapSendMessage(id, message);

            dataEvent.Set();
            confirmEvent.WaitOne();
            textBox.Text = string.Empty;
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
            dataEvent?.Dispose();
            stopEvent?.Dispose();
            confirmEvent?.Dispose();
            exitEvent?.Dispose();
        }
    }
}