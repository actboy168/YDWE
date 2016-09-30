using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Drawing;
using System.Threading;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;
using System.Reflection;

namespace YDColorizer
{
    class TextConvert
    {
        /// <summary>
        /// 将编辑框的文本转到预览框
        /// </summary>
        /// <param name="txtEdit"></param>
        /// <param name="txtPreview"></param>
        public static void ConvertToPreviewTextBox(MyTextBox txtEdit, MyRichTextBox txtPreview)
        {
            txtEdit.lockTextChange = true;
            txtPreview.lockTextChange = true;

            MyRichTextBox txtPreviewClone = new MyRichTextBox()
            {
                BackColor = txtPreview.BackColor,
                ForeColor = txtPreview.ForeColor,
                ignoreRefresh = true// 忽略控件自身的重绘，提高响应速度
            };

            int cursorLocation = txtEdit.SelectionStart;
            txtEdit.Text = txtEdit.Text.Replace(Environment.NewLine, "|n");// 转换回车换行为|n
            txtEdit.SelectionStart = cursorLocation;
            char[] charArray = txtEdit.Text.ToCharArray();// 将原来的字符串转为字符数组处理，并转换回车换行为|n
            StringBuilder buffer = new StringBuilder();// 用于缓冲还没输出的字符

            for (int i = 0; i < charArray.Length; i++)
            {
                if (charArray[i] == '|')// 转义字符
                {
                    if ((i + 1) < charArray.Length && charArray[i + 1] == '|')// 字符'|'
                    {
                        buffer.Append('|');// 将字符'|'输入到缓冲区
                        i++;
                        continue;
                    }
                    if (((i + 1) < charArray.Length) && ((charArray[i + 1] == 'n') || (charArray[i + 1] == 'N')))// 换行
                    {
                        Color currentColor = txtPreviewClone.SelectionColor;// 保存当前颜色
                        txtPreviewClone.AppendText(buffer.ToString() + Environment.NewLine);// 将缓冲字符和换行输出
                        buffer = new StringBuilder();// 清空缓冲区
                        txtPreviewClone.SelectionColor = currentColor;// 恢复颜色
                        i++;// 跳过n或N
                        continue;
                    }
                    else if (((i + 9) < charArray.Length) && ((charArray[i + 1] == 'c') || (charArray[i + 1] == 'C')))// 颜色开始
                    {
                        string strColor = charArray[i + 2].ToString() + charArray[i + 3].ToString()// A
                                        + charArray[i + 4].ToString() + charArray[i + 5].ToString()// R
                                        + charArray[i + 6].ToString() + charArray[i + 7].ToString()// G
                                        + charArray[i + 8].ToString() + charArray[i + 9].ToString()// B
                                        ;
                        if (IsColor(strColor) == true)// 可以转化为颜色
                        {
                            txtPreviewClone.AppendText(buffer.ToString());// 将缓冲字符和换行输出
                            buffer = new StringBuilder();// 清空缓冲区
                            txtPreviewClone.SelectionColor = ConvertToColor(strColor);
                            i += 9;// 跳过c或C和8位的颜色值
                            continue;
                        }
                    }
                    else if (((i + 1) < charArray.Length) && ((charArray[i + 1] == 'r') || (charArray[i + 1] == 'R')))// 颜色结束
                    {
                        txtPreviewClone.AppendText(buffer.ToString());// 将缓冲字符和换行输出
                        buffer = new StringBuilder();// 清空缓冲区
                        txtPreviewClone.SelectionColor = Color.White;
                        i++;// 跳过r或R
                        continue;
                    }
                }
                buffer.Append(charArray[i].ToString());// 将字符输入到缓冲区
            }
            txtPreviewClone.AppendText(buffer.ToString());// 将剩余的字符输出

            txtPreview.Rtf = txtPreviewClone.Rtf;
            txtPreviewClone.Dispose();
            txtEdit.lockTextChange = false;
            txtPreview.lockTextChange = false;
        }

