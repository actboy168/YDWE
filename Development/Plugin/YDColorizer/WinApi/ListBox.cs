using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace WinApi
{
    public class ListBox
    {
        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, int lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, string lParam);

        [DllImport("user32.dll", EntryPoint = "SendMessage", SetLastError = true)]
        private static extern int SendMessage(IntPtr hWnd, int wMsg, int wParam, StringBuilder lParam);

        private const int LB_GETCOUNT = 0x18B;
        private const int LB_GETSEL = 0x187;
        private const int LB_GETTEXT = 0x189;
        private const int LB_GETTEXTLEN = 0x18A;

        private const int LB_INSERTSTRING = 0x181;

        /// <summary>
        /// 获取该实例所指向的列表框的句柄
        /// </summary>
        public IntPtr Handle
        {
            get;
            private set;
        }
        
        /// <summary>
        /// 获取列表框中元素的个数
        /// </summary>
        public int Count
        {
            get
            {
                return SendMessage(this.Handle, LB_GETCOUNT, 0, 0);
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
                int length = SendMessage(this.Handle, LB_GETTEXTLEN, index, 0);
                StringBuilder sb = new StringBuilder(length + 1);
                if (SendMessage(this.Handle, LB_GETTEXT, index, sb) < 0)
                {
                    throw new Win32Exception(Marshal.GetLastWin32Error());
                }
                return sb.ToString();
            }
        }

        /// <summary>
        /// 使用已有的列表框句柄创建WinApi的ListBox类实例
        /// </summary>
        /// <param name="handle"></param>
        public ListBox(IntPtr handle)
        {
            this.Handle = handle;
        }

        /// <summary>
        /// 获取某项是否被选择
        /// </summary>
        /// <param name="index"></param>
        /// <returns></returns>
        public bool GetSelected(int index)
        {
            if (index < 0 || index >= this.Count)
            {
                throw new IndexOutOfRangeException();
            }
            if (SendMessage(this.Handle, LB_GETSEL, index, 0) > 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// 插入文本到列表框指定位置
        /// </summary>
        /// <param name="index">插入的位置，从0开始，最大为列表框当前数量</param>
        /// <param name="s">需插入的文本</param>
        /// <returns>插入的位置</returns>
        public int Insert(int index, string s)
        {
            return SendMessage(this.Handle, LB_INSERTSTRING, index, s);
        }
    }
}
