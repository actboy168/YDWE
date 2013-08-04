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

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e.Control == true)// 按下Ctrl键
            {
                if (e.KeyCode == Keys.Z)// 按下Z键
                {
                    //this.Undo();
                    return;
                }
                if (e.KeyCode == Keys.Y)// 按下Y键
                {
                    //this.Redo();
                    return;
                }
            }
            base.OnKeyDown(e);
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