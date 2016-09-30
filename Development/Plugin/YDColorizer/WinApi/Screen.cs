using System;
using System.Runtime.InteropServices;
using System.Drawing;

namespace WinApi
{
    public class Screen
    {
        [DllImport("user32.dll", EntryPoint = "GetDC", SetLastError = true)]
        private static extern IntPtr GetDC(IntPtr hwnd);
        [DllImport("user32.dll", EntryPoint = "ReleaseDC", SetLastError = true)]
        private static extern Int32 ReleaseDC(IntPtr hwnd, IntPtr hdc);
        [DllImport("gdi32.dll", EntryPoint = "GetPixel", SetLastError = true)]
        private static extern uint GetPixel(IntPtr hdc, int nXPos, int nYPos);

        /// <summary>
        /// 获取屏幕上一点的颜色
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <returns></returns>
        public static Color GetPixelColor(int x, int y)
        {
            IntPtr hdc = GetDC(IntPtr.Zero);
            uint pixel = GetPixel(hdc, x, y);
            ReleaseDC(IntPtr.Zero, hdc);
            Color color = Color.FromArgb((int)(pixel & 0x000000FF), (int)(pixel & 0x0000FF00) >> 8, (int)(pixel & 0x00FF0000) >> 16);
            return color;
        }
    }
}
