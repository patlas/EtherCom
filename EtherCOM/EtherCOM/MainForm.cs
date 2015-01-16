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
using System.Threading;

namespace EtherCOM
{
    public partial class EtherCOM : Form
    {
        public EtherCOM()
        {
            InitializeComponent();
        }

        private void EtherCOM_OnLoad(object sender, EventArgs e)
        {               
            // Initializace COM Ports
            string[] ports = SerialPort.GetPortNames();
            foreach (string port in ports)
                Com.Items.Add(port);
            //Default parameters
            ModuleIP.Text = "192.168.2.102";
            Port.Text = "7";

            Com.Text = "COM8";
            Baudrate.Text = "115200";
            Databits.Text = "8";
            Parity.SelectedIndex = 0;
        }

        private void EtherCOM_Connect(object sender, EventArgs e)
        {
            if (ModuleIP.Text.Length > 0 && Port.Text.Length > 0)
            {
                string portName = Com.Text;
                int baudRate = Convert.ToInt32(Baudrate.Text);
                Parity parity = System.IO.Ports.Parity.None;
                int dataBits = Convert.ToInt32(Databits.Text);
                StopBits stopBits = StopBits.One;
                SerialPort serial_port = new SerialPort(portName, baudRate, parity, dataBits, stopBits);

                RsForm RsFormInstance = new RsForm();
                RsFormInstance.Show();
                RsFormInstance.Location = new Point(this.Location.X + this.Width, this.Location.Y);
                RsFormInstance.Init(serial_port);

                TcpForm TcpFormInstance = new TcpForm();
                TcpFormInstance.Show();
                TcpFormInstance.Location = new Point(this.Location.X + this.Width + RsFormInstance.Width, this.Location.Y);
                TcpFormInstance.Init(ModuleIP.Text, Port.Text);
            }
            else
            {
                if( ModuleIP.Text.Length > 0 )
                    ModuleIP.Text = "Enter IP";
                if( Port.Text.Length > 0 )
                    Port.Text = "Enter PORT";
            }
        }
    }
}