        public static void ConvertToEditTextBox(MyRichTextBox txtPreview, MyTextBox txtEdit)
        {
            txtEdit.lockTextChange = true;

            #region 克隆控件
            MyRichTextBox txtPreviewClone = new MyRichTextBox()
            {
                Rtf = txtPreview.Rtf,
                SelectionStart = 0,// 从开头开始处理
                SelectionLength = 1,// 每次的处理长度
                ignoreRefresh = true// 忽略控件自身的重绘，提高响应速度
            };
            #endregion

            bool isHighLight = false;// 标志是否高亮中
            StringBuilder sb = new StringBuilder();// 存储已经处理的文本
            Color currentColor = txtPreviewClone.SelectionColor;// 当前颜色

            #region 处理第一个字符
            if (currentColor != Color.White)
            {
                sb.Append("|c" + ColorToHex(txtPreviewClone.SelectionColor));
                isHighLight = true;
            }
            #endregion

            int length = txtPreviewClone.TextLength;// 处理的总文本长度

            while (txtPreviewClone.SelectionStart < length)
            {
                if (txtPreviewClone.SelectionColor != currentColor)// 改变颜色
                {
                    if (isHighLight)// 结束高亮
                    {
                        sb.Append("|r");
                        isHighLight = false;
                    }
                    if (txtPreviewClone.SelectionColor != Color.White)// 不是默认颜色，开始高亮
                    {
                        sb.Append("|c" + ColorToHex(txtPreviewClone.SelectionColor));
                        isHighLight = true;
                    }
                    currentColor = txtPreviewClone.SelectionColor;
                }
                #region 将回车转为|n
                if (txtPreviewClone.SelectedText == "\n")
                {
                    sb.Append("|n");
                    txtPreviewClone.SelectionStart++;
                    continue;
                }
                #endregion
                #region 将|转为||
                else if (txtPreviewClone.SelectedText == "|")
                {
                    sb.Append("||");
                    txtPreviewClone.SelectionStart++;
                    continue;
                }
                #endregion
                sb.Append(txtPreviewClone.SelectedText);
                txtPreviewClone.SelectionStart++;
            }
            if (isHighLight)
            {
                sb.Append("|r");
            }

            txtEdit.Text = sb.ToString();
            txtEdit.AddToUndoStack(sb.ToString());

            txtPreviewClone.Dispose();// 销毁克隆对象
            txtEdit.lockTextChange = false;
        }

        //public static void ConvertToEditTextBoxEx(MyRichTextBox txtPreview, MyTextBox txtEdit)
        //{
        //    txtEdit.lockTextChange = true;

        //    string rtf = txtPreview.Rtf;

        //    int colorTableStart = rtf.IndexOf(@"\colortbl");// 颜色表开始位置
        //    int colorTableEnd = rtf.IndexOf('}', colorTableStart);// 颜色表结束位置
        //    string[] colorTable = rtf.Substring(colorTableStart, colorTableEnd - colorTableStart).Split(new char[] { ';' }, StringSplitOptions.RemoveEmptyEntries);// 获取颜色表子串并分割

        //    List<Color> colors = new List<Color>();// 存储rtf所用到的所有颜色
        //    for (int i = 1; i < colorTable.Length; i++)
        //    {
        //        string[] color = colorTable[i].Replace("red", string.Empty).Replace("green", string.Empty).Replace("blue", string.Empty).Split(new char[] { '\\' }, StringSplitOptions.RemoveEmptyEntries);// 获取颜色字符串的RGB值
        //        colors.Add(Color.FromArgb(int.Parse(color[0]), int.Parse(color[1]), int.Parse(color[2])));// 将颜色存入集合
        //    }

        //    string processStr = rtf.Substring(rtf.IndexOf(@"\fs18") + @"\fs18".Length).Replace(Environment.NewLine, string.Empty);// 截取处理字符串并去除换行符
        //    processStr = processStr.Substring(0, processStr.LastIndexOf(@"\par"));// 去除结尾的\par

        //    if (processStr.StartsWith(" ") == true)// 非汉字开头
        //    {
        //        processStr = processStr.Substring(1);
        //    }

