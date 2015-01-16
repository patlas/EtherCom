namespace EtherCOM
{
    partial class TcpForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.TcpReceived = new System.Windows.Forms.TextBox();
            this.TcpSend = new System.Windows.Forms.TextBox();
            this.TcpReceivedLabel = new System.Windows.Forms.Label();
            this.TcpSendLabel = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // TcpReceived
            // 
            this.TcpReceived.Location = new System.Drawing.Point(8, 60);
            this.TcpReceived.Multiline = true;
            this.TcpReceived.Name = "TcpReceived";
            this.TcpReceived.Size = new System.Drawing.Size(270, 190);
            this.TcpReceived.TabIndex = 11;
            // 
            // TcpSend
            // 
            this.TcpSend.Location = new System.Drawing.Point(48, 12);
            this.TcpSend.Name = "TcpSend";
            this.TcpSend.Size = new System.Drawing.Size(100, 20);
            this.TcpSend.TabIndex = 10;
            this.TcpSend.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TcpSend_OnKeyDown);
            // 
            // TcpReceivedLabel
            // 
            this.TcpReceivedLabel.AutoSize = true;
            this.TcpReceivedLabel.Location = new System.Drawing.Point(7, 43);
            this.TcpReceivedLabel.Name = "TcpReceivedLabel";
            this.TcpReceivedLabel.Size = new System.Drawing.Size(107, 13);
            this.TcpReceivedLabel.TabIndex = 9;
            this.TcpReceivedLabel.Text = "Received/Send data";
            // 
            // TcpSendLabel
            // 
            this.TcpSendLabel.AutoSize = true;
            this.TcpSendLabel.Location = new System.Drawing.Point(7, 15);
            this.TcpSendLabel.Name = "TcpSendLabel";
            this.TcpSendLabel.Size = new System.Drawing.Size(32, 13);
            this.TcpSendLabel.TabIndex = 8;
            this.TcpSendLabel.Text = "Send";
            // 
            // TcpForm
            // 
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.TcpReceived);
            this.Controls.Add(this.TcpSend);
            this.Controls.Add(this.TcpReceivedLabel);
            this.Controls.Add(this.TcpSendLabel);
            this.Name = "TcpForm";
            this.Text = "Communication over TCP";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label Send_label;
        private System.Windows.Forms.TextBox Send;
        private System.Windows.Forms.Label All_data;
        private System.Windows.Forms.TextBox Data;
        public System.Windows.Forms.TextBox TcpReceived;
        private System.Windows.Forms.TextBox TcpSend;
        private System.Windows.Forms.Label TcpReceivedLabel;
        private System.Windows.Forms.Label TcpSendLabel;
    }
}