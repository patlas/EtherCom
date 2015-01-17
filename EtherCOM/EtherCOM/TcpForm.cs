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
        private Thread oThread;

        public TcpForm()
        {
            InitializeComponent();
        }

        public void Init(string IP_arg, string Port_arg, byte baudRate_arg, byte dataBits_arg, byte parity_arg, byte stopBits_arg)
        {
            client = new TcpClient();
            IP = IP_arg;
            Port = Port_arg;
            IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse(IP), Convert.ToInt32(Port));
            client.Connect(serverEndPoint);
            clientStream = client.GetStream();
            byte[] RsParams = new byte[4];
            RsParams[0] = baudRate_arg;
            RsParams[1] = dataBits_arg;
            RsParams[2] = parity_arg;
            RsParams[3] = stopBits_arg;
            SendOverIP(RsParams);

            oThread = new Thread(new ThreadStart(TcpReading));
            oThread.IsBackground = true;
            oThread.Start();
        }

        private void TcpReading()
        {
            byte[] buffer_read;        
            while (true)
            {
                try
                {
                    string Text = Environment.NewLine + "Reading:";
                    buffer_read = new byte[10];
                    clientStream.ReadTimeout = 100;
                    clientStream.Read(buffer_read, 0, buffer_read.Length);
                    foreach (byte c in buffer_read)
                        Text += (char)c;
                    TcpReceived.Invoke(new Action(delegate() { TcpReceived.AppendText(Text); }));
                }
                catch (System.IO.IOException)
                {
                    client = new TcpClient();
                    IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse(IP), Convert.ToInt32(Port));
                    client.Connect(serverEndPoint);
                    clientStream = client.GetStream();
                }
            }
        }   

        private void TcpSend_OnKeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter && TcpSend.Text.Length > 0)
            {
                TcpReceived.Text += Environment.NewLine + "Sending:" + TcpSend.Text;
                byte[] bytes_write;
                int NumOfPacks = TcpSend.Text.Length / 8;
                int LastPack = TcpSend.Text.Length % 8;
                if (NumOfPacks > 0)
                {
                    bytes_write = new byte[8];
                    for (int i = 0; i < NumOfPacks; i++)
                    {
                        for (int j = 0; j < 8; j++)
                            bytes_write[j] = (byte)TcpSend.Text[j + i * 8];
                        SendOverIP(bytes_write);
                    }
                }
                if (LastPack > 0)
                {
                    bytes_write = new byte[LastPack];
                    for (int j = 0; j < LastPack; j++)
                        bytes_write[j] = (byte)TcpSend.Text[TcpSend.Text.Length - LastPack + j];
                    SendOverIP(bytes_write);
                }
            }
        }

        private void SendOverIP(byte[] bytes_write)
        {
            client = new TcpClient();
            IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse(IP), Convert.ToInt32(Port));
            client.Connect(serverEndPoint);
            clientStream = client.GetStream();
            clientStream.Write(bytes_write, 0, bytes_write.Length);
        }

        private void Clean_OnClick(object sender, EventArgs e)
        {
            TcpReceived.Text = String.Empty;
        }
    }
}


