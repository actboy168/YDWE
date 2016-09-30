using System;
using System.Collections.Generic;
using System.Text;
using System.Reflection;
using System.IO;
using System.Windows.Forms;
using System.Drawing;

namespace YDColorizer
{
    public class Config
    {
        /// <summary>
        /// 获取该程序集所在目录
        /// </summary>
        /// <returns>该程序集的完整路径</returns>
        public static string GetThisMoudlePath()
        {
            string str = Assembly.GetExecutingAssembly().CodeBase;// 获取该程序集完整路径
            int start = 8;// 去除file:///
            int end = str.LastIndexOf('/');// 去除文件名xxx.dll
            return str.Substring(start, end - start);
        }

        /// <summary>
        /// 配置文件名字
        /// </summary>
        private const string ConfigName = "YDColorizerConfig.ini";

        /// <summary>
        /// 获取配置文件路径
        /// </summary>
        /// <returns>配置文件的完整路径</returns>
        public static string GetConfigPath()
        {
            return GetThisMoudlePath() + "\\" + ConfigName;
        }

        /// <summary>
        /// 获取是否存在配置文件
        /// </summary>
        /// <returns></returns>
        public static bool IsConfigExists()
        {
            return File.Exists(GetConfigPath());
        }

        /// <summary>
        /// 保存窗口大小到配置文件
        /// </summary>
        /// <param name="form"></param>
        public static void SaveDialogSize(Form form)
        {
            if (IsConfigExists() == true)
            {
                if (WinApi.Ini.GetInt(GetConfigPath(), "Application", "RememberSize") != 0)
                {
                    WinApi.Ini.SetInt(GetConfigPath(), "Application", "Width", form.Width);
                    WinApi.Ini.SetInt(GetConfigPath(), "Application", "Height", form.Height);
                }
            }
        }

        /// <summary>
        /// 从配置文件加载窗口大小
        /// </summary>
        /// <param name="form"></param>
        /// <returns>成功则返回true</returns>
        public static bool LoadDialogSize(Form form)
        {
            if (IsConfigExists() == true)// 配置文件存在
            {
                if (WinApi.Ini.GetInt(GetConfigPath(), "Application", "RememberSize") != 0)// 配置保存窗口大小
                {
                    int width = WinApi.Ini.GetInt(GetConfigPath(), "Application", "Width");
                    int height = WinApi.Ini.GetInt(GetConfigPath(), "Application", "Height");
                    if (width > 0 && height > 0)// 范围正确
                    {
                        form.Width = width;
                        form.Height = height;
                        return true;
                    }
                    else// 范围错误
                    {
                        return false;
                    }
                }
                else// 配置不保存窗口大小
                {
                    return false;
                }
            }
            else// 配置文件不存在
            {
                return false;
            }
        }

        /// <summary>
        /// 从配置文件中加载按钮颜色
        /// </summary>
        /// <param name="btn">按钮</param>
        public static void LoadColor(Button btn)
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
        /// <param name="btn">按钮</param>
        public static void SaveColor(Button btn)
        {
            if (File.Exists(Config.GetConfigPath()) == true)// 配置文件存在
            {
                Color color = btn.BackColor;
                string str = color.R + "," + color.G + "," + color.B;
                WinApi.Ini.SetString(Config.GetConfigPath(), "UserCustomColor", btn.Name, str);
            }
        }

        /// <summary>
        /// 是否显示托盘图标
        /// </summary>
        /// <returns></returns>
        public static bool IsNotifyIconVisible()
        {
            int visible = WinApi.Ini.GetInt(GetConfigPath(), "Application", "NotifyIcon");
            return visible == 0 ? false : true;
        }

        /// <summary>
        /// 读取配置文件中设置的语言
        /// </summary>
        /// <returns>0：简体；1：繁体</returns>
        public static int GetLanguage()
        {
            int language = WinApi.Ini.GetInt(GetConfigPath(), "Application", "Language");
            if (language != 1)
            {
                return 0;// 默认简体
            }
            return language;// 繁体
        }

        public static void CreateDefault()
        {
            using (StreamWriter sw = new StreamWriter(GetConfigPath(), false, Encoding.Default))
            {
                sw.WriteLine("[YDColorizerConfig]");
                sw.WriteLine("; 这是YDColorizer的配置文件，请勿随意修改");
                sw.WriteLine("[Application]");
                sw.WriteLine("Language=0");
                sw.WriteLine("; 语言,0->简体中文,1->繁体中文");
                sw.WriteLine("NotifyIcon=0");
                sw.WriteLine("; 托盘图标,0->不显示,非0->显示");
                sw.WriteLine("RememberSize=0");
                sw.WriteLine("; 记住上次的窗口大小，0->不记，非0->记");
                sw.WriteLine("Width=400");
                sw.WriteLine("; 上次的窗口宽度");
                sw.WriteLine("Height=300");
                sw.WriteLine("; 上次的窗口高度");
                sw.WriteLine("[UserCustomColor]");
                sw.WriteLine("btn1stColor=255,0,0");
                sw.WriteLine("; 主颜色的rgb值");
                sw.WriteLine("btn2ndColor=0,255,0");
                sw.WriteLine("; 第一候选颜色的rgb值");
                sw.WriteLine("btn3rdColor=0,0,255");
                sw.WriteLine("; 第二候选颜色的rgb值");
                sw.WriteLine("btn4thColor=255,255,0");
                sw.WriteLine("; 第三候选颜色的rgb值");
            }
        }
    }
}