        //    StringBuilder finishStr = new StringBuilder();// 处理完成字符串            
        //    for (int i = 0; i < processStr.Length; i++)
        //    {
        //        if (processStr[i] == '\\')
        //        {
        //            if (i + 1 < processStr.Length && processStr[i + 1] == '\\')// '\'
        //            {
        //                finishStr.Append('\\');
        //                i++;
        //            }
        //            else if (i + 7 < processStr.Length && processStr[i + 1] == '\'' && processStr[i + 4] == '\\' && processStr[i + 5] == '\'')// 中文等非ASCII字符
        //            {
        //                finishStr.Append(Encoding.GetEncoding(936).GetString(new byte[] { (byte)EncodingConvert(processStr[i + 2], processStr[i + 3]), (byte)EncodingConvert(processStr[i + 6], processStr[i + 7]) }));
        //                i += 7;
        //            }
        //            else if (i + 3 < processStr.Length && processStr[i + 1] == 'c' && processStr[i + 2] == 'f' && processStr[i + 3] >= '0' && processStr[i + 3] <= '9')// 颜色
        //            {
        //                int j = i + 3;
        //                string num = string.Empty;
        //                while (j < processStr.Length && processStr[j] >= '0' && processStr[j] <= '9')
        //                {
        //                    num += processStr[j];
        //                    j++;
        //                }
        //                i = j - 1;
        //                if (j < processStr.Length && processStr[j] == ' ')
        //                {
        //                    i++;
        //                }
        //                int colorIndex = int.Parse(num) - 1;
        //                if (colors[colorIndex].R == 255 && colors[colorIndex].G == 255 && colors[colorIndex].B == 255)
        //                {
        //                    finishStr.Append("|r");
        //                }
        //                else
        //                {
        //                    finishStr.Append("|c" + ColorToHex(colors[colorIndex]));
        //                }
        //            }
        //            else if (i + 3 < processStr.Length && processStr[i + 1] == 'p' && processStr[i + 2] == 'a' && processStr[i + 3] == 'r')// 换行
        //            {
        //                finishStr.Append("|n");
        //                i += 3;
        //            }
        //        }
        //        else
        //        {
        //            finishStr.Append(processStr[i]);
        //        }
        //    }

        //    txtEdit.Text = finishStr.ToString();
        //    txtEdit.AddToUndoStack(finishStr.ToString());

        //    txtEdit.lockTextChange = false;
        //}

        private static int EncodingConvert(char char1, char char2)
        {
            int a = 0, b = 0;
            if (char1 >= '0' && char1 <= '9')
            {
                a = char1 - '0';
            }
            else if (char1 >= 'a' && char1 <= 'f')
            {
                a = char1 - 'a' + 10;
            }
            if (char2 >= '0' && char2 <= '9')
            {
                b = char2 - '0';
            }
            else if (char2 >= 'a' && char2 <= 'f')
            {
                b = char2 - 'a' + 10;
            }
            return a * 16 + b;
        }

        public static void SetTxtEditSingleColor(MyTextBox txtEdit, Color color)
        {
            txtEdit.lockTextChange = true;
            txtEdit.SelectedText = "|c" + ColorToHex(color) + txtEdit.SelectedText + "|r";
            txtEdit.lockTextChange = false;
            txtEdit.Select();// 激活控件
        }

        public static void SetTxtPreviewSingleColor(MyRichTextBox txtPreview, Color color)
        {
            txtPreview.lockTextChange = true;
            int cursorLocation = txtPreview.SelectionStart;// 保存编辑位置
            int selectLength = txtPreview.SelectionLength;// 保存处理字符串的长度
            txtPreview.SelectionColor = color;// 设置颜色
            txtPreview.SelectionStart = cursorLocation + selectLength;// 移动光标
            txtPreview.SelectionLength = 0;// 清空选择
            txtPreview.lockTextChange = false;
            txtPreview.Select();// 激活控件
        }

