using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace EtherCOM
{
    public partial class Form_Data : Form
    {
        private SerialPort serial_port;
        public Form_Data()
        {
            InitializeComponent();
        }

        public void SerialPort_Init(SerialPort sp)
        {
            serial_port = sp;
            serial_port.Open();
        }

        private void DataSend_OnKeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter && DataSend.Text.Length > 0)
            {
                DataReceived.Text += "Sending: " + DataSend.Text + Environment.NewLine;
                char[] chars_write;
                int NumOfPacks = DataSend.Text.Length/8;
                int LastPack = DataSend.Text.Length%8;
                if (NumOfPacks > 0)
                {
                    chars_write = new char[8];
                    for (int i = 0; i < NumOfPacks; i++)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            chars_write[j] = DataSend.Text[j + i * 8];
                        }
                        serial_port.Write(chars_write, 0, chars_write.Length);
                    }
                }
                else
                {
                    chars_write = new char[LastPack];
                    for (int j = 0; j < LastPack; j++)
                    {
                        chars_write[j] = DataSend.Text[j];
                    }
                    serial_port.Write(chars_write, 0, chars_write.Length);
                }
            }
        }
    }
}
