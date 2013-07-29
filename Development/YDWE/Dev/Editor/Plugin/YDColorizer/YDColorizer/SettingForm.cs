using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace YDColorizer
{
    public partial class SettingForm : Form
    {
        public SettingForm()
        {
            InitializeComponent();
        }

        private void SettingForm_Load(object sender, EventArgs e)
        {
            if (Config.IsConfigExists() == false)
            {
                MessageBox.Show("配置文件不存在！");
                this.Close();
            }
            else
            {
                int language = WinApi.Ini.GetInt(Config.GetConfigPath(), "Application", "Language");
                int notifyIcon = WinApi.Ini.GetInt(Config.GetConfigPath(), "Application", "NotifyIcon");
                int rememberSize = WinApi.Ini.GetInt(Config.GetConfigPath(), "Application", "RememberSize");
                if (language == 0)
                {
                    rdochs.Checked = true;
                }
                else
                {
                    rdocht.Checked = true;
                }
                if (notifyIcon == 0)
                {
                    rdoNotifyIconHide.Checked = true;
                }
                else
                {
                    rdoNotifyIconShow.Checked = true;
                }
                if (rememberSize == 0)
                {
                    rdoRememberSizeNo.Checked = true;
                }
                else
                {
                    rdoRememberSizeYes.Checked = true;
                }
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            if (rdochs.Checked == true)
            {
                WinApi.Ini.SetInt(Config.GetConfigPath(), "Application", "Language", 0);
            }
            if (rdocht.Checked == true)
            {
                WinApi.Ini.SetInt(Config.GetConfigPath(), "Application", "Language", 1);
            }
            if (rdoNotifyIconHide.Checked == true)
            {
                WinApi.Ini.SetInt(Config.GetConfigPath(), "Application", "NotifyIcon", 0);
            }
            if (rdoNotifyIconShow.Checked == true)
            {
                WinApi.Ini.SetInt(Config.GetConfigPath(), "Application", "NotifyIcon", 1);
            }
            if (rdoRememberSizeNo.Checked == true)
            {
                WinApi.Ini.SetInt(Config.GetConfigPath(), "Application", "RememberSize", 0);
            }
            if (rdoRememberSizeYes.Checked == true)
            {
                WinApi.Ini.SetInt(Config.GetConfigPath(), "Application", "RememberSize", 1);
            }
            this.Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.Close();
        }
    }
}
