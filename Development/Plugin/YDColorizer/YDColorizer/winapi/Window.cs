using System;
using System.Text;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing;

namespace WinApi
{
    public class Window
    {
        [DllImport("user32.dll", EntryPoint = "FindWindow", SetLastError = true)]
        private static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        [DllImport("user32.dll", EntryPoint = "GetWindowRect", SetLastError = true)]
        private static extern bool GetWindowRect(IntPtr hWnd, out LPRECT lpRect);

        [DllImport("user32.dll", EntryPoint = "GetWindowText", SetLastError = true)]
        private static extern bool GetWindowText(IntPtr hWnd, StringBuilder lpString, int nMaxCount);

        [DllImport("user32.dll", EntryPoint = "GetWindowTextLength", SetLastError = true)]
        private static extern int GetWindowTextLength(IntPtr hWnd);

        [DllImport("user32.dll", EntryPoint = "IsIconic", SetLastError = true)]
        private static extern bool IsIconic(IntPtr hwnd);

        [DllImport("user32.dll", EntryPoint = "IsWindowVisible", SetLastError = true)]
        private static extern bool IsWindowVisible(IntPtr hWnd);

        [DllImport("user32.dll", EntryPoint = "IsZoomed", SetLastError = true)]
        private static extern bool IsZoomed(IntPtr hwnd);

        [DllImport("user32.dll", EntryPoint = "MoveWindow", SetLastError = true)]
        private static extern bool MoveWindow(IntPtr hWnd, int x, int y, int nWidth, int nHeight, bool bRepaint);

        [DllImport("user32.dll", EntryPoint = "SetWindowText", SetLastError = true)]
        private static extern bool SetWindowText(IntPtr hWnd, string lpString);

        [DllImport("user32.dll", EntryPoint = "ShowWindow", SetLastError = true)]
        private static extern bool ShowWindow(IntPtr hWnd, int nCmdShow);

        [DllImport("user32.dll", EntryPoint = "WindowFromPoint", SetLastError = true)]
        private static extern IntPtr WindowFromPoint(int x, int y);

        [DllImport("user32.dll", EntryPoint = "SetParent",SetLastError=true)]
        private static extern IntPtr SetParent(            IntPtr hWndChild,            IntPtr hWndNewParent        );

        private const int SW_HIDE = 0;
        private const int SW_MAXIMIZE = 3;
        private const int SW_MINIMIZE = 6;
        private const int SW_SHOW = 5;

        /// <summary>
        /// 该结构接收窗口的左上角和右下角的屏幕坐标。
        /// </summary>
        private struct LPRECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;
        }

        /// <summary>
        /// 获取该实例所指向的窗口的句柄
        /// </summary>
        public IntPtr Handle
        {
            get;
            private set;
        }

        /// <summary>
        /// 获取或设置窗口左上角的横坐标
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
                return rect.Left;
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
        /// 获取或设置窗口左上角的纵坐标
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
                return rect.Top;
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
        /// 获取或设置窗口宽度
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
        /// 获取或设置窗口高度
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
        /// 获取或设置窗口的左上角
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
        /// 获取或设置窗口标题
        /// </summary>
        public string Text
        {
            get
            {
                StringBuilder sb = new StringBuilder(GetWindowTextLength(this.Handle) + 1);
                if (GetWindowText(this.Handle, sb, sb.Capacity) == false)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
                return sb.ToString();
            }
            set
            {
                if (SetWindowText(this.Handle, value) == false)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
            }
        }

        /// <summary>
        /// 获取或设置窗口的可见性
        /// </summary>
        public bool Visible
        {
            get
            {
                return IsWindowVisible(this.Handle);
            }
            set
            {
                if (value == true)
                {
                    ShowWindow(this.Handle, SW_SHOW);
                }
                else if (value == false)
                {
                    ShowWindow(this.Handle, SW_HIDE);
                }
            }
        }

        /// <summary>
        /// 获取或设置窗口的可视状态
        /// </summary>
        public FormWindowState WindowState
        {
            get
            {
                if (IsZoomed(this.Handle) == true)
                {
                    return FormWindowState.Maximized;
                }
                else if (IsIconic(this.Handle) == true)
                {
                    return FormWindowState.Minimized;
                }
                else
                {
                    return FormWindowState.Normal;
                }
            }
            set
            {
                if (value == FormWindowState.Maximized)
                {
                    ShowWindow(this.Handle, SW_MAXIMIZE);
                }
                if (value == FormWindowState.Minimized)
                {
                    ShowWindow(this.Handle, SW_MINIMIZE);
                }
                if (value == FormWindowState.Normal)
                {
                    ShowWindow(this.Handle, SW_SHOW);
                }
            }
        }

        /// <summary>
        /// 使用已有的窗口句柄创建WinApi的Window类实例
        /// </summary>
        /// <param name="handle">窗口句柄</param>
        public Window(IntPtr handle)
        {
            this.Handle = handle;
        }

        /// <summary>
        /// 寻找包含该点的窗口，并以此创建WinApi的Window类实例
        /// </summary>
        /// <param name="point"></param>
        public Window(Point point)
        {
            this.Handle = WindowFromPoint(point.X, point.Y);
        }

        /// <summary>
        /// 寻找包含点(x,y)的窗口，并以此创建WinApi的Window类实例
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        public Window(int x, int y)
        {
            this.Handle = WindowFromPoint(x, y);
        }

        /// <summary>
        /// 根据窗口标题枚举所有窗口，并以第一个符合要求的窗口创建WinApi的Window类实力
        /// </summary>
        /// <param name="lpWindowName">窗口标题</param>
        public Window(string lpWindowName)
        {
            this.Handle = FindWindow(null, lpWindowName);
        }

        /// <summary>
        /// 根据窗口类名及窗口标题枚举所有窗口，并以第一个符合要求的窗口创建WinApi的Window类实例
        /// </summary>
        /// <param name="lpClassName">窗口类名</param>
        /// <param name="lpWindowName">窗口标题</param>
        public Window(string lpClassName, string lpWindowName)
        {
            this.Handle = FindWindow(lpClassName, lpWindowName);
        }

        /// <summary>
        /// 添加指定句柄的控件到当前实例的窗口
        /// </summary>
        /// <param name="handle"></param>
        public void Add(IntPtr handle)
        {
            if (SetParent(handle, this.Handle) == IntPtr.Zero)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }
        }

        /// <summary>
        /// 将该窗口内指定句柄的控件移除该窗口
        /// </summary>
        /// <param name="handle"></param>
        public void Remove(IntPtr handle)
        {
            SetParent(handle, IntPtr.Zero);
        }

        /// <summary>
        /// 显示窗口
        /// </summary>
        public void Show()
        {
            this.Visible = true;
        }

        /// <summary>
        /// 隐藏窗口
        /// </summary>
        public void Hide()
        {
            this.Visible = false;
        }
    }
}
