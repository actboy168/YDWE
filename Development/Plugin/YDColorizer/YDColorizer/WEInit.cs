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

        List<string> dialogBoxTitles = AboutConfig.LoadSearchTitles();

        EditDialogBox edb = new EditDialogBox();
        int thisProcessId = System.Diagnostics.Process.GetCurrentProcess().Id;

        timer.Interval = 100;// 设置搜索间隔为100毫秒
        timer.Tick += new EventHandler((object object_sender, EventArgs EventArgs_e) =>
        {
            try
            {
                if (edb.hWnd == IntPtr.Zero)
                {
                    foreach (var title in dialogBoxTitles)// 遍历标题数组寻找对话框
                    {
                        WinApi.Window dialogBox = new WinApi.Window(DialogBoxClassName, title);// 搜索we物体编辑器的文本编辑框
                        if (dialogBox.Handle != IntPtr.Zero && thisProcessId == AboutProcess.GetProcessId(dialogBox.Handle))// 搜索的对话框与该ydwe插件同进程
                        {
                            edb.AttachDialog(dialogBox.Handle);// 重建模拟窗口
                            break;
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