        public static void SetTxtEditGradual(MyTextBox txtEdit, Color startColor, Color endColor)
        {
            int processLength = 0;// 去除控制字符外的字符长度
            string selectedText = txtEdit.SelectedText;// 选择的文本
            for (int i = 0; i < selectedText.Length; i++)
            {
                if (i + 1 < selectedText.Length && selectedText[i] == '|' && selectedText[i + 1] == '|')
                {
                    processLength++;
                    i++;
                    continue;
                }
                if (i + 1 < selectedText.Length && selectedText[i] == '|' && selectedText[i + 1] == 'n')
                {
                    i++;
                    continue;
                }
                if (i + 1 < selectedText.Length && selectedText[i] == '|' && selectedText[i + 1] == 'r')
                {
                    i++;
                    continue;
                }
                if (i + 1 < selectedText.Length && selectedText[i] == '|' && selectedText[i + 1] == 'c')
                {
                    if (i + 9 < selectedText.Length)
                    {
                        string strColor = selectedText[i + 2].ToString() + selectedText[i + 3].ToString()// A
                                        + selectedText[i + 4].ToString() + selectedText[i + 5].ToString()// R
                                        + selectedText[i + 6].ToString() + selectedText[i + 7].ToString()// G
                                        + selectedText[i + 8].ToString() + selectedText[i + 9].ToString()// B
                                        ;
                        if (IsColor(strColor) == true)
                        {
                            i += 9;
                            continue;
                        }
                    }
                }
                processLength++;
            }
            if (processLength > 1)
            {
                txtEdit.lockTextChange = true;
                int j = 0;
                for (int i = 0; i < selectedText.Length; i++)
                {
                    int r, g, b;
                    Color tempColor;
                    if (i + 1 < selectedText.Length && selectedText[i] == '|' && selectedText[i + 1] == '|')
                    {
                        r = startColor.R + (endColor.R - startColor.R) * j / (processLength - 1);
                        g = startColor.G + (endColor.G - startColor.G) * j / (processLength - 1);
                        b = startColor.B + (endColor.B - startColor.B) * j / (processLength - 1);
                        tempColor = Color.FromArgb(r, g, b);
                        j++;

                        txtEdit.SelectionLength = 0;
                        txtEdit.SelectedText = "|c" + ColorToHex(tempColor);
                        txtEdit.SelectionStart += 2;
                        txtEdit.SelectedText = "|r";
                        i++;
                        continue;
                    }
                    if (i + 1 < selectedText.Length && selectedText[i] == '|' && selectedText[i + 1] == 'n')
                    {
                        txtEdit.SelectionStart += 2;
                        i++;
                        continue;
                    }
                    if (i + 1 < selectedText.Length && selectedText[i] == '|' && selectedText[i + 1] == 'r')
                    {
                        txtEdit.SelectionStart += 2;
                        i++;
                        continue;
                    }
                    if (i + 1 < selectedText.Length && selectedText[i] == '|' && selectedText[i + 1] == 'c')
                    {
                        if (i + 9 < selectedText.Length)
                        {
                            string strColor = selectedText[i + 2].ToString() + selectedText[i + 3].ToString()// A
                                            + selectedText[i + 4].ToString() + selectedText[i + 5].ToString()// R
                                            + selectedText[i + 6].ToString() + selectedText[i + 7].ToString()// G
                                            + selectedText[i + 8].ToString() + selectedText[i + 9].ToString()// B
                                            ;
                            if (IsColor(strColor) == true)
                            {
                                txtEdit.SelectionStart += 9;
                                i += 9;
                                continue;
                            }
                        }
                    }
                    r = startColor.R + (endColor.R - startColor.R) * j / (processLength - 1);
                    g = startColor.G + (endColor.G - startColor.G) * j / (processLength - 1);
                    b = startColor.B + (endColor.B - startColor.B) * j / (processLength - 1);
                    tempColor = Color.FromArgb(r, g, b);
                    j++;

                    txtEdit.SelectionLength = 0;
                    txtEdit.SelectedText = "|c" + ColorToHex(tempColor);
                    txtEdit.SelectionStart++;
                    txtEdit.SelectedText = "|r";
                }
                txtEdit.lockTextChange = false;
                txtEdit.Select();// 激活控件
            }
            else
            {
                SetTxtEditSingleColor(txtEdit, startColor);
            }

            //int selectLength = txtEdit.SelectionLength;// 需要处理的长度
            //if (selectLength > 1)
            //{
            //    txtEdit.lockTextChange = true;
            //    for (int i = 0; i < selectLength; i++)
            //    {
            //        Color tempColor = Color.FromArgb(startColor.R + (endColor.R - startColor.R) * i / (selectLength - 1), startColor.G + (endColor.G - startColor.G) * i / (selectLength - 1), startColor.B + (endColor.B - startColor.B) * i / (selectLength - 1));// 计算当前字的颜色
            //        txtEdit.SelectionLength = 0;
            //        txtEdit.SelectedText = "|c" + ColorToHex(tempColor);
            //        txtEdit.SelectionStart++;
            //        txtEdit.SelectedText = "|r";
            //        txtEdit.Select();// 激活控件
            //    }
            //    txtEdit.lockTextChange = false;
            //}
            //else
            //{
            //    SetTxtEditSingleColor(txtEdit, startColor);
            //}
        }

