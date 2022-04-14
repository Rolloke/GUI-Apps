namespace XMLAlarm
{
    partial class XMLAlarm
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
            this.buttonClose = new System.Windows.Forms.Button();
            this.buttonSend = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.textBoxDestination = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.comboBoxAlarmNr = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBoxName = new System.Windows.Forms.TextBox();
            this.textBoxXMLResult = new System.Windows.Forms.TextBox();
            this.comboBoxControl = new System.Windows.Forms.ComboBox();
            this.checkBoxOnOff = new System.Windows.Forms.CheckBox();
            this.SuspendLayout();
            // 
            // buttonClose
            // 
            this.buttonClose.Location = new System.Drawing.Point(191, 273);
            this.buttonClose.Name = "buttonClose";
            this.buttonClose.Size = new System.Drawing.Size(75, 23);
            this.buttonClose.TabIndex = 0;
            this.buttonClose.Text = "Close";
            this.buttonClose.UseVisualStyleBackColor = true;
            this.buttonClose.Click += new System.EventHandler(this.buttonClose_Click);
            // 
            // buttonSend
            // 
            this.buttonSend.Location = new System.Drawing.Point(19, 273);
            this.buttonSend.Name = "buttonSend";
            this.buttonSend.Size = new System.Drawing.Size(75, 23);
            this.buttonSend.TabIndex = 1;
            this.buttonSend.Text = "Send";
            this.buttonSend.UseVisualStyleBackColor = true;
            this.buttonSend.Click += new System.EventHandler(this.buttonSend_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(63, 13);
            this.label1.TabIndex = 2;
            this.label1.Text = "Destination:";
            // 
            // textBoxDestination
            // 
            this.textBoxDestination.Location = new System.Drawing.Point(82, 10);
            this.textBoxDestination.Name = "textBoxDestination";
            this.textBoxDestination.Size = new System.Drawing.Size(184, 20);
            this.textBoxDestination.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(16, 43);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Alarm No:";
            // 
            // comboBoxAlarmNr
            // 
            this.comboBoxAlarmNr.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxAlarmNr.FormattingEnabled = true;
            this.comboBoxAlarmNr.Location = new System.Drawing.Point(82, 40);
            this.comboBoxAlarmNr.Name = "comboBoxAlarmNr";
            this.comboBoxAlarmNr.Size = new System.Drawing.Size(184, 21);
            this.comboBoxAlarmNr.TabIndex = 5;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(19, 78);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(38, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Name:";
            // 
            // textBoxName
            // 
            this.textBoxName.Location = new System.Drawing.Point(82, 78);
            this.textBoxName.Name = "textBoxName";
            this.textBoxName.Size = new System.Drawing.Size(184, 20);
            this.textBoxName.TabIndex = 7;
            // 
            // textBoxXMLResult
            // 
            this.textBoxXMLResult.Location = new System.Drawing.Point(19, 139);
            this.textBoxXMLResult.Multiline = true;
            this.textBoxXMLResult.Name = "textBoxXMLResult";
            this.textBoxXMLResult.Size = new System.Drawing.Size(247, 128);
            this.textBoxXMLResult.TabIndex = 8;
            // 
            // comboBoxControl
            // 
            this.comboBoxControl.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxControl.FormattingEnabled = true;
            this.comboBoxControl.Location = new System.Drawing.Point(22, 112);
            this.comboBoxControl.Name = "comboBoxControl";
            this.comboBoxControl.Size = new System.Drawing.Size(121, 21);
            this.comboBoxControl.TabIndex = 9;
            // 
            // checkBoxOnOff
            // 
            this.checkBoxOnOff.AutoSize = true;
            this.checkBoxOnOff.Location = new System.Drawing.Point(191, 116);
            this.checkBoxOnOff.Name = "checkBoxOnOff";
            this.checkBoxOnOff.Size = new System.Drawing.Size(55, 17);
            this.checkBoxOnOff.TabIndex = 10;
            this.checkBoxOnOff.Text = "on/off";
            this.checkBoxOnOff.UseVisualStyleBackColor = true;
            // 
            // XMLAlarm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(283, 308);
            this.Controls.Add(this.checkBoxOnOff);
            this.Controls.Add(this.comboBoxControl);
            this.Controls.Add(this.textBoxXMLResult);
            this.Controls.Add(this.textBoxName);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.comboBoxAlarmNr);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBoxDestination);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.buttonSend);
            this.Controls.Add(this.buttonClose);
            this.Name = "XMLAlarm";
            this.Text = "XML Alarm";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button buttonClose;
        private System.Windows.Forms.Button buttonSend;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBoxDestination;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox comboBoxAlarmNr;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBoxName;
        private System.Windows.Forms.TextBox textBoxXMLResult;
        private System.Windows.Forms.ComboBox comboBoxControl;
        private System.Windows.Forms.CheckBox checkBoxOnOff;
    }
}

