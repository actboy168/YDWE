using System;
using System.Text;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace WinApi
{
    public class Button
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
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, int lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, string lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, StringBuilder lParam);

        [DllImport("user32.dll", EntryPoint = "GetClientRect", SetLastError = true)]
        private static extern bool GetClientRect(IntPtr hWnd, out LPRECT lpRect);

        [DllImport("user32.dll", EntryPoint = "ScreenToClient", SetLastError = true)]
        private static extern int ScreenToClient(IntPtr hWnd, ref Point lpPoint);

        [DllImport("user32.dll", EntryPoint = "PostMessage", SetLastError = true)]
        private static extern int PostMessage(IntPtr hWnd, int wMsg, int wParam, int lParam);

        private const int BM_CLICK = 245;
        private const int WM_GETTEXT = 13;
        private const int WM_SETTEXT = 12;

        /// <summary>
        /// 该结构接收按钮的左上角和右下角的屏幕坐标。
        /// </summary>
        private struct LPRECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;
        }

        /// <summary>
        /// 获取该实例所指向的按钮的句柄
        /// </summary>
        public IntPtr Handle
        {
            get;
            private set;
        }

        /// <summary>
        /// 获取或设置按钮距离容器左边缘的像素
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
        /// 获取或设置按钮距离容器上边缘的像素
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
        /// 获取或设置按钮的宽度
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
        /// 获取或设置按钮的高度
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
        /// 获取或设置按钮的左上角
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
        /// 获取或设置按钮显示的文本（设置对.Net程序无效）
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
        /// 使用已有的按钮句柄创建WinApi的Button类实例
        /// </summary>
        /// <param name="handle">按钮</param>
        public Button(IntPtr handle)
        {
            this.Handle = handle;
        }

        /// <summary>
        /// 搜索在窗口内符合要求的按钮，并以此按钮创建WinApi的Button类实例
        /// </summary>
        /// <param name="hWndParent">按钮所在窗口句柄</param>
        /// <param name="btnText">按钮上显示的文本</param>
        public Button(IntPtr hWndParent, string btnText)
        {
            this.Handle = FindWindowEx(hWndParent, IntPtr.Zero, null, btnText);
        }

        /// <summary>
        /// 搜索在窗口内符合要求的按钮，并以此按钮创建WinApi的Button类实例
        /// </summary>
        /// <param name="hWndParent">按钮所在窗口句柄</param>
        /// <param name="hWndChildAfter">按钮在哪个控件之后，IntPtr.Zero则从第一个开始搜索</param>
        /// <param name="lpszClass">按钮类名，一般为"Button"</param>
        /// <param name="btnText">按钮上显示的文本</param>
        public Button(IntPtr hWndParent, IntPtr hWndChildAfter, string lpszClass, string btnText)
        {
            this.Handle = FindWindowEx(hWndParent, hWndChildAfter, lpszClass, btnText);
        }

        /// <summary>
        /// 点击按钮
        /// </summary>
        public void Click()
        {
            SendMessage(this.Handle, BM_CLICK, 0, 0);
        }

        /// <summary>
        /// 异步点击按钮
        /// </summary>
        public void ClickAsync()
        {
            PostMessage(this.Handle, BM_CLICK, 0, 0);
        }
    }
}
