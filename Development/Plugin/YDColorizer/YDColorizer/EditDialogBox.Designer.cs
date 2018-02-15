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
            this.txtEdit = new YDColorizer.MyTextBox();
            this.txtPreview = new YDColorizer.MyRichTextBox();
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
            this.splitContainer.Panel1.SuspendLayout();
            this.splitContainer.Panel2.SuspendLayout();
            this.splitContainer.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblStatic
            // 
            this.lblStatic.AutoSize = true;
            this.lblStatic.Location = new System.Drawing.Point(17, 16);
            this.lblStatic.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.lblStatic.Name = "lblStatic";
            this.lblStatic.Size = new System.Drawing.Size(55, 15);
            this.lblStatic.TabIndex = 0;
            this.lblStatic.Text = "Static";
            // 
            // splitContainer
            // 
            this.splitContainer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer.Location = new System.Drawing.Point(16, 35);
            this.splitContainer.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
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
            this.splitContainer.Size = new System.Drawing.Size(547, 240);
            this.splitContainer.SplitterDistance = 121;
            this.splitContainer.SplitterWidth = 5;
            this.splitContainer.TabIndex = 1;
            this.splitContainer.TabStop = false;
            // 
            // txtEdit
            // 
            this.txtEdit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.txtEdit.HideSelection = false;
            this.txtEdit.Location = new System.Drawing.Point(0, 0);
            this.txtEdit.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.txtEdit.Multiline = true;
            this.txtEdit.Name = "txtEdit";
            this.txtEdit.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtEdit.Size = new System.Drawing.Size(547, 121);
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
            this.txtPreview.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.txtPreview.Name = "txtPreview";
            this.txtPreview.ScrollBars = System.Windows.Forms.RichTextBoxScrollBars.ForcedVertical;
            this.txtPreview.Size = new System.Drawing.Size(547, 114);
            this.txtPreview.TabIndex = 1;
            this.txtPreview.Text = "";
            this.txtPreview.TextChanged += new System.EventHandler(this.txtPreview_TextChanged);
            this.txtPreview.Enter += new System.EventHandler(this.txtPreview_Enter);
            this.txtPreview.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txt_KeyDown);
            this.txtPreview.MouseDown += new System.Windows.Forms.MouseEventHandler(this.txtPreview_MouseDown);
            // 
            // btnOk
            // 
            this.btnOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOk.Location = new System.Drawing.Point(355, 282);
            this.btnOk.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnOk.Name = "btnOk";
            this.btnOk.Size = new System.Drawing.Size(100, 29);
            this.btnOk.TabIndex = 2;
            this.btnOk.Text = "btnOk";
            this.btnOk.UseVisualStyleBackColor = true;
            this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.Location = new System.Drawing.Point(463, 282);
            this.btnCancel.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(100, 29);
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
            this.btn1stColor.Location = new System.Drawing.Point(16, 280);
            this.btn1stColor.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btn1stColor.Name = "btn1stColor";
            this.btn1stColor.Size = new System.Drawing.Size(33, 31);
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
            this.btn2ndColor.Location = new System.Drawing.Point(57, 292);
            this.btn2ndColor.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btn2ndColor.Name = "btn2ndColor";
            this.btn2ndColor.Size = new System.Drawing.Size(20, 19);
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
            this.btn3rdColor.Location = new System.Drawing.Point(85, 292);
            this.btn3rdColor.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btn3rdColor.Name = "btn3rdColor";
            this.btn3rdColor.Size = new System.Drawing.Size(20, 19);
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
            this.btn4thColor.Location = new System.Drawing.Point(113, 292);
            this.btn4thColor.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btn4thColor.Name = "btn4thColor";
            this.btn4thColor.Size = new System.Drawing.Size(20, 19);
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
            this.btnRedo.Location = new System.Drawing.Point(532, 2);
            this.btnRedo.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnRedo.Name = "btnRedo";
            this.btnRedo.Size = new System.Drawing.Size(31, 29);
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
            this.btnUndo.Location = new System.Drawing.Point(493, 2);
            this.btnUndo.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnUndo.Name = "btnUndo";
            this.btnUndo.Size = new System.Drawing.Size(31, 29);
            this.btnUndo.TabIndex = 4;
            this.btnUndo.UseVisualStyleBackColor = false;
            this.btnUndo.Click += new System.EventHandler(this.btnUndo_Click);
            // 
            // btnSetColor
            // 
            this.btnSetColor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnSetColor.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnSetColor.Image = global::YDColorizer.Properties.Resources.icoSetColor;
            this.btnSetColor.Location = new System.Drawing.Point(141, 280);
            this.btnSetColor.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnSetColor.Name = "btnSetColor";
            this.btnSetColor.Size = new System.Drawing.Size(40, 31);
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
            this.btnGradual.Location = new System.Drawing.Point(189, 280);
            this.btnGradual.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.btnGradual.Name = "btnGradual";
            this.btnGradual.Size = new System.Drawing.Size(40, 31);
            this.btnGradual.TabIndex = 0;
            this.btnGradual.TabStop = false;
            this.btnGradual.UseVisualStyleBackColor = true;
            this.btnGradual.Click += new System.EventHandler(this.btnGradual_Click);
            // 
            // EditDialogBox
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(579, 326);
            this.ControlBox = false;
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
            this.Margin = new System.Windows.Forms.Padding(4, 4, 4, 4);
            this.MinimumSize = new System.Drawing.Size(487, 238);
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
    }
}