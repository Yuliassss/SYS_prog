using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace serkova3CSharp
{
    enum MessageType { INIT = 0, EXIT = 1, START = 2, SEND = 3, STOP = 4, CONFIRM = 5 }

    public partial class Form1 : Form
    {
        // ---- Транспорт через DLL (TCP/boost::asio внутри) ----
        [DllImport("serkova3DLL.dll",
                   CallingConvention = CallingConvention.Cdecl,
                   CharSet = CharSet.Unicode)]
        private static extern int SendM_C(int type, int num, int addr, string str);

        public Form1()
        {
            InitializeComponent();
        }

        // Полностью пересобирает список по числу активных потоков на сервере
        private void RefreshList(int n)
        {
            listBox.Items.Clear();
            listBox.Items.Add("Все потоки");
            listBox.Items.Add("Главный поток");
            for (int i = 0; i < n; i++)
                listBox.Items.Add($"Поток {i}");
            if (listBox.Items.Count > 0)
                listBox.SelectedIndex = 0;
        }

        private void buttonStart_Click(object sender, EventArgs e)
        {
            if (!isConnected)
            {
                // Первый клик — подключаемся к серверу. Сервер вернёт текущее число потоков.
                int n = SendM_C((int)MessageType.INIT, 0, 0, "");
                if (n < 0)
                {
                    MessageBox.Show("Не удалось подключиться к серверу.\nЗапустите serkova3Server.exe.",
                        "Ошибка", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
                isConnected = true;
                buttonSend.Enabled = true;
                textBox.Enabled = true;
                buttonStop.Enabled = true;
                RefreshList(n);
                return;
            }

            // Последующие клики — создаём N потоков
            int newCount = SendM_C((int)MessageType.START, (int)numericUpDown.Value, 0, "");
            if (newCount < 0) { OnDisconnected(); return; }
            RefreshList(newCount);
            if (listBox.Items.Count > 2) listBox.SelectedIndex = listBox.Items.Count - 1;
        }

        private void buttonStop_Click(object sender, EventArgs e)
        {
            if (!isConnected) return;

            int newCount = SendM_C((int)MessageType.STOP, 0, 0, "");
            if (newCount < 0) { OnDisconnected(); return; }
            RefreshList(newCount);
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            if (!isConnected) return;
            string text = textBox.Text;
            if (string.IsNullOrEmpty(text)) return;

            // 0 = "Все потоки" → addr=-2; 1 = "Главный поток" → addr=-1; 2+ = поток (index - 2)
            int idx = listBox.SelectedIndex;
            int addr;
            if (idx == 0) addr = -2;
            else if (idx == 1) addr = -1;
            else addr = idx - 2;

            int newCount = SendM_C((int)MessageType.SEND, 0, addr, text);
            if (newCount < 0) { OnDisconnected(); return; }

            RefreshList(newCount);
            textBox.Clear();
        }

        private void OnDisconnected()
        {
            isConnected = false;
            buttonSend.Enabled = false;
            textBox.Enabled = false;
            buttonStop.Enabled = false;
            listBox.Items.Clear();
            MessageBox.Show("Соединение с сервером потеряно.", "Внимание");
        }

        private bool isConnected = false;

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {
            if (isConnected)
            {
                try { SendM_C((int)MessageType.EXIT, 0, 0, ""); } catch { }
            }
        }
    }
}
