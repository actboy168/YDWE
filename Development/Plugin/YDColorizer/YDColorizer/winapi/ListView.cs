using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace WinApi
{
    class ListView
    {
        [DllImport("user32.dll")]
        private static extern int SendMessage(IntPtr hWnd, int msg, int w, int l);

        [DllImport("user32.dll", EntryPoint = "SendMessage")]
        private static extern IntPtr SendMessageB(IntPtr hWnd, int msg, int w, int l);


        private const int LVM_GETITEMCOUNT = 0x1004;


        private const int LVM_GETHEADER = 0x101F;

        private const int HDM_GETITEMCOUNT = 0x1200;

        public IntPtr Handle
        {
            get;
            private set;
        }


        public ListView(IntPtr handle)
        {
            this.Handle = handle;
        }

        public int Row
        {
            get
            {
                return SendMessage(this.Handle, LVM_GETITEMCOUNT, 0, 0);
            }
        }

        public IntPtr Header
        {
            get
            {
                return SendMessageB(this.Handle, LVM_GETHEADER, 0, 0);
            }
        }

        public int Column
        {
            get
            {
                return SendMessage(this.Header, HDM_GETITEMCOUNT, 0, 0);
            }
        }







































        [DllImport("user32.dll")]//找出某个窗口的创建者(线程或进程),返回创建者的标志符

        private static extern int GetWindowThreadProcessId(int hwnd, out int processId);
    }
}
