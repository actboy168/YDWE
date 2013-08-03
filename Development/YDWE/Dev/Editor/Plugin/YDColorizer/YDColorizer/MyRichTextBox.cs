using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace YDColorizer
{
    class MyRichTextBox : RichTextBox
    {
        private const int WM_PAINT = 0x000F;

        /// <summary>
        /// 是否忽略控件重绘
        /// </summary>
        public bool ignoreRefresh = false;

        /// <summary>
        /// 是否锁定TextChange事件
        /// </summary>
        public bool lockTextChange = false;

        protected override void OnTextChanged(EventArgs e)
        {
            if (lockTextChange == false)
            {
                base.OnTextChanged(e);
                this.ClearUndo();// 禁止自身的Undo
            }
        }

        public MyRichTextBox()
        {
            this.LanguageOption = RichTextBoxLanguageOptions.AutoFont;
        }

        protected override void WndProc(ref Message m)
        {
            if (m.Msg == WM_PAINT && ignoreRefresh == true)
            {
                return;
            }
            base.WndProc(ref m);
        }
    }
}