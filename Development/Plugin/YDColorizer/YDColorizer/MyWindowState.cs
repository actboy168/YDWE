using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace YDColorizer
{
    class MyWindowState
    {
        public class Visible
        {
            [DllImport("user32.dll", EntryPoint = "ShowWindow")]
            private static extern bool SetWindowVisible(IntPtr hWnd, int nCmdShow);

            private const int SW_HIDE = 0;
            private const int SW_SHOW = 5;

            /// <summary>
            /// 获取窗口是否隐藏
            /// </summary>
            /// <param name="hWnd"></param>
            /// <returns></returns>
            [DllImport("user32.dll", EntryPoint = "IsWindowVisible")]
            public static extern bool IsWindowVisible(IntPtr hWnd);

            /// <summary>
            /// 隐藏窗口
            /// </summary>
            /// <param name="hWnd"></param>
            public static void HideWindow(IntPtr hWnd)
            {
                SetWindowVisible(hWnd, SW_HIDE);
            }
        }

        public class GetLocation
        {
            [DllImport("user32.dll", EntryPoint = "GetWindowRect")]
            private static extern bool GetWindowRect(IntPtr hWnd, out LPRECT lpRect);

            private struct LPRECT
            {
                public int Left;
                public int Top;
                public int Right;
                public int Bottom;
            }

            /// <summary>
            /// 获取窗口的距离屏幕左方的距离
            /// </summary>
            /// <param name="hWnd"></param>
            /// <returns></returns>
            public static int GetLeft(IntPtr hWnd)
            {
                LPRECT rect = new LPRECT();
                GetWindowRect(hWnd, out rect);
                return rect.Left;
            }

            /// <summary>
            /// 获取窗口的距离屏幕上方的距离
            /// </summary>
            /// <param name="hWnd"></param>
            /// <returns></returns>
            public static int GetTop(IntPtr hWnd)
            {
                LPRECT rect = new LPRECT();
                GetWindowRect(hWnd, out rect);
                return rect.Top;
            }
        }

        public class GetSize
        {
            [DllImport("user32.dll", EntryPoint = "GetWindowRect")]
            private static extern bool GetWindowRect(IntPtr hWnd, out LPRECT lpRect);

            private struct LPRECT
            {
                public int Left;
                public int Top;
                public int Right;
                public int Bottom;
            }

            /// <summary>
            /// 获取窗口的宽度
            /// </summary>
            /// <param name="hWnd"></param>
            /// <returns></returns>
            public static int GetWidth(IntPtr hWnd)
            {
                LPRECT rect = new LPRECT();
                GetWindowRect(hWnd, out rect);
                return rect.Right - rect.Left;
            }

            /// <summary>
            /// 获取窗口的高度
            /// </summary>
            /// <param name="hWnd"></param>
            /// <returns></returns>
            public static int GetHeight(IntPtr hWnd)
            {
                LPRECT rect = new LPRECT();
                GetWindowRect(hWnd, out rect);
                return rect.Bottom - rect.Top;
            }
        }

        public class Text
        {
            [DllImport("user32.dll", EntryPoint = "SendMessage")]
            private static extern int SendMessage(IntPtr hwnd, int wMsg, int wParam, StringBuilder lParam);

            private const int WM_GETTEXT = 13;

            public static string GetText(IntPtr hWnd)
            {
                StringBuilder sb = new StringBuilder(1024);
                SendMessage(hWnd, WM_GETTEXT, sb.Capacity, sb);
                return sb.ToString();
            }

            [DllImport("user32.dll", EntryPoint = "GetWindowText")]
            private static extern int ApiGetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);

            public static string GetTitle(IntPtr hWnd)
            {
                StringBuilder sb = new StringBuilder(1024);
                ApiGetWindowText(hWnd, sb, sb.Capacity);
                return sb.ToString();
            }

            [DllImport("user32.dll", EntryPoint = "SendMessage")]
            private static extern int SendMessage(IntPtr hwnd, int wMsg, int wParam, string lParam);

            private const int WM_SETTEXT = 12;

            public static void SetText(IntPtr hWnd, string s)
            {
                SendMessage(hWnd, WM_SETTEXT, 0, s);
            }
        }

        public class ClickButton
        {
            [DllImport("user32.dll", EntryPoint = "SendMessage")]
            private static extern int SendMessage(IntPtr hwnd, int wMsg, int wParam, int lParam);

            private const int BM_CLICK = 245;

            public static void Click(IntPtr hWnd)
            {
                SendMessage(hWnd, BM_CLICK, 0, 0);
            }
        }
    }
}