using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace YDColorizer
{
    public partial class GetScreenColorForm : Form
    {
        private const int picsX = 7;// 预览色块横轴个数
        private const int picsY = 7;// 预览色块纵轴个数

        /// <summary>
        /// 保存屏幕
        /// </summary>
        private Bitmap screen;

        /// <summary>
        /// 容器
        /// </summary>
        private Panel pnl;

        /// <summary>
        /// 预览色块
        /// </summary>
        private Panel[,] pics;

        /// <summary>
        /// 当前点色块
        /// </summary>
        private Panel picCurrentPoint;

        /// <summary>
        /// 预览色值
        /// </summary>
        private Label lbl;

        /// <summary>
        /// 选中颜色
        /// </summary>
        public Color SelectColor
        {
            get;
            set;
        }

        public GetScreenColorForm()
        {
            InitializeComponent();
            Init();
        }

        private void Init()
        {
            InitScreen();
            InitPanel();
            InitPics();
            InitCurrentPoint();
            InitPreviewColorRGB();
        }

        private void InitScreen()
        {
            this.Width = Screen.PrimaryScreen.Bounds.Width;
            this.Height = Screen.PrimaryScreen.Bounds.Height;
            Image img = new Bitmap(this.Width, this.Height);
            Graphics g = Graphics.FromImage(img);
            g.CopyFromScreen(0, 0, 0, 0, new Size(this.Width, this.Height));
            screen = new Bitmap(img);
            this.BackgroundImage = screen;
            img.Dispose();
        }

        private void InitPanel()
        {
            pnl = new Panel();
            pnl.Width = 10 * picsX + 102;
            pnl.Height = 10 * picsY + 2;
            pnl.Left = MousePosition.X + 5;
            pnl.Top = MousePosition.Y + 5;
            pnl.BorderStyle = BorderStyle.FixedSingle;
            this.Controls.Add(pnl);
        }

        private void InitPics()
        {
            pics = new Panel[picsX, picsY];
            for (int i = 0; i < picsX; i++)
            {
                for (int j = 0; j < picsY; j++)
                {
                    pics[i, j] = new Panel();
                    pics[i, j].Width = 10;
                    pics[i, j].Height = 10;
                    pics[i, j].Left = i * 10;
                    pics[i, j].Top = j * 10;
                    if (i == picsX / 2 && j == picsY / 2)
                    {
                        pics[i, j].BorderStyle = BorderStyle.FixedSingle;
                    }
                    pnl.Controls.Add(pics[i, j]);
                }
            }
        }

        private void InitCurrentPoint()
        {
            picCurrentPoint = new Panel();
            picCurrentPoint.Width = 90;
            picCurrentPoint.Height = 15;
            picCurrentPoint.Left = picsX * 10 + 5;
            picCurrentPoint.Top = 3;
            picCurrentPoint.BorderStyle = BorderStyle.FixedSingle;
            pnl.Controls.Add(picCurrentPoint);
        }

        private void InitPreviewColorRGB()
        {
            lbl = new Label();
            lbl.Left = picsX * 10 + 5;
            lbl.Top = 22;
            lbl.Height = 100;
            pnl.Controls.Add(lbl);
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            int screenX = this.Width;
            int screenY = this.Height;

            int mouseX = MousePosition.X;
            int mouseY = MousePosition.Y;
            Color tempColor = Color.Black;

            for (int i = 0; i < picsX; i++)
            {
                for (int j = 0; j < picsY; j++)
                {
                    if ((mouseX - picsX / 2 + i >= 0) && (mouseX - picsY / 2 + i < screenX) && (mouseY - picsX / 2 + j >= 0) && (mouseY - picsY / 2 + j < screenY))// 确保不会超出屏幕
                    {
                        tempColor = screen.GetPixel(mouseX - picsX / 2 + i, mouseY - picsY / 2 + j);
                        pics[i, j].BackColor = tempColor;
                        if (i == picsX / 2 && j == picsY / 2)
                        {
                            lbl.Text = "RGB:\n(" + tempColor.R + "," + tempColor.G + "," + tempColor.B + ")";
                        }
                    }
                    else
                    {
                        pics[i, j].BackColor = Color.Black;
                    }
                }
            }

            picCurrentPoint.BackColor = pics[picsX / 2, picsY / 2].BackColor;

            #region 移动预览框
            if (mouseX + 5 + pnl.Width <= screenX)
            {
                pnl.Left = mouseX + 5;
            }
            else
            {
                pnl.Left = mouseX - 5 - pnl.Width;
            }
            if (mouseY + 5 + pnl.Height <= screenY)
            {
                pnl.Top = mouseY + 5;
            }
            else
            {
                pnl.Top = mouseY - 5 - pnl.Height;
            }
            #endregion
        }

        private void GetScreenColorForm_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)// Esc键取消
            {
                this.DialogResult = DialogResult.Cancel;
            }
            if (WinApi.KeyBoard.IsUpDown() == true)// UP
            {
                WinApi.Mouse.SetY(WinApi.Mouse.GetY() - 1);
            }
            if (WinApi.KeyBoard.IsDownDown() == true)// Down
            {
                WinApi.Mouse.SetY(WinApi.Mouse.GetY() + 1);
            }
            if (WinApi.KeyBoard.IsLeftDown() == true)// Left
            {
                WinApi.Mouse.SetX(WinApi.Mouse.GetX() - 1);
            }
            if (WinApi.KeyBoard.IsRightDown() == true)// RIGHT
            {
                WinApi.Mouse.SetX(WinApi.Mouse.GetX() + 1);
            }
        }

        private void GetScreenColorForm_Click(object sender, EventArgs e)
        {
            MouseEventArgs mea = e as MouseEventArgs;
            if (mea != null)
            {
                if (mea.Button == MouseButtons.Left)// 鼠标左键确定
                {
                    SelectColor = screen.GetPixel(MousePosition.X, MousePosition.Y);
                    this.DialogResult = DialogResult.OK;
                }
                if (mea.Button == MouseButtons.Right)// 鼠标右键取消
                {
                    this.DialogResult = DialogResult.Cancel;
                }
            }
        }

        /// <summary>
        /// 关闭前释放资源
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void GetScreenColorForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            timer.Stop();
            screen.Dispose();
        }
    }
}