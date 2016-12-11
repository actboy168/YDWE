using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.IO;
using System.Text;

namespace YDColorizer
{
    class GlobalException
    {
        public static void CatchException(Exception e, string default_msg)
        {
            StringBuilder sb = new StringBuilder();
            sb.AppendLine("***************************************************************");
            sb.AppendLine("    你的颜色插件发生了严重的错误，请截图并发到www.ydwe.net。   ");
            sb.AppendLine("***************************************************************");
            if (e != null)
            {                
                sb.AppendLine("【异常类型】：" + e.GetType().Name);
                sb.AppendLine("【异常信息】：" + e.Message);
                sb.AppendLine("【堆栈调用】：" + e.StackTrace);
            }
            else
            {
                sb.AppendLine("【未处理异常】：" + default_msg);
            }
            sb.AppendLine("***************************************************************");
            MessageBox.Show(sb.ToString(), "YDColorizer");
        }

        static void CatchThreadException(object sender, System.Threading.ThreadExceptionEventArgs e)
        {
            CatchException(e.Exception as Exception, e.ToString());
        }

        static void CatchUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            CatchException(e.ExceptionObject as Exception, e.ToString());
        }
 
        public static void Initialize()
        {
            try
            { 
                Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException);
                Application.ThreadException += new System.Threading.ThreadExceptionEventHandler(CatchThreadException);
                AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(CatchUnhandledException);
            }
            catch (Exception e)
            {
                CatchException(e, e.ToString());
            }
        }
    }
}
