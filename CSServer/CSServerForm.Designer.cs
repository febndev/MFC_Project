namespace CSServer
{
    partial class CSServerForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            logBox = new System.Windows.Forms.RichTextBox();
            SuspendLayout();
            // 
            // logBox
            // 
            logBox.Location = new System.Drawing.Point(27, 34);
            logBox.Name = "logBox";
            logBox.ReadOnly = true;
            logBox.Size = new System.Drawing.Size(745, 388);
            logBox.TabIndex = 1;
            logBox.Text = "";
            logBox.WordWrap = false;
            // 
            // CSServerForm
            // 
            AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            ClientSize = new System.Drawing.Size(800, 450);
            Controls.Add(logBox);
            Name = "CSServerForm";
            Text = "CanSCan CSServer";
            //FormClosing += CSServerForm_FormClosing;
            //Shown += CSServerForm_Shown;
            ResumeLayout(false);
        }

        #endregion
        private System.Windows.Forms.RichTextBox logBox;
    }
}
