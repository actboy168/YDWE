using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace WinApi
{
    public class ComboBox
    {
        [DllImport("user32.dll", EntryPoint = "FindWindowEx", SetLastError = true)]
        private static extern IntPtr FindWindowEx(IntPtr hWndParent, IntPtr hWndChildAfter, string lpszClass, string lpszWindow);

        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, int lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, StringBuilder lParam);

        private const int CB_GETCOUNT = 0x146;
        private const int CB_GETCURSEL = 0x147;
        private const int CB_GETLBTEXT = 0x148;
        private const int CB_GETLBTEXTLEN = 0x149;
        private const int CB_SETCURSEL = 0x14E;


        public const int CB_SELECTSTRING = 0x14D;


        /// <summary>
        /// 获取该实例所指向的组合框的句柄
        /// </summary>
        public IntPtr Handle
        {
            get;
            private set;
        }

        /// <summary>
        /// 获取组合框中元素的个数
        /// </summary>
        public int Count
        {
            get
            {
                return SendMessage(this.Handle, CB_GETCOUNT, 0, 0);
            }
        }

        public string this[int index]
        {
            get
            {
                if (index < 0 || index >= this.Count)
                {
                    throw new IndexOutOfRangeException();
                }
                int length = SendMessage(this.Handle, CB_GETLBTEXTLEN, index, 0);
                StringBuilder sb = new StringBuilder(length + 1);
                if (SendMessage(this.Handle, CB_GETLBTEXT, index, sb) < 0)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
                return sb.ToString();
            }
        }

        public int SelectedIndex
        {
            set
            {
                if (value < 0 || value >= this.Count)
                {
                    throw new IndexOutOfRangeException();
                }
                SendMessage(this.Handle, CB_SETCURSEL, value, 0);
            }
        }

        /// <summary>
        /// 使用已有的组合框句柄创建WinApi的ComboBox类实例
        /// </summary>
        /// <param name="handle"></param>
        public ComboBox(IntPtr handle)
        {
            this.Handle = handle;
        }

        public ComboBox(IntPtr hWndParent, IntPtr hWndChildAfter, string lpszClass, string cmbText)
        {
            this.Handle = FindWindowEx(hWndParent, hWndChildAfter, lpszClass, cmbText);
        }

        public void ClearSelected()
        {
            SendMessage(this.Handle, CB_SETCURSEL, -1, 0);
        }




        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, string lParam);

        public void SearchAndSelect(string s)
        {
            SendMessage(this.Handle, CB_SELECTSTRING, -1, s);
        }
    }
}
