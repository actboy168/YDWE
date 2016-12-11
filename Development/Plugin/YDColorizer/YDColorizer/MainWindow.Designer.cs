namespace YDColorizer
{
    partial class MainWindow
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
            this.components = new System.ComponentModel.Container();
            this.chkHook = new System.Windows.Forms.CheckBox();
            this.chkNotifyIcon = new System.Windows.Forms.CheckBox();
            this.timer = new System.Windows.Forms.Timer(this.components);
            this.notifyIcon = new System.Windows.Forms.NotifyIcon(this.components);
            this.SuspendLayout();
            // 
            // chkHook
            // 
            this.chkHook.AutoSize = true;
            this.chkHook.Checked = true;
            this.chkHook.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkHook.Location = new System.Drawing.Point(13, 13);
            this.chkHook.Name = "chkHook";
            this.chkHook.Size = new System.Drawing.Size(66, 16);
            this.chkHook.TabIndex = 0;
            this.chkHook.Text = "chkHook";
            this.chkHook.UseVisualStyleBackColor = true;
            this.chkHook.CheckedChanged += new System.EventHandler(this.chkHook_CheckedChanged);
            // 
            // chkNotifyIcon
            // 
            this.chkNotifyIcon.AutoSize = true;
            this.chkNotifyIcon.Location = new System.Drawing.Point(13, 36);
            this.chkNotifyIcon.Name = "chkNotifyIcon";
            this.chkNotifyIcon.Size = new System.Drawing.Size(102, 16);
            this.chkNotifyIcon.TabIndex = 1;
            this.chkNotifyIcon.Text = "chkNotifyIcon";
            this.chkNotifyIcon.UseVisualStyleBackColor = true;
            this.chkNotifyIcon.CheckedChanged += new System.EventHandler(this.chkNotifyIcon_CheckedChanged);
            // 
            // timer
            // 
            this.timer.Enabled = true;
            this.timer.Interval = 1;
            this.timer.Tick += new System.EventHandler(this.timer_Tick);
            // 
            // notifyIcon
            // 
            this.notifyIcon.Text = "notifyIcon";
            this.notifyIcon.Visible = true;
            this.notifyIcon.Click += new System.EventHandler(this.notifyIcon_Click);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 61);
            this.Controls.Add(this.chkNotifyIcon);
            this.Controls.Add(this.chkHook);
            this.Name = "MainWindow";
            this.Text = "YDColorizer 2.1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainWindow_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.CheckBox chkHook;
        private System.Windows.Forms.CheckBox chkNotifyIcon;
        private System.Windows.Forms.Timer timer;
        private System.Windows.Forms.NotifyIcon notifyIcon;
    }
}

