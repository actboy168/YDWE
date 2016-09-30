using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace YDColorizer
{
    class MyTextBox : TextBox
    {
        /// <summary>
        /// 是否锁定TextChange事件
        /// </summary>
        public bool lockTextChange = false;

        /// <summary>
        /// Undo列表
        /// </summary>
        public Stack<UndoRedoInfo> undoStack = new Stack<UndoRedoInfo>();

        /// <summary>
        /// Redo列表
        /// </summary>
        public Stack<UndoRedoInfo> redoStack = new Stack<UndoRedoInfo>();

        /// <summary>
        /// 用于保存每次UndoRedo信息
        /// </summary>
        public struct UndoRedoInfo
        {
            public UndoRedoInfo(string text, int cursorLocation)
            {
                this.Text = text;
                this.CursorLocation = cursorLocation;
            }
            public readonly string Text;// 文本内容
            public readonly int CursorLocation;// 光标位置
        }

        /// <summary>
        /// 对上一次的操作
        /// </summary>
        public UndoRedoInfo lastText;

        public event EventHandler undoStackChange;
        public event EventHandler redoStackChange;

        protected override void OnTextChanged(EventArgs e)
        {
            if (lockTextChange == false)
            {
                base.OnTextChanged(e);
                AddToUndoStack(this.Text);
            }
        }

        public void AddToUndoStack(string text)
        {
            this.ClearUndo();// 禁止自身的Undo
            this.redoStack.Clear();// 清空重做
            this.undoStack.Push(lastText);// 写入上一次的内容
            this.lastText = new UndoRedoInfo(text, this.SelectionStart);// 保存本次的内容
            undoStackChange(undoStack.Count, EventArgs.Empty);
            redoStackChange(redoStack.Count, EventArgs.Empty);
        }

        protected override void OnKeyDown(KeyEventArgs e)
        {
            if (e.Control == true)// 按下Ctrl键
            {
                if (e.KeyCode == Keys.Z)// 按下Z键
                {
                    return;
                }
                if (e.KeyCode == Keys.Y)// 按下Y键
                {
                    return;
                }
                if (e.KeyCode == Keys.A)// 按下A键
                {
                    this.SelectAll();
                }
            }
            base.OnKeyDown(e);
        }

        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);
            if (e.Button == MouseButtons.Right)
            {
                ContextMenu cm = new ContextMenu();
                cm.MenuItems.Add(new MenuItem("撤销", new EventHandler(Undo)) { Enabled = (undoStack.Count > 0) });
                cm.MenuItems.Add(new MenuItem("剪切", new EventHandler((object object_sender, EventArgs EventArgs_e) => { this.Cut(); })) { Enabled = (this.SelectionLength > 0) });
                cm.MenuItems.Add(new MenuItem("复制", new EventHandler((object object_sender, EventArgs EventArgs_e) => { this.Copy(); })) { Enabled = (this.SelectionLength > 0) });
                cm.MenuItems.Add(new MenuItem("粘贴", new EventHandler((object object_sender, EventArgs EventArgs_e) => { this.Paste(); })));
                cm.MenuItems.Add(new MenuItem("删除", new EventHandler((object object_sender, EventArgs EventArgs_e) => { this.SelectedText = string.Empty; })) { Enabled = (this.SelectionLength > 0) });
                cm.MenuItems.Add(new MenuItem("全选", new EventHandler((object object_sender, EventArgs EventArgs_e) => { this.SelectAll(); })));
                cm.Show(this, e.Location);
            }
        }

        public new void Undo()
        {
            Undo(null, EventArgs.Empty);
        }

        public void Undo(object sender, EventArgs e)
        {
            this.lockTextChange = true;
            if (undoStack.Count > 0)// 可以撤销
            {
                this.redoStack.Push(new UndoRedoInfo(this.Text, this.SelectionStart));
                this.lastText = undoStack.Peek();
                this.Text = lastText.Text;
                this.SelectionStart = lastText.CursorLocation;
                this.undoStack.Pop();
                undoStackChange(undoStack.Count, EventArgs.Empty);
                redoStackChange(redoStack.Count, EventArgs.Empty);
            }
            this.lockTextChange = false;
        }

        public void Redo()
        {
            Redo(null, EventArgs.Empty);
        }

        public void Redo(object sender, EventArgs e)
        {
            this.lockTextChange = true;
            if (redoStack.Count > 0)// 可以重做
            {
                this.undoStack.Push(new UndoRedoInfo(this.Text, this.SelectionStart));
                this.Text = redoStack.Peek().Text;
                this.SelectionStart = redoStack.Peek().CursorLocation;
                this.redoStack.Pop();
                undoStackChange(undoStack.Count, EventArgs.Empty);
                redoStackChange(redoStack.Count, EventArgs.Empty);
            }
            this.lockTextChange = false;
        }
    }
}