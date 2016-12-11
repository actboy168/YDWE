using System;
using System.Collections.Generic;
using System.Text;
using YDColorizer;
using System.Windows.Forms;

public class WEInit
{
    /// <summary>
    /// 搜索的窗口的类名
    /// </summary>
    private const string DialogBoxClassName = "#32770";

    [MTAThread]
    public static void begin()
    {
        YDColorizer.GlobalException.Initialize();

        Timer timer = new Timer();

        #region 配置文件不存在，创建默认配置文件
        if (Config.IsConfigExists() == false)
        {
            Config.CreateDefault();
        }
        #endregion

        Program.language = Config.GetLanguage();

        List<string> dialogBoxTitles = AboutConfig.LoadSearchTitles();

        #region 托盘图标
        if (Config.IsNotifyIconVisible() == true)
        {
            NotifyIcon notifyIcon = new NotifyIcon();
            notifyIcon.Icon = YDColorizer.Properties.Resources.icoYDColorizerRuning;
            switch (Program.language)
            {
                case 0:
                    notifyIcon.Text = "YDWE颜色插件正在执行" + Environment.NewLine + "点击图标可以暂时停止执行";
                    break;
                case 1:
                    notifyIcon.Text = "YDWE顏色插件正在執行" + Environment.NewLine + "點擊圖標可以暫時停止執行";
                    break;
                default:
                    break;
            }
            notifyIcon.Visible = true;
            notifyIcon.Click += new EventHandler((object object_sender, EventArgs EventArgs_e) =>
            {
                if (timer.Enabled == true)
                {
                    timer.Enabled = false;
                    notifyIcon.Icon = YDColorizer.Properties.Resources.icoYDColorizerStop;
                    switch (Program.language)
                    {
                        case 0:
                            notifyIcon.Text = "YDWE颜色插件已暂停" + Environment.NewLine + "点击图标可以恢复执行";
                            break;
                        case 1:
                            notifyIcon.Text = "YDWE顏色插件已暫停" + Environment.NewLine + "點擊圖標可以恢復執行";
                            break;
                        default:
                            break;
                    }
                }
                else
                {
                    timer.Enabled = true;
                    notifyIcon.Icon = YDColorizer.Properties.Resources.icoYDColorizerRuning;
                    switch (Program.language)
                    {
                        case 0:
                            notifyIcon.Text = "YDWE颜色插件正在执行" + Environment.NewLine + "点击图标可以暂时停止执行";
                            break;
                        case 1:
                            notifyIcon.Text = "YDWE顏色插件正在執行" + Environment.NewLine + "點擊圖標可以暫時停止執行";
                            break;
                        default:
                            break;
                    }
                }
            });
        }
        #endregion

        timer.Interval = 100;// 设置搜索间隔为100毫秒

        EditDialogBox edb = new EditDialogBox();

        timer.Tick += new EventHandler((object object_sender, EventArgs EventArgs_e) =>
        {
            try
            {
                if (edb.hWnd == IntPtr.Zero)
                {
                    foreach (var title in dialogBoxTitles)// 遍历标题数组寻找对话框
                    {
                        WinApi.Window dialogBox = new WinApi.Window(DialogBoxClassName, title);// 搜索we物体编辑器的文本编辑框
                        if (dialogBox.Handle != IntPtr.Zero)// 找到
                        {
                            int thisProcessId = System.Diagnostics.Process.GetCurrentProcess().Id;
                            int targetProcessId = AboutProcess.GetProcessId(dialogBox.Handle);
                            if (thisProcessId == targetProcessId)// 搜索的对话框与该ydwe插件同进程
                            {
                                edb.AttachDialog(dialogBox.Handle);// 重建模拟窗口
                                break;
                            }
                        }
                    }
                }
            }
            catch (Exception e)
            {
                timer.Stop();
                GlobalException.CatchException(e, e.ToString());
            }
        });

        timer.Start();// 启动搜索计时器
    }
}