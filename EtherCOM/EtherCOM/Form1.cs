using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace EtherCOM
{
    public partial class EtherCOM : Form
    {
        public Form_Data Data;
        static public SerialPort serial_port;
        public EtherCOM()
        {
            InitializeComponent();
        }

        private void EtherCOM_OnLoad(object sender, EventArgs e)
        {       
            Data = new Form_Data();
            Data.Show();
            Data.Location = new Point(this.Location.X + this.Size.Width, this.Location.Y);
            // Initializace COM Ports
            string[] ports = SerialPort.GetPortNames();
            foreach (string port in ports)
            {
                COM.Items.Add(port);
            }
            //Default parameters
            Module_IP.Text = "192.168.2.102";
            Port.Text = "7";

            COM.Text = "COM8";
            Baudrate.Text = "115200";
            Databits.Text = "8";
            Parity.SelectedIndex = 0;
            Handshake.SelectedIndex = 0;
            Mode.SelectedIndex = 0;
            {
                string portName = COM.Text;
                int baudRate = Convert.ToInt32(Baudrate.Text);
                Parity parity = (System.IO.Ports.Parity)Parity.SelectedValue;
                int dataBits = Convert.ToInt32(Databits.Text);
                StopBits stopBits = StopBits.One;
                serial_port = new SerialPort(portName, baudRate, parity, dataBits, stopBits);
                Data.SerialPort_Init(serial_port);
            }
        }

        private void EtherCOM_Connect(object sender, EventArgs e)
        {
            if (Module_IP.Text.Length > 0 && Port.Text.Length > 0)
            {
                TcpClient client = new TcpClient();

                IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse(Module_IP.Text), Convert.ToInt32(Port.Text));

                client.Connect(serverEndPoint);

                NetworkStream clientStream = client.GetStream();

                ASCIIEncoding encoder = new ASCIIEncoding();
                String rs232_parameters = COM.Text + "," + Baudrate.Text + "," +
                                          Databits.Text + "," + Parity.Text + "," +
                                          Handshake.Text + "," + Mode.Text;
                rs232_parameters = COM.Text;
                byte[] buffer_write = encoder.GetBytes(rs232_parameters);
                for (int i = 0; i < buffer_write.Length; i++)
                {
                    byte[] buffer_write_8byte = new byte[8];
                    buffer_write_8byte[i % 8] = buffer_write[i];
                    if (i % 8 == 0 || i == buffer_write.Length - 1)
                        clientStream.Write(buffer_write_8byte, 0, buffer_write_8byte.Length);
                }          
                clientStream.Flush();
                byte[] buffer_read = new byte[255];
                clientStream.Read(buffer_read, 0, buffer_read.Length);
                Decoder decoder = encoder.GetDecoder();
                char[] text = new char[255];
                decoder.GetChars(buffer_read, 0, buffer_read.Length, text, 0, true);
                for (int i = 0; i < 255; i++)
                    Data.DataReceived.Text += text[i];
            }
        }
    }
}
