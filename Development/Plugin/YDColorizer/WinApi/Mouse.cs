using System;
using System.Runtime.InteropServices;
using System.ComponentModel;
using System.Drawing;

namespace WinApi
{
    public class Mouse
    {
        [DllImport("user32.dll", EntryPoint = "ClipCursor", SetLastError = true)]
        private static extern bool ClipCursor(ref LPRECT lpRect);

        [DllImport("user32.dll", EntryPoint = "ClipCursor", SetLastError = true)]
        private static extern bool ClipCursor(IntPtr lpRect);

        [DllImport("user32.dll", EntryPoint = "GetClipCursor", SetLastError = true)]
        private static extern bool GetClipCursor(ref LPRECT lpRect);

        [DllImport("user32.dll", EntryPoint = "GetCursorPos", SetLastError = true)]
        private static extern bool GetCursorPos(ref Point lpPoint);

        [DllImport("user32.dll", EntryPoint = "SetCursorPos", SetLastError = true)]
        private static extern bool SetCursorPos(int x, int y);

        [DllImport("user32.dll", EntryPoint = "ShowCursor", SetLastError = true)]
        private static extern int ShowCursor(bool bShow);

        /// <summary>
        /// 该结构表示区域的左上角和右下角的屏幕坐标。
        /// </summary>
        private struct LPRECT
        {
            public int Left;
            public int Top;
            public int Right;
            public int Bottom;
        }

        /// <summary>
        /// 获取鼠标在屏幕上的坐标
        /// </summary>
        /// <returns></returns>
        public static Point GetPosition()
        {
            Point p = new Point();
            if (GetCursorPos(ref p) == false)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }
            return p;
        }

        /// <summary>
        /// 获取鼠标在屏幕上的x轴坐标
        /// </summary>
        /// <returns></returns>
        public static int GetX()
        {
            return GetPosition().X;
        }

        /// <summary>
        /// 获取鼠标在屏幕上的y轴坐标
        /// </summary>
        /// <returns></returns>
        public static int GetY()
        {
            return GetPosition().Y;
        }

        /// <summary>
        /// 设置鼠标在屏幕上的坐标
        /// </summary>
        /// <param name="x">x轴坐标</param>
        /// <param name="y">y轴坐标</param>
        public static void SetPosition(int x, int y)
        {
            if (SetCursorPos(x, y) == false)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }
        }

        /// <summary>
        /// 设置鼠标在屏幕上的坐标
        /// </summary>
        /// <param name="p">目标点</param>
        public static void SetPosition(Point p)
        {
            SetPosition(p.X, p.Y);
        }

        /// <summary>
        /// 设置鼠标在屏幕上的x轴坐标
        /// </summary>
        /// <param name="x"></param>
        public static void SetX(int x)
        {
            SetPosition(x, GetY());
        }

        /// <summary>
        /// 设置鼠标在屏幕上的y轴坐标
        /// </summary>
        /// <param name="y"></param>
        public static void SetY(int y)
        {
            SetPosition(GetX(), y);
        }

        /// <summary>
        /// 限制鼠标在屏幕上的一个区域内
        /// </summary>
        /// <param name="rect">限制区域</param>
        public static void Look(Rectangle rect)
        {
            LPRECT lpRect = new LPRECT() { Left = rect.Left, Top = rect.Top, Right = rect.Right, Bottom = rect.Bottom };
            if (ClipCursor(ref lpRect) == false)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }
        }

        /// <summary>
        /// 解除鼠标的区域限制
        /// </summary>
        public static void UnLock()
        {
            if (ClipCursor(IntPtr.Zero) == false)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }
        }

        /// <summary>
        /// 获取限制鼠标的区域（若无限制则返回屏幕区域）
        /// </summary>
        /// <returns>限制鼠标的区域</returns>
        public static Rectangle GetLockRect()
        {
            LPRECT lpRect = new LPRECT();
            if (GetClipCursor(ref lpRect) == false)
            {
                throw new Win32Exception(Marshal.GetLastWin32Error());
            }
            return new Rectangle(lpRect.Left, lpRect.Top, lpRect.Right - lpRect.Left, lpRect.Bottom - lpRect.Top);
        }

        /// <summary>
        /// 显示鼠标
        /// </summary>
        public static void Show()
        {
            int count = -1;// 内部的显示计数器
            while (count >= 0)
            {
                count = ShowCursor(true);
            }
        }

        /// <summary>
        /// 隐藏鼠标
        /// </summary>
        public static void Hide()
        {
            int count = 0;// 内部的显示计数器
            while (count < 0)
            {
                count = ShowCursor(false);
            }
        }
    }
}
