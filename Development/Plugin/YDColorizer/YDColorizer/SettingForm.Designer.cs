namespace YDColorizer
{
    partial class SettingForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.rdochs = new System.Windows.Forms.RadioButton();
            this.rdocht = new System.Windows.Forms.RadioButton();
            this.label2 = new System.Windows.Forms.Label();
            this.rdoNotifyIconShow = new System.Windows.Forms.RadioButton();
            this.rdoNotifyIconHide = new System.Windows.Forms.RadioButton();
            this.label3 = new System.Windows.Forms.Label();
            this.rdoRememberSizeYes = new System.Windows.Forms.RadioButton();
            this.rdoRememberSizeNo = new System.Windows.Forms.RadioButton();
            this.btnOK = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel3.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(149, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "语言：（需重新启动ydwe）";
            // 
            // rdochs
            // 
            this.rdochs.AutoSize = true;
            this.rdochs.Location = new System.Drawing.Point(3, 3);
            this.rdochs.Name = "rdochs";
            this.rdochs.Size = new System.Drawing.Size(71, 16);
            this.rdochs.TabIndex = 1;
            this.rdochs.TabStop = true;
            this.rdochs.Text = "简体中文";
            this.rdochs.UseVisualStyleBackColor = true;
            // 
            // rdocht
            // 
            this.rdocht.AutoSize = true;
            this.rdocht.Location = new System.Drawing.Point(80, 3);
            this.rdocht.Name = "rdocht";
            this.rdocht.Size = new System.Drawing.Size(71, 16);
            this.rdocht.TabIndex = 2;
            this.rdocht.TabStop = true;
            this.rdocht.Text = "繁体中文";
            this.rdocht.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(13, 53);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(173, 12);
            this.label2.TabIndex = 3;
            this.label2.Text = "托盘图标：（需重新启动ydwe）";
            // 
            // rdoNotifyIconShow
            // 
            this.rdoNotifyIconShow.AutoSize = true;
            this.rdoNotifyIconShow.Location = new System.Drawing.Point(3, 3);
            this.rdoNotifyIconShow.Name = "rdoNotifyIconShow";
            this.rdoNotifyIconShow.Size = new System.Drawing.Size(47, 16);
            this.rdoNotifyIconShow.TabIndex = 4;
            this.rdoNotifyIconShow.TabStop = true;
            this.rdoNotifyIconShow.Text = "显示";
            this.rdoNotifyIconShow.UseVisualStyleBackColor = true;
            // 
            // rdoNotifyIconHide
            // 
            this.rdoNotifyIconHide.AutoSize = true;
            this.rdoNotifyIconHide.Location = new System.Drawing.Point(80, 3);
            this.rdoNotifyIconHide.Name = "rdoNotifyIconHide";
            this.rdoNotifyIconHide.Size = new System.Drawing.Size(59, 16);
            this.rdoNotifyIconHide.TabIndex = 5;
            this.rdoNotifyIconHide.TabStop = true;
            this.rdoNotifyIconHide.Text = "不显示";
            this.rdoNotifyIconHide.UseVisualStyleBackColor = true;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(13, 93);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(89, 12);
            this.label3.TabIndex = 6;
            this.label3.Text = "记忆窗口大小：";
            // 
            // rdoRememberSizeYes
            // 
            this.rdoRememberSizeYes.AutoSize = true;
            this.rdoRememberSizeYes.Location = new System.Drawing.Point(3, 3);
            this.rdoRememberSizeYes.Name = "rdoRememberSizeYes";
            this.rdoRememberSizeYes.Size = new System.Drawing.Size(35, 16);
            this.rdoRememberSizeYes.TabIndex = 7;
            this.rdoRememberSizeYes.TabStop = true;
            this.rdoRememberSizeYes.Text = "是";
            this.rdoRememberSizeYes.UseVisualStyleBackColor = true;
            // 
            // rdoRememberSizeNo
            // 
            this.rdoRememberSizeNo.AutoSize = true;
            this.rdoRememberSizeNo.Location = new System.Drawing.Point(80, 3);
            this.rdoRememberSizeNo.Name = "rdoRememberSizeNo";
            this.rdoRememberSizeNo.Size = new System.Drawing.Size(35, 16);
            this.rdoRememberSizeNo.TabIndex = 8;
            this.rdoRememberSizeNo.TabStop = true;
            this.rdoRememberSizeNo.Text = "否";
            this.rdoRememberSizeNo.UseVisualStyleBackColor = true;
            // 
            // btnOK
            // 
            this.btnOK.Location = new System.Drawing.Point(12, 136);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(75, 23);
            this.btnOK.TabIndex = 9;
            this.btnOK.Text = "确定";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Location = new System.Drawing.Point(93, 136);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 10;
            this.btnCancel.Text = "取消";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // panel1
            // 
            this.panel1.AutoSize = true;
            this.panel1.Controls.Add(this.rdochs);
            this.panel1.Controls.Add(this.rdocht);
            this.panel1.Location = new System.Drawing.Point(12, 28);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(154, 22);
            this.panel1.TabIndex = 11;
            // 
            // panel2
            // 
            this.panel2.AutoSize = true;
            this.panel2.Controls.Add(this.rdoNotifyIconShow);
            this.panel2.Controls.Add(this.rdoNotifyIconHide);
            this.panel2.Location = new System.Drawing.Point(12, 68);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(154, 22);
            this.panel2.TabIndex = 12;
            // 
            // panel3
            // 
            this.panel3.AutoSize = true;
            this.panel3.Controls.Add(this.rdoRememberSizeYes);
            this.panel3.Controls.Add(this.rdoRememberSizeNo);
            this.panel3.Location = new System.Drawing.Point(12, 108);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(154, 22);
            this.panel3.TabIndex = 13;
            // 
            // SettingForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(221, 170);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "SettingForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "颜色插件设置";
            this.Load += new System.EventHandler(this.SettingForm_Load);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.RadioButton rdochs;
        private System.Windows.Forms.RadioButton rdocht;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.RadioButton rdoNotifyIconShow;
        private System.Windows.Forms.RadioButton rdoNotifyIconHide;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.RadioButton rdoRememberSizeYes;
        private System.Windows.Forms.RadioButton rdoRememberSizeNo;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel3;
    }
}