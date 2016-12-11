using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace YDColorizer
{
    class GetWindowHandle
    {
        [DllImport("user32.dll", EntryPoint = "FindWindow")]
        private static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        /// <summary>
        /// 搜索的窗口的类名
        /// </summary>
        private const string DialogBoxClassName = "#32770";

        [DllImport("user32.dll", EntryPoint = "FindWindowEx")]
        private static extern IntPtr FindWindowEx(IntPtr hwndParent, IntPtr hwndChildAfter, string lpszClass, string lpszWindow);

        /// <summary>
        /// 获取对话框句柄
        /// </summary>
        /// <param name="windowTitle"></param>
        /// <returns></returns>
        public static IntPtr GetDialogBoxHandle(string windowTitle)
        {
            return FindWindow(DialogBoxClassName, windowTitle);
        }

        /// <summary>
        /// 获取标签句柄
        /// </summary>
        /// <param name="hWnd"></param>
        /// <returns></returns>
        public static IntPtr GetStaticHandle(IntPtr hWnd)
        {
            return FindWindowEx(hWnd, IntPtr.Zero, "Static", null);
        }

        /// <summary>
        /// 获取文本框句柄
        /// </summary>
        /// <param name="hWnd"></param>
        /// <returns></returns>
        public static IntPtr GetEditHandle(IntPtr hWnd)
        {
            return FindWindowEx(hWnd, IntPtr.Zero, "Edit", null);
        }

        /// <summary>
        /// 获取确定按钮句柄
        /// </summary>
        /// <param name="hWnd"></param>
        /// <returns></returns>
        public static IntPtr GetBtnOkHandle(IntPtr hWnd)
        {
            return FindWindowEx(hWnd, GetEditHandle(hWnd), "Button", null);
        }

        /// <summary>
        /// 获取取消按钮句柄
        /// </summary>
        /// <param name="hWnd"></param>
        /// <returns></returns>
        public static IntPtr GetBtnCancelHandle(IntPtr hWnd)
        {
            return FindWindowEx(hWnd, GetBtnOkHandle(hWnd), "Button", null);
        }
    }
}