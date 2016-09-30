using System;
using System.Text;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Drawing;

namespace WinApi
{
    public class TextBox
    {
        [DllImport("user32.dll", EntryPoint = "FindWindowEx", SetLastError = true)]
        private static extern IntPtr FindWindowEx(IntPtr hWndParent, IntPtr hWndChildAfter, string lpszClass, string lpszWindow);

        [DllImport("user32.dll", EntryPoint = "GetParent", SetLastError = true)]
        private static extern IntPtr GetParent(IntPtr hwnd);

        [DllImport("user32.dll", EntryPoint = "GetWindowRect", SetLastError = true)]
        private static extern bool GetWindowRect(IntPtr hWnd, out LPRECT lpRect);

        [DllImport("user32.dll", EntryPoint = "MoveWindow", SetLastError = true)]
        private static extern bool MoveWindow(IntPtr hWnd, int x, int y, int nWidth, int nHeight, bool bRepaint);

        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, string lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, StringBuilder lParam);

        [DllImport("user32.dll", EntryPoint = "ScreenToClient", SetLastError = true)]
        private static extern int ScreenToClient(IntPtr hwnd, ref Point lpPoint);

        private const int WM_GETTEXT = 13;
        private const int WM_SETTEXT = 12;

        /// <summary>
        /// 该结构接收文本框的左上角和右下角的屏幕坐标。
        /// </summary>
        private struct LPRECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;
        }

        /// <summary>
        /// 获取该实例所指向的文本框的句柄
        /// </summary>
        public IntPtr Handle
        {
            get;
            private set;
        }

        /// <summary>
        /// 获取或设置文本框距离容器左边缘的像素
        /// </summary>
        public int Left
        {
            get
            {
                LPRECT rect = new LPRECT();
                if (GetWindowRect(this.Handle, out rect) == false)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
                Point point = new Point(rect.Left, rect.Top);
                if (ScreenToClient(GetParent(this.Handle), ref point) == 0)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
                return point.X;
            }
            set
            {
                if (MoveWindow(this.Handle, value, this.Top, this.Width, this.Height, true) == false)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
            }
        }
        
        /// <summary>
        /// 获取或设置文本框距离容器上边缘的像素
        /// </summary>
        public int Top
        {
            get
            {
                LPRECT rect = new LPRECT();
                if (GetWindowRect(this.Handle, out rect) == false)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
                Point point = new Point(rect.Left, rect.Top);
                if (ScreenToClient(GetParent(this.Handle), ref point) == 0)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
                return point.Y;
            }
            set
            {
                if (MoveWindow(this.Handle, this.Left, value, this.Width, this.Height, true) == false)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
            }
        }

        /// <summary>
        /// 获取或设置文本框的宽度
        /// </summary>
        public int Width
        {
            get
            {
                LPRECT rect = new LPRECT();
                if (GetWindowRect(this.Handle, out rect) == false)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
                return rect.Right - rect.Left;
            }
            set
            {
                if (MoveWindow(this.Handle, this.Left, this.Top, value, this.Height, true) == false)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
            }
        }

        /// <summary>
        /// 获取或设置文本框的高度
        /// </summary>
        public int Height
        {
            get
            {
                LPRECT rect = new LPRECT();
                if (GetWindowRect(this.Handle, out rect) == false)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
                return rect.Bottom - rect.Top;
            }
            set
            {
                if (MoveWindow(this.Handle, this.Left, this.Top, this.Width, value, true) == false)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
            }
        }

        /// <summary>
        /// 获取或设置文本框的左上角
        /// </summary>
        public Point Location
        {
            get
            {
                return new Point(this.Left, this.Top);
            }
            set
            {
                this.Left = value.X;
                this.Top = value.Y;
            }
        }

        /// <summary>
        /// 获取或设置文本框显示的文本（设置对.Net程序无效）
        /// </summary>
        public string Text
        {
            get
            {
                StringBuilder sb = new StringBuilder(1024);
                SendMessage(this.Handle, WM_GETTEXT, sb.Capacity, sb);
                return sb.ToString();
            }
            set
            {
                SendMessage(this.Handle, WM_SETTEXT, 0, value);
            }
        }

        /// <summary>
        /// 使用已有的文本框句柄创建WinApi的TextBox类实例
        /// </summary>
        /// <param name="handle">文本框</param>
        public TextBox(IntPtr handle)
        {
            this.Handle = handle;
        }

        /// <summary>
        /// 搜索在窗口内符合要求的文本框，并以此文本框创建WinApi的TextBox类实例
        /// </summary>
        /// <param name="hWndParent">文本框所在窗口句柄</param>
        /// <param name="txtText">文本框上显示的文本</param>
        public TextBox(IntPtr hWndParent, string txtText)
        {
            this.Handle = FindWindowEx(hWndParent, IntPtr.Zero, null, txtText);
        }

        /// <summary>
        /// 搜索在窗口内符合要求的文本框，并以此文本框创建WinApi的TextBox类实例
        /// </summary>
        /// <param name="hWndParent">文本框所在窗口句柄</param>
        /// <param name="hWndChildAfter">文本框在哪个控件之后，IntPtr.Zero则从第一个开始搜索</param>
        /// <param name="lpszClass">文本框类名，一般为"Edit"</param>
        /// <param name="txtText">文本框上显示的文本</param>
        public TextBox(IntPtr hWndParent, IntPtr hWndChildAfter, string lpszClass, string txtText)
        {
            this.Handle = FindWindowEx(hWndParent, hWndChildAfter, lpszClass, txtText);
        }
    }
}
