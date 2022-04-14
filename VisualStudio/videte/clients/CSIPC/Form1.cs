using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using System.Diagnostics;
using System.Drawing.Imaging;
using System.IO;

using Videte.CIPC;

namespace CSIPC
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form1 : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label labelHost;
		private System.Windows.Forms.TextBox textBoxHost;
		private System.Windows.Forms.Button buttonClose;
		private System.Windows.Forms.Button buttonConnect;

		private CIPCOutput cipcOutput = null;
		private CIPCInput cipcInput = null;
		private CIPCDatabase cipcDatabase = null;

		private System.Windows.Forms.Button buttonDisconnect;
        private CheckedListBox checkedListBoxInputs;
        private Label label1;
        private Label label2;
        private ListBox listBoxOutputs;
        private Label label3;
        private ListBox listBoxArchives;
        private PictureBox pictureBoxLive;
        private Label label4;
        private Label label5;
        private TextBox textBoxUser;
        private TextBox textBoxPassword;
        private Label labelResult;
        private Label labelCamera;
        private PictureBox pictureBoxPlay;
        private Label label6;
        private ListBox listBoxSequences;
        private Label label7;
        private ListBox listBoxFields;

        delegate void SetInfoCallback(object sender, System.EventArgs e);

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form1()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
this.labelHost = new System.Windows.Forms.Label();
this.textBoxHost = new System.Windows.Forms.TextBox();
this.buttonClose = new System.Windows.Forms.Button();
this.buttonConnect = new System.Windows.Forms.Button();
this.buttonDisconnect = new System.Windows.Forms.Button();
this.checkedListBoxInputs = new System.Windows.Forms.CheckedListBox();
this.label1 = new System.Windows.Forms.Label();
this.label2 = new System.Windows.Forms.Label();
this.listBoxOutputs = new System.Windows.Forms.ListBox();
this.label3 = new System.Windows.Forms.Label();
this.listBoxArchives = new System.Windows.Forms.ListBox();
this.pictureBoxLive = new System.Windows.Forms.PictureBox();
this.label4 = new System.Windows.Forms.Label();
this.label5 = new System.Windows.Forms.Label();
this.textBoxUser = new System.Windows.Forms.TextBox();
this.textBoxPassword = new System.Windows.Forms.TextBox();
this.labelResult = new System.Windows.Forms.Label();
this.labelCamera = new System.Windows.Forms.Label();
this.pictureBoxPlay = new System.Windows.Forms.PictureBox();
this.label6 = new System.Windows.Forms.Label();
this.listBoxSequences = new System.Windows.Forms.ListBox();
this.label7 = new System.Windows.Forms.Label();
this.listBoxFields = new System.Windows.Forms.ListBox();
((System.ComponentModel.ISupportInitialize)(this.pictureBoxLive)).BeginInit();
((System.ComponentModel.ISupportInitialize)(this.pictureBoxPlay)).BeginInit();
this.SuspendLayout();
// 
// labelHost
// 
this.labelHost.Location = new System.Drawing.Point(15, 9);
this.labelHost.Name = "labelHost";
this.labelHost.Size = new System.Drawing.Size(48, 16);
this.labelHost.TabIndex = 0;
this.labelHost.Text = "Host:";
// 
// textBoxHost
// 
this.textBoxHost.Location = new System.Drawing.Point(82, 5);
this.textBoxHost.Name = "textBoxHost";
this.textBoxHost.Size = new System.Drawing.Size(100, 20);
this.textBoxHost.TabIndex = 1;
// 
// buttonClose
// 
this.buttonClose.Location = new System.Drawing.Point(707, 3);
this.buttonClose.Name = "buttonClose";
this.buttonClose.Size = new System.Drawing.Size(75, 23);
this.buttonClose.TabIndex = 2;
this.buttonClose.Text = "Close";
this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
// 
// buttonConnect
// 
this.buttonConnect.Location = new System.Drawing.Point(545, 3);
this.buttonConnect.Name = "buttonConnect";
this.buttonConnect.Size = new System.Drawing.Size(75, 23);
this.buttonConnect.TabIndex = 3;
this.buttonConnect.Text = "Connect";
this.buttonConnect.Click += new System.EventHandler(this.buttonConnect_Click);
// 
// buttonDisconnect
// 
this.buttonDisconnect.Location = new System.Drawing.Point(626, 3);
this.buttonDisconnect.Name = "buttonDisconnect";
this.buttonDisconnect.Size = new System.Drawing.Size(75, 23);
this.buttonDisconnect.TabIndex = 4;
this.buttonDisconnect.Text = "Disconnect";
this.buttonDisconnect.Click += new System.EventHandler(this.buttonDisconnect_Click);
// 
// checkedListBoxInputs
// 
this.checkedListBoxInputs.FormattingEnabled = true;
this.checkedListBoxInputs.Location = new System.Drawing.Point(12, 385);
this.checkedListBoxInputs.Name = "checkedListBoxInputs";
this.checkedListBoxInputs.Size = new System.Drawing.Size(233, 199);
this.checkedListBoxInputs.TabIndex = 5;
// 
// label1
// 
this.label1.AutoSize = true;
this.label1.Location = new System.Drawing.Point(19, 366);
this.label1.Name = "label1";
this.label1.Size = new System.Drawing.Size(41, 13);
this.label1.TabIndex = 6;
this.label1.Text = "Alarms:";
// 
// label2
// 
this.label2.AutoSize = true;
this.label2.Location = new System.Drawing.Point(11, 64);
this.label2.Name = "label2";
this.label2.Size = new System.Drawing.Size(51, 13);
this.label2.TabIndex = 7;
this.label2.Text = "Cameras:";
// 
// listBoxOutputs
// 
this.listBoxOutputs.FormattingEnabled = true;
this.listBoxOutputs.Location = new System.Drawing.Point(12, 80);
this.listBoxOutputs.Name = "listBoxOutputs";
this.listBoxOutputs.Size = new System.Drawing.Size(233, 277);
this.listBoxOutputs.TabIndex = 8;
this.listBoxOutputs.SelectedIndexChanged += new System.EventHandler(this.OutputSelectionChanged);
// 
// label3
// 
this.label3.AutoSize = true;
this.label3.Location = new System.Drawing.Point(252, 366);
this.label3.Name = "label3";
this.label3.Size = new System.Drawing.Size(51, 13);
this.label3.TabIndex = 9;
this.label3.Text = "Archives:";
// 
// listBoxArchives
// 
this.listBoxArchives.FormattingEnabled = true;
this.listBoxArchives.Location = new System.Drawing.Point(251, 385);
this.listBoxArchives.Name = "listBoxArchives";
this.listBoxArchives.Size = new System.Drawing.Size(233, 199);
this.listBoxArchives.TabIndex = 10;
this.listBoxArchives.SelectedIndexChanged += new System.EventHandler(this.listBoxArchives_SelectedIndexChanged);
// 
// pictureBoxLive
// 
this.pictureBoxLive.Location = new System.Drawing.Point(251, 69);
this.pictureBoxLive.Name = "pictureBoxLive";
this.pictureBoxLive.Size = new System.Drawing.Size(352, 288);
this.pictureBoxLive.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
this.pictureBoxLive.TabIndex = 11;
this.pictureBoxLive.TabStop = false;
// 
// label4
// 
this.label4.AutoSize = true;
this.label4.Location = new System.Drawing.Point(198, 9);
this.label4.Name = "label4";
this.label4.Size = new System.Drawing.Size(32, 13);
this.label4.TabIndex = 12;
this.label4.Text = "User:";
// 
// label5
// 
this.label5.AutoSize = true;
this.label5.Location = new System.Drawing.Point(365, 9);
this.label5.Name = "label5";
this.label5.Size = new System.Drawing.Size(56, 13);
this.label5.TabIndex = 13;
this.label5.Text = "Password:";
// 
// textBoxUser
// 
this.textBoxUser.Location = new System.Drawing.Point(236, 5);
this.textBoxUser.Name = "textBoxUser";
this.textBoxUser.Size = new System.Drawing.Size(100, 20);
this.textBoxUser.TabIndex = 14;
// 
// textBoxPassword
// 
this.textBoxPassword.Location = new System.Drawing.Point(428, 5);
this.textBoxPassword.Name = "textBoxPassword";
this.textBoxPassword.Size = new System.Drawing.Size(100, 20);
this.textBoxPassword.TabIndex = 15;
// 
// labelResult
// 
this.labelResult.Location = new System.Drawing.Point(9, 32);
this.labelResult.Name = "labelResult";
this.labelResult.Size = new System.Drawing.Size(277, 14);
this.labelResult.TabIndex = 16;
// 
// labelCamera
// 
this.labelCamera.Location = new System.Drawing.Point(252, 50);
this.labelCamera.Name = "labelCamera";
this.labelCamera.Size = new System.Drawing.Size(212, 16);
this.labelCamera.TabIndex = 17;
// 
// pictureBoxPlay
// 
this.pictureBoxPlay.Location = new System.Drawing.Point(626, 69);
this.pictureBoxPlay.Name = "pictureBoxPlay";
this.pictureBoxPlay.Size = new System.Drawing.Size(352, 288);
this.pictureBoxPlay.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
this.pictureBoxPlay.TabIndex = 18;
this.pictureBoxPlay.TabStop = false;
// 
// label6
// 
this.label6.AutoSize = true;
this.label6.Location = new System.Drawing.Point(487, 366);
this.label6.Name = "label6";
this.label6.Size = new System.Drawing.Size(64, 13);
this.label6.TabIndex = 19;
this.label6.Text = "Sequences:";
// 
// listBoxSequences
// 
this.listBoxSequences.FormattingEnabled = true;
this.listBoxSequences.Location = new System.Drawing.Point(490, 385);
this.listBoxSequences.Name = "listBoxSequences";
this.listBoxSequences.Size = new System.Drawing.Size(233, 199);
this.listBoxSequences.TabIndex = 20;
// 
// label7
// 
this.label7.AutoSize = true;
this.label7.Location = new System.Drawing.Point(726, 366);
this.label7.Name = "label7";
this.label7.Size = new System.Drawing.Size(34, 13);
this.label7.TabIndex = 21;
this.label7.Text = "Fields";
// 
// listBoxFields
// 
this.listBoxFields.FormattingEnabled = true;
this.listBoxFields.Location = new System.Drawing.Point(729, 385);
this.listBoxFields.Name = "listBoxFields";
this.listBoxFields.Size = new System.Drawing.Size(233, 199);
this.listBoxFields.TabIndex = 22;
// 
// Form1
// 
this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
this.ClientSize = new System.Drawing.Size(999, 604);
this.Controls.Add(this.listBoxFields);
this.Controls.Add(this.label7);
this.Controls.Add(this.listBoxSequences);
this.Controls.Add(this.label6);
this.Controls.Add(this.pictureBoxPlay);
this.Controls.Add(this.labelCamera);
this.Controls.Add(this.labelResult);
this.Controls.Add(this.textBoxPassword);
this.Controls.Add(this.textBoxUser);
this.Controls.Add(this.label5);
this.Controls.Add(this.label4);
this.Controls.Add(this.pictureBoxLive);
this.Controls.Add(this.listBoxArchives);
this.Controls.Add(this.label3);
this.Controls.Add(this.listBoxOutputs);
this.Controls.Add(this.label2);
this.Controls.Add(this.label1);
this.Controls.Add(this.checkedListBoxInputs);
this.Controls.Add(this.buttonDisconnect);
this.Controls.Add(this.buttonConnect);
this.Controls.Add(this.buttonClose);
this.Controls.Add(this.textBoxHost);
this.Controls.Add(this.labelHost);
this.Name = "Form1";
this.Text = "idip C# Client";
((System.ComponentModel.ISupportInitialize)(this.pictureBoxLive)).EndInit();
((System.ComponentModel.ISupportInitialize)(this.pictureBoxPlay)).EndInit();
this.ResumeLayout(false);
this.PerformLayout();

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form1());
		}

		private void buttonClose_Click(object sender, System.EventArgs e)
		{
			if (cipcDatabase!=null)
			{
                cipcDatabase.Disconnect();
                cipcDatabase = null;
			}
			if (cipcInput!=null)
			{
                cipcInput.Disconnect();
                cipcInput = null;
			}
			if (cipcOutput!=null)
			{
                cipcOutput.Disconnect();
                cipcOutput = null;
			}
			this.Close();
		}

		private void buttonConnect_Click(object sender, System.EventArgs e)
		{
            cipcInput = new CIPCInput();
            cipcInput.Info += new System.EventHandler(this.InputInfo);
            cipcInput.Alarm += new System.EventHandler(this.InputAlarm);
            cipcInput.Connection += new System.EventHandler(this.CipcConnected);
            cipcInput.Connect(textBoxHost.Text, textBoxUser.Text, "", textBoxPassword.Text, 0);

            cipcOutput = new CIPCOutput();
            cipcOutput.Info += new System.EventHandler(this.OutputInfo);
            cipcOutput.Picture += new System.EventHandler(this.OutputPicture);

            uint outputOptions = CIPCBase.SCO_MULTI_CAMERA | CIPCBase.SCO_NO_STREAM;
            cipcOutput.Connect(textBoxHost.Text, textBoxUser.Text, "", textBoxPassword.Text, outputOptions);

            cipcDatabase = new CIPCDatabase();
            cipcDatabase.Info += new System.EventHandler(this.ArchiveInfo);
            cipcDatabase.FieldInfo += new System.EventHandler(this.FieldInfo);
            cipcDatabase.Connect(textBoxHost.Text, textBoxUser.Text, "", textBoxPassword.Text, 0);
        }


        private void CipcConnected(object sender, System.EventArgs e)
        {
            if (labelResult.InvokeRequired)
            {
                // mich selbst im richtigen Thread aufrufen.
                this.Invoke(new SetInfoCallback(CipcConnected), new object[] { sender, e });
            }
            else
            {
                if (e.GetType() == typeof(Videte.CIPC.CIPCEventArgs))
                {
                    CIPCEventArgs args = (CIPCEventArgs)e;
                    labelResult.Text = args.fetchresult.ErrorString;
                }
            }
        }

        private void InputInfo(object sender, System.EventArgs e)
        {
            if (checkedListBoxInputs.InvokeRequired)
            {
                // mich selbst im richtigen Thread aufrufen.
                this.Invoke(new SetInfoCallback(InputInfo), new object[] { sender,e });
            }
            else
            {
                for (int i = 0; i < this.cipcInput.Inputs.Length; i++)
                {
                    Input inp = this.cipcInput.Inputs[i];
                    if (inp.IsEnabled())
                    {
                        int index = checkedListBoxInputs.Items.Add(inp);
                    }
                }
            }
        }

        private void InputAlarm(object sender, System.EventArgs e)
        {
            Trace.WriteLine("input alarm");
        }

        private void OutputInfo(object sender, System.EventArgs e)
        {
            if (listBoxOutputs.InvokeRequired)
            {
                // mich selbst im richtigen Thread aufrufen.
                this.Invoke(new SetInfoCallback(OutputInfo), new object[] { sender, e });
            }
            else
            {
                for (int i = 0; i < this.cipcOutput.Outputs.Length; i++)
                {
                    Output outp = this.cipcOutput.Outputs[i];
                    if (outp.IsEnabled())
                    {
                        int index = listBoxOutputs.Items.Add(outp);
                    }
                }
            }
        }

        private void OutputSelectionChanged(object sender, System.EventArgs e)
        {
            if (listBoxOutputs.SelectedItem.GetType() == typeof(Videte.CIPC.Output))
            {
                Videte.CIPC.Output output = (Videte.CIPC.Output)listBoxOutputs.SelectedItem;
                if (   cipcOutput != null
                    && cipcOutput.IsConnected()
                    && output.IsCamera())
                {
                    cipcOutput.DoRequestJPEG(output.GetID(), Videte.CIPC.Resolution.RESOLUTION_2CIF, 2, 0);
                }
            }
        }

        private void OutputPicture(object sender, System.EventArgs e)
        {
            if (this.pictureBoxLive.InvokeRequired)
            {
                this.Invoke(new SetInfoCallback(OutputPicture), new object[] { sender, e });
            }
            else
            {
                if (e.GetType() == typeof(Videte.CIPC.CIPCOutputEventArgs))
                {
                    CIPCOutputEventArgs args = (CIPCOutputEventArgs)e;
                    CIPCPictureRecord pict = args.pict;

                    if (pict.GetCompressionType() == CompressionType.COMPRESSION_JPEG)
                    {
                        try
                        {
                            MemoryStream stream = new MemoryStream(pict.GetData(0).Length);
                            stream.Write(pict.GetData(0), 0, pict.GetData(0).Length);
                            Bitmap image = new Bitmap(stream);
                            this.pictureBoxLive.Image = image;
                            this.labelCamera.Text = String.Format("{0:s},{1,3:d}",pict.GetTimestamp().ToLongTimeString(),pict.GetTimestamp().Millisecond.ToString());
                        }
                        catch (Exception ex)
                        {
                            labelResult.Text = ex.Message;
                        }

                        SecID idCam = pict.GetCamID();

                        if (   this.listBoxOutputs.SelectedItem != null
                            && this.listBoxOutputs.SelectedItem.GetType() == typeof(Videte.CIPC.Output))
                        {
                            Output outp = (Output)this.listBoxOutputs.SelectedItem;
                            idCam = outp.GetID();
                        }

                        this.cipcOutput.DoRequestJPEG(idCam, Resolution.RESOLUTION_2CIF, 2, 0);

                    }
                }
            }
        }

        private void ArchiveInfo(object sender, System.EventArgs e)
        {
            if (listBoxArchives.InvokeRequired)
            {
                // mich selbst im richtigen Thread aufrufen.
                this.Invoke(new SetInfoCallback(ArchiveInfo), new object[] { sender, e });
            }
            else
            {
                for (int i = 0; i < this.cipcDatabase.Archives.Length; i++)
                {
                    Archive arch = this.cipcDatabase.Archives[i];
                    int index = listBoxArchives.Items.Add(arch);
                }
                this.cipcDatabase.DoRequestFieldInfo();
            }
        }

        private void FieldInfo(object sender, System.EventArgs e)
        {
            if (listBoxArchives.InvokeRequired)
            {
                // mich selbst im richtigen Thread aufrufen.
                this.Invoke(new SetInfoCallback(FieldInfo), new object[] { sender, e });
            }
            else
            {
                for (int i = 0; i < this.cipcDatabase.Fields.Length; i++)
                {
                    Field field = this.cipcDatabase.Fields[i];
                    int index = listBoxFields.Items.Add(field);
                }
            }
        }
        
        private void buttonDisconnect_Click(object sender, System.EventArgs e)
		{
			if (cipcOutput!=null)
			{
                cipcOutput.Disconnect();
                cipcOutput = null;
                listBoxOutputs.Items.Clear();
                pictureBoxLive.Image = null;
			}
			if (cipcInput!=null)
			{
                cipcInput.Disconnect();
                cipcInput = null;
                checkedListBoxInputs.Items.Clear();
            }
            if (cipcDatabase != null)
			{
                cipcDatabase.Disconnect();
                cipcDatabase = null;
                listBoxArchives.Items.Clear();
			}
            labelResult.Text = "";
		}

        private void listBoxArchives_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBoxArchives.SelectedItem.GetType() == typeof(Videte.CIPC.Archive))
            {
                Videte.CIPC.Archive arch = (Videte.CIPC.Archive)listBoxArchives.SelectedItem;
                if (cipcDatabase != null
                    && cipcDatabase.IsConnected())
                {
                    Videte.CIPC.Sequence[] seqs = arch.GetSequences();
                    this.listBoxSequences.Items.Clear();
                    for (int i = 0; i < seqs.Length; i++)
                    {
                        this.listBoxSequences.Items.Add(seqs[i]);
                    }
                }
            }
        }
    }

}
