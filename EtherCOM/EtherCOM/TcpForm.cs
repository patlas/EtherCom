using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EtherCOM
{
    public partial class TcpForm : Form
    {
        private TcpClient client;
        private string IP;
        private string Port;
        private NetworkStream clientStream;
        private ASCIIEncoding encoder;
        private byte[] buffer_write;
        private Thread oThread;

        public TcpForm()
        {
            InitializeComponent();
        }

        public void Init(string IP_arg, string Port_arg)
        {
            client = new TcpClient();
            IP = IP_arg;
            Port = Port_arg;
            IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse(IP), Convert.ToInt32(Port));
            client.Connect(serverEndPoint);
            clientStream = client.GetStream();
            encoder = new ASCIIEncoding();

            oThread = new Thread(new ThreadStart(TcpReading));
            oThread.IsBackground = true;
            oThread.Start();
        }

        private void TcpReading()
        {
            byte[] buffer_read = new byte[8];
            char[] chars_read = new char[8];
            
            while (true)
            {
                string Text = Environment.NewLine + "Reading:";
                clientStream.Read(buffer_read, 0, buffer_read.Length);
                encoder.GetDecoder().GetChars(buffer_read, 0, buffer_read.Length, chars_read, 0);
                foreach (char c in chars_read)
                    Text += c;
                TcpReceived.Invoke(new Action(delegate() { TcpReceived.AppendText(Text); }));
            }
        }   

        private void TcpSend_OnKeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter && TcpSend.Text.Length > 0)
            {
                TcpReceived.Text += Environment.NewLine + "Sending:" + TcpSend.Text;
                char[] chars_write;
                int NumOfPacks = TcpSend.Text.Length / 8;
                int LastPack = TcpSend.Text.Length % 8;
                if (NumOfPacks > 0)
                {
                    chars_write = new char[8];
                    for (int i = 0; i < NumOfPacks; i++)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            chars_write[j] = TcpSend.Text[j + i * 8];
                        }
                        SendOverIP(chars_write);
                    }
                    chars_write = new char[8];
                }
                else
                {
                    chars_write = new char[LastPack];
                    for (int j = 0; j < LastPack; j++)
                    {
                        chars_write[j] = TcpSend.Text[j];
                    }
                    SendOverIP(chars_write);
                }
            }
        }

        private void SendOverIP(char[] chars_write)
        {
            client = new TcpClient();
            IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse(IP), Convert.ToInt32(Port));
            client.Connect(serverEndPoint);
            buffer_write = encoder.GetBytes(chars_write);
            clientStream = client.GetStream();
            clientStream.Write(buffer_write, 0, buffer_write.Length);
        }
    }
}


