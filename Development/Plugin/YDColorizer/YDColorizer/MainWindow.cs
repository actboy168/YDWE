using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace YDColorizer
{
    public partial class MainWindow : Form
    {
        private List<string> dialogBoxTitles;

        public MainWindow()
        {
            InitializeComponent();
            Environment.CurrentDirectory = AppDomain.CurrentDomain.BaseDirectory;
            Init();
        }

        private void Init()
        {
            Program.language = AboutConfig.GetLanguage();
            this.InitText();
            this.InitNotifyIcon();
            this.dialogBoxTitles = AboutConfig.LoadSearchTitles();
        }

        private void InitText()
        {
            switch (Program.language)
            {
                case 0:// 简体中文
                    chkHook.Text = "监听";
                    chkNotifyIcon.Text = "托盘图标";
                    notifyIcon.Text = "监听中";
                    break;
                case 1:// 繁体中文
                    chkHook.Text = "監聽";
                    chkNotifyIcon.Text = "托盤圖標";
                    notifyIcon.Text = "監聽中";
                    break;
                default:
                    break;
            }
        }

        private void InitNotifyIcon()
        {
            this.notifyIcon.Icon = Properties.Resources.icoYDColorizerRuning;
            if (AboutConfig.NotifyIconVisible() == true)
            {
                notifyIcon.Visible = true;
                chkNotifyIcon.Checked = true;
            }
            else
            {
                notifyIcon.Visible = false;
                chkNotifyIcon.Checked = false;
            }
        }

        private void timer_Tick(object sender, EventArgs e)
        {
            foreach (var title in this.dialogBoxTitles)
            {
                IntPtr handle = GetWindowHandle.GetDialogBoxHandle(title);
                if (handle != IntPtr.Zero)// 找到
                {
                    if (MyWindowState.Visible.IsWindowVisible(handle) == true)// 非处理中
                    {
                        EditDialogBox edb = new EditDialogBox(handle);
                        edb.Show();
                        break;
                    }
                }
            }
        }

        /// <summary>
        /// 禁止在处理时关闭
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void MainWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            foreach (var title in this.dialogBoxTitles)
            {
                IntPtr handle = GetWindowHandle.GetDialogBoxHandle(title);
                if (handle != IntPtr.Zero)// 找到
                {
                    if (MyWindowState.Visible.IsWindowVisible(handle) == false)// 存在处理中
                    {
                        e.Cancel = true;// 取消退出
                        return;
                    }
                }
            }
            notifyIcon.Visible = false;
        }

        private void chkHook_CheckedChanged(object sender, EventArgs e)
        {
            if (chkHook.Checked == true)
            {
                timer.Enabled = true;
                switch (Program.language)
                {
                    case 0:
                        notifyIcon.Text = "监听中";
                        break;
                    case 1:
                        notifyIcon.Text = "監聽中";
                        break;
                    default:
                        break;
                }
                notifyIcon.Icon = Properties.Resources.icoYDColorizerRuning;
            }
            else
            {
                timer.Enabled = false;
                switch (Program.language)
                {
                    case 0:
                        notifyIcon.Text = "已暂停";
                        break;
                    case 1:
                        notifyIcon.Text = "已暫停";
                        break;
                    default:
                        break;
                }
                notifyIcon.Icon = Properties.Resources.icoYDColorizerStop;
            }
        }

        private void chkNotifyIcon_CheckedChanged(object sender, EventArgs e)
        {
            notifyIcon.Visible = chkNotifyIcon.Checked;
        }

        private void notifyIcon_Click(object sender, EventArgs e)
        {
            if (chkHook.Checked == true)
            {
                chkHook.Checked = false;
            }
            else
            {
                chkHook.Checked = true;
            }
        }
    }
}