        public static void SetTxtPreviewGradual(MyRichTextBox txtPreview, Color startColor, Color endColor)
        {
            int selectLength = txtPreview.SelectionLength;// 需要处理的长度
            if (selectLength > 1)
            {
                txtPreview.lockTextChange = true;
                for (int i = 0; i < selectLength; i++)
                {
                    txtPreview.SelectionLength = 1;
                    Color tempColor = Color.FromArgb(startColor.R + (endColor.R - startColor.R) * i / (selectLength - 1), startColor.G + (endColor.G - startColor.G) * i / (selectLength - 1), startColor.B + (endColor.B - startColor.B) * i / (selectLength - 1));// 计算当前字的颜色
                    txtPreview.SelectionColor = tempColor;
                    txtPreview.SelectionLength = 0;
                    txtPreview.SelectionStart++;
                }
                txtPreview.lockTextChange = false;
                txtPreview.Select();// 激活控件
            }
            else
            {
                SetTxtPreviewSingleColor(txtPreview, startColor);
            }
        }

        /// <summary>
        /// 判定一个字符串是否可以转化为颜色
        /// </summary>
        /// <param name="strColor"></param>
        /// <returns></returns>
        private static bool IsColor(string strColor)
        {
            foreach (var ch in strColor)
            {
                if (((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F')) == false)
                {
                    return false;
                }
            }
            return true;
        }

        /// <summary>
        /// 将字符串转化为颜色
        /// </summary>
        /// <param name="strColor"></param>
        /// <returns></returns>
        private static Color ConvertToColor(string strColor)
        {
            string strA = strColor.Substring(0, 2);// A
            string strR = strColor.Substring(2, 2);// R
            string strG = strColor.Substring(4, 2);// G
            string strB = strColor.Substring(6, 2);// B
            int colorA = Convert.ToInt32(strA, 16);// 将16进制转化为10进制
            int colorR = Convert.ToInt32(strR, 16);
            int colorG = Convert.ToInt32(strG, 16);
            int colorB = Convert.ToInt32(strB, 16);
            return Color.FromArgb(colorR, colorG, colorB);// 好像War3中A用不着
        }

        /// <summary>
        /// 将颜色转化为16进制字符串
        /// </summary>
        /// <param name="color"></param>
        /// <returns></returns>
        public static string ColorToHex(Color color)
        {
            string strA = color.A.ToString("x").PadLeft(2, '0');// 转换为16进制并在前面补充0使长度为2
            string strR = color.R.ToString("x").PadLeft(2, '0');
            string strG = color.G.ToString("x").PadLeft(2, '0');
            string strB = color.B.ToString("x").PadLeft(2, '0');
            return strA + strR + strG + strB;
        }
    }
}