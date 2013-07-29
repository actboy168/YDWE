using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace YDColorizer
{
    class MyRichTextBox : RichTextBox
    {
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
    }
}