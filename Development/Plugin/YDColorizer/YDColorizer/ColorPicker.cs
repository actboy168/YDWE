using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace YDColorizer
{
    public partial class ColorPicker : Form
    {
        /// <summary>
        /// 父控件
        /// </summary>
        private Control parentControl;

        /// <summary>
        /// 选择的颜色
        /// </summary>
        public Color selectedColor;

        /// <summary>
        /// 色块
        /// </summary>
        private Button[,] btnColor = new Button[5, 8];

        private Button btnOtherColor;

        private Button btnScreenColor;

        /// <summary>
        /// 颜色被选中
        /// </summary>
        public event EventHandler SelectedColorChanged;

        struct ColorSet
        {
            public Color color;
            public string name;
        }

        /// <summary>
        /// 简体中文的颜色集
        /// </summary>
        private ColorSet[,] colorSetChs = 
{{new ColorSet{color=Color.FromArgb(  0,  0,  0),name="黑色"  },new ColorSet{color=Color.FromArgb(153, 51,  0),name="褐色"},new ColorSet{color=Color.FromArgb( 51, 51,  0),name="橄榄色"},new ColorSet{color=Color.FromArgb(  0, 51,  0),name="深绿"},new ColorSet{color=Color.FromArgb(  0, 51,102),name="深青"  },new ColorSet{color=Color.FromArgb(  0,  0,128),name="深蓝"},new ColorSet{color=Color.FromArgb( 51, 51,153),name="靛蓝"  },new ColorSet{color=Color.FromArgb( 51, 51, 51),name="灰色-80%"}} 
,{new ColorSet{color=Color.FromArgb(128,  0,  0),name="深红"  },new ColorSet{color=Color.FromArgb(255,102,  0),name="橙色"},new ColorSet{color=Color.FromArgb(128,128,  0),name="深黄"  },new ColorSet{color=Color.FromArgb(  0,128,  0),name="绿色"},new ColorSet{color=Color.FromArgb(  0,128,128),name="青色"  },new ColorSet{color=Color.FromArgb(  0,  0,255),name="蓝色"},new ColorSet{color=Color.FromArgb(102,102,153),name="蓝灰"  },new ColorSet{color=Color.FromArgb(128,128,128),name="灰色-50%"}}
,{new ColorSet{color=Color.FromArgb(255,  0,  0),name="红色"  },new ColorSet{color=Color.FromArgb(255,153,  0),name="浅橙"},new ColorSet{color=Color.FromArgb(153,204,  0),name="酸橙"  },new ColorSet{color=Color.FromArgb( 51,153,102),name="海绿"},new ColorSet{color=Color.FromArgb( 51,204,204),name="水绿"  },new ColorSet{color=Color.FromArgb( 51,102,255),name="浅蓝"},new ColorSet{color=Color.FromArgb(128,  0,128),name="紫罗兰"},new ColorSet{color=Color.FromArgb(153,153,153),name="灰色-40%"}}
,{new ColorSet{color=Color.FromArgb(255,  0,255),name="粉红"  },new ColorSet{color=Color.FromArgb(255,104,  0),name="金色"},new ColorSet{color=Color.FromArgb(255,255,  0),name="黄色"  },new ColorSet{color=Color.FromArgb(  0,255,  0),name="鲜绿"},new ColorSet{color=Color.FromArgb(  0,255,255),name="青绿"  },new ColorSet{color=Color.FromArgb(  0,204,255),name="天蓝"},new ColorSet{color=Color.FromArgb(153, 51,102),name="梅红"  },new ColorSet{color=Color.FromArgb(192,192,192),name="灰色-25%"}}
,{new ColorSet{color=Color.FromArgb(255,153,204),name="玫瑰红"},new ColorSet{color=Color.FromArgb(255,204,153),name="茶色"},new ColorSet{color=Color.FromArgb(255,255,204),name="浅黄"  },new ColorSet{color=Color.FromArgb(204,255,204),name="浅绿"},new ColorSet{color=Color.FromArgb(204,255,255),name="浅青绿"},new ColorSet{color=Color.FromArgb(153,204,255),name="浅蓝"},new ColorSet{color=Color.FromArgb(204,153,255),name="浅紫"  },new ColorSet{color=Color.FromArgb(255,255,255),name="白色"    }}
};

        /// <summary>
        /// 繁体中文的颜色集
        /// </summary>
        private ColorSet[,] colorSetCht =
{{new ColorSet{color=Color.FromArgb( 0, 0, 0),name="黑色" },new ColorSet{color=Color.FromArgb(153, 51, 0),name="褐色"},new ColorSet {color=Color.FromArgb( 51, 51, 0),name="橄欖色"},new ColorSet{color=Color.FromArgb( 0, 51, 0),name="深綠"},new ColorSet{color =Color.FromArgb( 0, 51,102),name="深青" },new ColorSet{color=Color.FromArgb( 0, 0,128),name="深藍"},new ColorSet{color=Color.FromArgb( 51, 51,153 ),name="靛藍" },new ColorSet{color=Color.FromArgb( 51, 51, 51),name="灰色-80%"}}
,{new ColorSet{color=Color.FromArgb(128, 0, 0),name="深紅" },new ColorSet{color=Color.FromArgb(255,102, 0),name="橙色"},new ColorSet{ color=Color.FromArgb(128,128, 0),name="深黃" },new ColorSet{color=Color.FromArgb( 0,128, 0),name="綠色"},new ColorSet{color=Color.FromArgb( 0,128,128 ),name="青色" },new ColorSet{color=Color.FromArgb( 0, 0,255),name="藍色"},new ColorSet{color=Color.FromArgb(102,102,153),name="藍灰" } ,new ColorSet{color=Color.FromArgb(128,128,128),name="灰色-50%"}}
,{new ColorSet{color=Color.FromArgb(255, 0, 0),name="紅色" },new ColorSet{color=Color.FromArgb(255,153, 0),name="淺橙"},new ColorSet{ color=Color.FromArgb(153,204, 0),name="酸橙" },new ColorSet{color=Color.FromArgb( 51,153,102),name="海綠"},new ColorSet{color=Color.FromArgb( 51,204,204) ,name="水綠" },new ColorSet{color=Color.FromArgb( 51,102,255),name="淺藍"},new ColorSet{color=Color.FromArgb(128, 0,128),name="紫羅蘭"},new ColorSet{color=Color.FromArgb(153,153,153),name="灰色-40%"}}
,{new ColorSet{color=Color.FromArgb(255, 0,255),name="粉紅" },new ColorSet{color=Color.FromArgb(255,104, 0),name="金色"},new ColorSet{color=Color .FromArgb(255,255, 0),name="黃色" },new ColorSet{color=Color.FromArgb( 0,255, 0),name="鮮綠"},new ColorSet{color=Color.FromArgb( 0,255,255),name ="青綠" },new ColorSet{color=Color.FromArgb( 0,204,255),name="天藍"},new ColorSet{color=Color.FromArgb(153, 51,102),name="梅紅" },new ColorSet{ color=Color.FromArgb(192,192,192),name="灰色-25%"}}
,{new ColorSet{color=Color.FromArgb(255,153,204),name="玫瑰紅"},new ColorSet{color=Color.FromArgb(255,204,153),name="茶色"},new ColorSet{color=Color.FromArgb( 255,255,204),name="淺黃" },new ColorSet{color=Color.FromArgb(204,255,204),name="淺綠"},new ColorSet{color=Color.FromArgb(204,255,255),name="淺青綠"}, new ColorSet{color=Color.FromArgb(153,204,255),name="淺藍"},new ColorSet{color=Color.FromArgb(204,153,255),name="淺紫" },new ColorSet{color=Color.FromArgb(255,255,255 ),name="白色" }}
};

        public ColorPicker(Control parentControl)
        {
            InitializeComponent();
            this.parentControl = parentControl;// 保存窗体句柄
            Init();
        }

        private void Init()
        {
            #region 保存原来的颜色
            this.selectedColor = this.parentControl.BackColor;
            #endregion
            #region 设置父窗体
            this.Owner = this.parentControl.TopLevelControl as Form;
            #endregion
            InitLocation();
            InitControls();
        }

        private void InitLocation()
        {
            this.Left = parentControl.TopLevelControl.PointToScreen(parentControl.Location).X;
            this.Top = parentControl.TopLevelControl.PointToScreen(parentControl.Location).Y + parentControl.Height;
        }

        private void InitControls()
        {
            ToolTip toolTip = new ToolTip();

            #region 容器
            Panel pnl = new Panel();
            pnl.Dock = DockStyle.Fill;// 在父容器中填充
            pnl.BorderStyle = BorderStyle.FixedSingle;// 设置边框
            this.Controls.Add(pnl);
            #endregion

            #region 色块
            for (int i = 0; i < 5; i++)
            {
                for (int j = 0; j < 8; j++)
                {
                    btnColor[i, j] = new Button();
                    btnColor[i, j].Left = j * 18 + 6;
                    btnColor[i, j].Top = i * 18 + 6;
                    btnColor[i, j].Width = 12;
                    btnColor[i, j].Height = 12;
                    btnColor[i, j].FlatStyle = FlatStyle.Flat;// 设置按钮风格
                    btnColor[i, j].TabStop = false;// 禁止使用Tab获得焦点

                    switch (Program.language)
                    {
                        case 0:
                            btnColor[i, j].BackColor = colorSetChs[i, j].color;// 设置颜色
                            toolTip.SetToolTip(btnColor[i, j], colorSetChs[i, j].name + Environment.NewLine + "(" + colorSetChs[i, j].color.R + "," + colorSetChs[i, j].color.G + "," + colorSetChs[i, j].color.B + ")");// 设置说明文本
                            break;
                        case 1:
                            btnColor[i, j].BackColor = colorSetCht[i, j].color;// 设置颜色
                            toolTip.SetToolTip(btnColor[i, j], colorSetCht[i, j].name + Environment.NewLine + "(" + colorSetCht[i, j].color.R + "," + colorSetCht[i, j].color.G + "," + colorSetCht[i, j].color.B + ")");// 设置说明文本
                            break;
                        default:
                            break;
                    }

                    btnColor[i, j].Click += new EventHandler(btn_Click);

                    pnl.Controls.Add(btnColor[i, j]);
                }
            }
            #endregion

            #region 其他颜色
            btnOtherColor = new Button();
            switch (Program.language)
            {
                case 0:
                    btnOtherColor.Text = "其他颜色";
                    toolTip.SetToolTip(btnOtherColor, "选择其他颜色");
                    break;
                case 1:
                    btnOtherColor.Text = "其他顏色";
                    toolTip.SetToolTip(btnOtherColor, "選擇其他顏色");
                    break;
                default:
                    break;
            }
            btnOtherColor.Left = 6;
            btnOtherColor.Top = 96;
            btnOtherColor.Width = 138;
            btnOtherColor.Height = 23;
            btnOtherColor.FlatStyle = FlatStyle.Flat;// 设置按钮风格
            btnOtherColor.TabStop = false;// 禁止使用Tab获得焦点

            btnOtherColor.Image = Properties.Resources.icoCustomColor;
            btnOtherColor.ImageAlign = ContentAlignment.MiddleLeft;

            btnOtherColor.Click += new EventHandler(btn_Click);

            pnl.Controls.Add(btnOtherColor);
            #endregion
            #region 屏幕取色
            btnScreenColor = new Button();
            switch (Program.language)
            {
                case 0:
                    btnScreenColor.Text = "屏幕取色";
                    toolTip.SetToolTip(btnScreenColor, "选取屏幕上一点的颜色");
                    break;
                case 1:
                    btnScreenColor.Text = "屏幕取色";
                    toolTip.SetToolTip(btnScreenColor, "選取屏幕上一點的顏色");
                    break;
                default:
                    break;
            }
            btnScreenColor.Left = 6;
            btnScreenColor.Top = 125;
            btnScreenColor.Width = 138;
            btnScreenColor.Height = 23;
            btnScreenColor.FlatStyle = FlatStyle.Flat;// 设置按钮风格
            btnScreenColor.TabStop = false;// 禁止使用Tab获得焦点

            btnScreenColor.Image = Properties.Resources.icoGetColorFromScreen;
            btnScreenColor.ImageAlign = ContentAlignment.MiddleLeft;

            btnScreenColor.Click += new EventHandler(btn_Click);

            pnl.Controls.Add(btnScreenColor);
            #endregion
        }

        private void btn_Click(object sender, EventArgs e)
        {
            Button btn = sender as Button;
            if (btn == btnOtherColor)// 其他颜色
            {
                this.Deactivate -= new EventHandler(ColorPicker_Deactivate);// 取消因失去焦点而消失
                ColorDialog colorDialog = new ColorDialog();
                if (colorDialog.ShowDialog() == DialogResult.OK)
                {
                    selectedColor = colorDialog.Color;
                }
                colorDialog.Dispose();
                SelectedColorChanged(selectedColor, EventArgs.Empty);
            }
            else if (btn == btnScreenColor)// 屏幕取色
            {
                this.Deactivate -= new EventHandler(ColorPicker_Deactivate);// 取消因失去焦点而消失
                GetScreenColorForm gscf = new GetScreenColorForm();
                if (gscf.ShowDialog() == DialogResult.OK)
                {
                    selectedColor = gscf.SelectColor;
                }
                gscf.Dispose();
                SelectedColorChanged(selectedColor, EventArgs.Empty);
            }
            else if (btn != null)// 色块
            {
                selectedColor = btn.BackColor;
                SelectedColorChanged(selectedColor, EventArgs.Empty);
            }
        }

        /// <summary>
        /// 窗体失去焦点，使窗体消失
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ColorPicker_Deactivate(object sender, EventArgs e)
        {
            SelectedColorChanged(selectedColor, EventArgs.Empty);
        }
    }
}