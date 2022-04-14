using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.Net;
using System.Net.Sockets;
using System.Diagnostics;
using System.IO;

namespace XMLAlarm
{
    public partial class XMLAlarm : Form
    {
        public XMLAlarm()
        {
            InitializeComponent();
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            int i = this.comboBoxAlarmNr.SelectedIndex + 1;
            MemoryStream stream = new MemoryStream(512);
            XmlTextWriter xw = new XmlTextWriter(stream, System.Text.Encoding.Unicode);
            xw.Formatting = Formatting.Indented;

            xw.WriteStartDocument(true);

            xw.WriteStartElement("Alarm");
            xw.WriteElementString("ID", i.ToString());
            xw.WriteElementString("Name", textBoxName.Text);
            xw.WriteElementString("Control", comboBoxControl.SelectedItem.ToString());
            xw.WriteElementString("State", checkBoxOnOff.Checked?"1":"0");

            xw.WriteEndElement();

            xw.WriteEndDocument();
            xw.Close();

            byte[] msg = stream.ToArray();
            char[] uniChars = new char[msg.Length];
            Encoding.Unicode.GetChars(msg, 0, msg.Length, uniChars, 0);
            string uniString = new string(uniChars);
            textBoxXMLResult.Text = uniString;

            String receiver = textBoxDestination.Text;
            if (receiver.Length > 0)
            {
                // Get DNS host information.
                IPHostEntry hostInfo = Dns.GetHostEntry(receiver);
                // Get the DNS IP addresses associated with the host.
                IPAddress[] IPaddresses = hostInfo.AddressList;
                if (IPaddresses.Length > 0)
                {
                    String address = IPaddresses[0].ToString();
                    address = String.Concat(address, "\n");
                    Trace.Write(address);
                    IPEndPoint hostEndPoint = new IPEndPoint(IPaddresses[0], 2100);
                    Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
                    socket.SendTo(msg, hostEndPoint);
                }
            }
        }
        protected override void OnCreateControl()
        {
            // TODO:  Add IdipUDPAlarm.OnCreateControl implementation
            base.OnCreateControl();
            for (int i = 1; i <= 32; i++)
            {
                this.comboBoxAlarmNr.Items.Add(i.ToString());
            }
            this.comboBoxAlarmNr.SelectedIndex = 0;
            this.comboBoxControl.Items.Add("Level");
            this.comboBoxControl.Items.Add("Edge");
            this.comboBoxControl.SelectedIndex = 0;
        }
    }
}