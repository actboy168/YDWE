using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Drawing;

namespace YDColorizer
{
    class MyMousePosition
    {        /// <summary>
        /// 获取鼠标坐标
        /// </summary>
        /// <param name="IpPoint"></param>
        /// <returns></returns>
        [DllImport("user32.dll", EntryPoint = "GetCursorPos")]
        private static extern bool GetCursorPos(ref Point IpPoint);

        public static Point GetMousePosition()
        {
            Point p = new Point();
            GetCursorPos(ref p);
            return p;
        }

        public static int GetMousePositionX()
        {
            return GetMousePosition().X;
        }

        public static int GetMousePositionY()
        {
            return GetMousePosition().Y;
        }

        /// 设置鼠标坐标
        /// </summary>
        /// <param name="X"></param>
        /// <param name="Y"></param>
        /// <returns></returns>
        [DllImport("user32.dll", EntryPoint = "SetCursorPos")]
        private static extern bool SetCursorPOS(int X, int Y);

        public static void SetMousePosition(int x, int y)
        {
            SetCursorPOS(x, y);
        }
        public static void SetMousePosition(Point p)
        {
            SetCursorPOS(p.X, p.Y);
        }
        public static void SetMousePositionX(int x)
        {
            SetCursorPOS(x, GetMousePositionY());
        }
        public static void SetMousePositionY(int y)
        {
            SetCursorPOS(GetMousePositionX(), y);
        }
    }
}