using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.IO;

namespace YDColorizer
{
    class UserCustomColor
    {
        /// <summary>
        /// 从配置文件加载按钮颜色
        /// </summary>
        /// <param name="btn">需加载的按钮</param>
        public static void Load(Button btn)
        {
            if (File.Exists(Config.GetConfigPath()) == true)// 配置文件存在
            {
                string[] color = WinApi.Ini.GetString(Config.GetConfigPath(), "UserCustomColor", btn.Name).Split(',');// 获取配置文件中的rgb
                if (color.Length == 3)// 格式正确
                {
                    byte r, g, b;
                    if (byte.TryParse(color[0], out r) == true && byte.TryParse(color[1], out g) == true && byte.TryParse(color[2], out b) == true)// 范围正确且能转换
                    {
                        btn.BackColor = Color.FromArgb(r, g, b);
                    }
                    else// 范围错误或者文本错误
                    {
                        btn.BackColor = Color.Black;
                    }
                }
                else// 格式错误
                {
                    btn.BackColor = Color.Black;
                }
            }
            else// 配置文件不存在
            {
                btn.BackColor = Color.Black;
            }
        }

        /// <summary>
        /// 保存按钮颜色到配置文件
        /// </summary>
        /// <param name="btn">需保存的按钮</param>
        public static void Save(Button btn)
        {
            if (File.Exists(Config.GetConfigPath()) == true)// 配置文件存在
            {
                Color color = btn.BackColor;
                string str = color.R + "," + color.G + "," + color.B;
                WinApi.Ini.SetString(Config.GetConfigPath(), "UserCustomColor", btn.Name, str);
            }
        }
    }
}
