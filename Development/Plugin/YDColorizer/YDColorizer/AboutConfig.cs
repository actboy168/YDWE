using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Reflection;
using System.Runtime.InteropServices;
using System.IO;
using System.Windows.Forms;
using System.Text.RegularExpressions;

namespace YDColorizer
{
    class AboutConfig
    {
        private static string ReadLocalFile(string lpszFileName)
        {
            try
            {
                return File.ReadAllText(Config.GetThisMoudlePath() + "\\" + lpszFileName, Encoding.UTF8);
            }
            catch (Exception)
            {
                return null;

            }
        }

        private static string ReadMpqFile(string lpszFileName)
        {
            IntPtr ptr_buf_ = IntPtr.Zero;
            int size_ = 0;
            SFileLoadFile(lpszFileName, out ptr_buf_, out size_, 0, 0);
            if (size_ == 0)
            {
                return null;
            }

            byte[] buf_ = new byte[size_];
            Marshal.Copy(ptr_buf_, buf_, 0, size_);
            SFileUnloadFile(ptr_buf_);
            return Encoding.UTF8.GetString(buf_);
        }

        private static string Format(string format_, string arg_)
        {
            return Regex.Replace(format_, @"(\%s)", arg_, RegexOptions.Compiled);
        }

        /// <summary>
        /// 加载搜索的标题
        /// </summary>
        /// <returns></returns>
        public static List<string> LoadSearchTitles()
        {
            List<string> dialogBoxTitles = new List<string>();
            string text_ = ReadLocalFile("title.txt");
            
            if (text_ != null)
            {                
                var arr = text_.Split(Environment.NewLine.ToCharArray());

                foreach (var str in arr)
                {
                    dialogBoxTitles.Add(str);
                }
            }

            return dialogBoxTitles;
        }

        [DllImport("Storm.dll", EntryPoint = "#279", CharSet = CharSet.Ansi, SetLastError = true)]
        private static extern int SFileLoadFile(string lpszFileName, out IntPtr pBuf, out int nSize, int nUnknownA, int nUnknownB);
        [DllImport("Storm.dll", EntryPoint = "#280", CharSet = CharSet.Ansi, SetLastError = true)]
        private static extern int SFileUnloadFile(IntPtr pBuf);
    }
}