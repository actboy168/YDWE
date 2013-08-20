namespace YDColorizer
{
    partial class EditDialogBox
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EditDialogBox));
            this.lblStatic = new System.Windows.Forms.Label();
            this.splitContainer = new System.Windows.Forms.SplitContainer();
            this.btnOk = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btn1stColor = new System.Windows.Forms.Button();
            this.btn2ndColor = new System.Windows.Forms.Button();
            this.btn3rdColor = new System.Windows.Forms.Button();
            this.btn4thColor = new System.Windows.Forms.Button();
            this.btnRedo = new System.Windows.Forms.Button();
            this.btnUndo = new System.Windows.Forms.Button();
            this.btnSetColor = new System.Windows.Forms.Button();
            this.btnGradual = new System.Windows.Forms.Button();
            this.btnSetting = new System.Windows.Forms.Button();
            this.txtEdit = new YDColorizer.MyTextBox();
            this.txtPreview = new YDColorizer.MyRichTextBox();
            this.splitContainer.Panel1.SuspendLayout();
            this.splitContainer.Panel2.SuspendLayout();
            this.splitContainer.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblStatic
            // 
            this.lblStatic.AutoSize = true;
            this.lblStatic.Location = new System.Drawing.Point(13, 13);
            this.lblStatic.Name = "lblStatic";
            this.lblStatic.Size = new System.Drawing.Size(41, 12);
            this.lblStatic.TabIndex = 0;
            this.lblStatic.Text = "Static";
            // 
            // splitContainer
            // 
            this.splitContainer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer.Location = new System.Drawing.Point(12, 28);
            this.splitContainer.Name = "splitContainer";
            this.splitContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer.Panel1
            // 
            this.splitContainer.Panel1.Controls.Add(this.txtEdit);
            // 
            // splitContainer.Panel2
            // 
            this.splitContainer.Panel2.BackColor = System.Drawing.Color.Black;
            this.splitContainer.Panel2.Controls.Add(this.txtPreview);
            this.splitContainer.Size = new System.Drawing.Size(410, 192);
            this.splitContainer.SplitterDistance = 97;
            this.splitContainer.TabIndex = 1;
            this.splitContainer.TabStop = false;
            // 
            // btnOk
            // 
            this.btnOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOk.Location = new System.Drawing.Point(266, 226);
            this.btnOk.Name = "btnOk";
            this.btnOk.Size = new System.Drawing.Size(75, 23);
            this.btnOk.TabIndex = 2;
            this.btnOk.Text = "btnOk";
            this.btnOk.UseVisualStyleBackColor = true;
            this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.Location = new System.Drawing.Point(347, 226);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 3;
            this.btnCancel.Text = "btnCancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btn1stColor
            // 
            this.btn1stColor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btn1stColor.BackColor = System.Drawing.Color.Black;
            this.btn1stColor.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btn1stColor.Location = new System.Drawing.Point(12, 224);
            this.btn1stColor.Name = "btn1stColor";
            this.btn1stColor.Size = new System.Drawing.Size(25, 25);
            this.btn1stColor.TabIndex = 0;
            this.btn1stColor.TabStop = false;
            this.btn1stColor.UseVisualStyleBackColor = false;
            this.btn1stColor.Click += new System.EventHandler(this.btnSelectColor_Click);
            // 
            // btn2ndColor
            // 
            this.btn2ndColor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btn2ndColor.BackColor = System.Drawing.Color.Black;
            this.btn2ndColor.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btn2ndColor.Location = new System.Drawing.Point(43, 234);
            this.btn2ndColor.Name = "btn2ndColor";
            this.btn2ndColor.Size = new System.Drawing.Size(15, 15);
            this.btn2ndColor.TabIndex = 0;
            this.btn2ndColor.TabStop = false;
            this.btn2ndColor.UseVisualStyleBackColor = false;
            this.btn2ndColor.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnSwapColor_MouseDown);
            // 
            // btn3rdColor
            // 
            this.btn3rdColor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btn3rdColor.BackColor = System.Drawing.Color.Black;
            this.btn3rdColor.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btn3rdColor.Location = new System.Drawing.Point(64, 234);
            this.btn3rdColor.Name = "btn3rdColor";
            this.btn3rdColor.Size = new System.Drawing.Size(15, 15);
            this.btn3rdColor.TabIndex = 0;
            this.btn3rdColor.TabStop = false;
            this.btn3rdColor.UseVisualStyleBackColor = false;
            this.btn3rdColor.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnSwapColor_MouseDown);
            // 
            // btn4thColor
            // 
            this.btn4thColor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btn4thColor.BackColor = System.Drawing.Color.Black;
            this.btn4thColor.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.btn4thColor.Location = new System.Drawing.Point(85, 234);
            this.btn4thColor.Name = "btn4thColor";
            this.btn4thColor.Size = new System.Drawing.Size(15, 15);
            this.btn4thColor.TabIndex = 0;
            this.btn4thColor.TabStop = false;
            this.btn4thColor.UseVisualStyleBackColor = false;
            this.btn4thColor.MouseDown += new System.Windows.Forms.MouseEventHandler(this.btnSwapColor_MouseDown);
            // 
            // btnRedo
            // 
            this.btnRedo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnRedo.BackColor = System.Drawing.Color.Transparent;
            this.btnRedo.Enabled = false;
            this.btnRedo.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnRedo.Image = ((System.Drawing.Image)(resources.GetObject("btnRedo.Image")));
            this.btnRedo.Location = new System.Drawing.Point(370, 2);
            this.btnRedo.Name = "btnRedo";
            this.btnRedo.Size = new System.Drawing.Size(23, 23);
            this.btnRedo.TabIndex = 0;
            this.btnRedo.TabStop = false;
            this.btnRedo.UseVisualStyleBackColor = false;
            this.btnRedo.Click += new System.EventHandler(this.btnRedo_Click);
            // 
            // btnUndo
            // 
            this.btnUndo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnUndo.BackColor = System.Drawing.Color.Transparent;
            this.btnUndo.Enabled = false;
            this.btnUndo.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnUndo.Image = ((System.Drawing.Image)(resources.GetObject("btnUndo.Image")));
            this.btnUndo.Location = new System.Drawing.Point(341, 2);
            this.btnUndo.Name = "btnUndo";
            this.btnUndo.Size = new System.Drawing.Size(23, 23);
            this.btnUndo.TabIndex = 4;
            this.btnUndo.UseVisualStyleBackColor = false;
            this.btnUndo.Click += new System.EventHandler(this.btnUndo_Click);
            // 
            // btnSetColor
            // 
            this.btnSetColor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnSetColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSetColor.Image = global::YDColorizer.Properties.Resources.icoSetColor;
            this.btnSetColor.Location = new System.Drawing.Point(106, 224);
            this.btnSetColor.Name = "btnSetColor";
            this.btnSetColor.Size = new System.Drawing.Size(30, 25);
            this.btnSetColor.TabIndex = 0;
            this.btnSetColor.TabStop = false;
            this.btnSetColor.UseVisualStyleBackColor = true;
            this.btnSetColor.Click += new System.EventHandler(this.btnSetColor_Click);
            // 
            // btnGradual
            // 
            this.btnGradual.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnGradual.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnGradual.Image = global::YDColorizer.Properties.Resources.icoGradualColor;
            this.btnGradual.Location = new System.Drawing.Point(142, 224);
            this.btnGradual.Name = "btnGradual";
            this.btnGradual.Size = new System.Drawing.Size(30, 25);
            this.btnGradual.TabIndex = 0;
            this.btnGradual.TabStop = false;
            this.btnGradual.UseVisualStyleBackColor = true;
            this.btnGradual.Click += new System.EventHandler(this.btnGradual_Click);
            // 
            // btnSetting
            // 
            this.btnSetting.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnSetting.BackColor = System.Drawing.Color.Transparent;
            this.btnSetting.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSetting.Image = global::YDColorizer.Properties.Resources.icoSetting;
            this.btnSetting.Location = new System.Drawing.Point(399, 2);
            this.btnSetting.Name = "btnSetting";
            this.btnSetting.Size = new System.Drawing.Size(23, 23);
            this.btnSetting.TabIndex = 5;
            this.btnSetting.UseVisualStyleBackColor = false;
            this.btnSetting.Click += new System.EventHandler(this.btnSetting_Click);
            // 
            // txtEdit
            // 
            this.txtEdit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtEdit.HideSelection = false;
            this.txtEdit.Location = new System.Drawing.Point(0, 0);
            this.txtEdit.Multiline = true;
            this.txtEdit.Name = "txtEdit";
            this.txtEdit.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtEdit.Size = new System.Drawing.Size(410, 97);
            this.txtEdit.TabIndex = 0;
            this.txtEdit.TextChanged += new System.EventHandler(this.txtEdit_TextChanged);
            this.txtEdit.Enter += new System.EventHandler(this.txtEdit_Enter);
            this.txtEdit.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txt_KeyDown);
            // 
            // txtPreview
            // 
            this.txtPreview.BackColor = System.Drawing.Color.Black;
            this.txtPreview.DetectUrls = false;
            this.txtPreview.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtPreview.ForeColor = System.Drawing.Color.White;
            this.txtPreview.HideSelection = false;
            this.txtPreview.Location = new System.Drawing.Point(0, 0);
            this.txtPreview.Name = "txtPreview";
            this.txtPreview.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
            this.txtPreview.Size = new System.Drawing.Size(410, 91);
            this.txtPreview.TabIndex = 1;
            this.txtPreview.Text = "";
            this.txtPreview.TextChanged += new System.EventHandler(this.txtPreview_TextChanged);
            this.txtPreview.Enter += new System.EventHandler(this.txtPreview_Enter);
            this.txtPreview.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txt_KeyDown);
            this.txtPreview.MouseDown += new System.Windows.Forms.MouseEventHandler(this.txtPreview_MouseDown);
            // 
            // EditDialogBox
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(434, 261);
            this.ControlBox = false;
            this.Controls.Add(this.btnSetting);
            this.Controls.Add(this.btnGradual);
            this.Controls.Add(this.btnSetColor);
            this.Controls.Add(this.btnUndo);
            this.Controls.Add(this.btnRedo);
            this.Controls.Add(this.btn4thColor);
            this.Controls.Add(this.btn3rdColor);
            this.Controls.Add(this.btn2ndColor);
            this.Controls.Add(this.btn1stColor);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOk);
            this.Controls.Add(this.splitContainer);
            this.Controls.Add(this.lblStatic);
            this.DoubleBuffered = true;
            this.KeyPreview = true;
            this.MinimumSize = new System.Drawing.Size(370, 200);
            this.Name = "EditDialogBox";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "EditDialogBox";
            this.TopMost = true;
            this.Load += new System.EventHandler(this.EditDialogBox_Load);
            this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.EditDialogBox_KeyDown);
            this.splitContainer.Panel1.ResumeLayout(false);
            this.splitContainer.Panel1.PerformLayout();
            this.splitContainer.Panel2.ResumeLayout(false);
            this.splitContainer.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lblStatic;
        private System.Windows.Forms.SplitContainer splitContainer;
        private System.Windows.Forms.Button btnOk;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btn1stColor;
        private System.Windows.Forms.Button btn2ndColor;
        private System.Windows.Forms.Button btn3rdColor;
        private System.Windows.Forms.Button btn4thColor;
        private MyTextBox txtEdit;
        private System.Windows.Forms.Button btnRedo;
        private System.Windows.Forms.Button btnUndo;
        private MyRichTextBox txtPreview;
        private System.Windows.Forms.Button btnSetColor;
        private System.Windows.Forms.Button btnGradual;
        private System.Windows.Forms.Button btnSetting;
    }
}