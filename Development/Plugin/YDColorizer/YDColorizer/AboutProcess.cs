using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace YDColorizer
{
    class AboutProcess
    {
        [DllImport("user32.dll", EntryPoint = "GetWindowThreadProcessId")]
        private static extern int GetWindowThreadProcessId(IntPtr hwnd, out int ID);

        /// <summary>
        /// 获取窗口的进程号
        /// </summary>
        /// <param name="hwnd">窗口句柄</param>
        /// <returns>进程号</returns>
        public static int GetProcessId(IntPtr hwnd)
        {
            int processId;
            GetWindowThreadProcessId(hwnd, out processId);
            return processId;
        }
    }
}
