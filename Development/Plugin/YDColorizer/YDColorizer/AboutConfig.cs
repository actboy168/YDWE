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
            // TODO: 和ydwe的加载方式保持一致
            string mpqdir = Config.GetThisMoudlePath() + "\\..\\..\\share\\mpq\\ydwe\\";
            try
            {
                return File.ReadAllText(mpqdir + lpszFileName, Encoding.UTF8);
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
            //string text_ = ReadMpqFile("UI\\WorldEditStrings.txt");
            string text_ = ReadLocalFile("UI\\WorldEditStrings.txt");
            
            if (text_ != null)
            {
                // WESTRING_COD_TYPE_STRING=字符串
                // WESTRING_UE_DLG_EDITVALUE=编辑单位值 - %s
                // WESTRING_IE_DLG_EDITVALUE=编辑物品值 - %s
                // WESTRING_AE_DLG_EDITVALUE=编辑技能值 - %s
                // WESTRING_FE_DLG_EDITVALUE=编辑效果值 - %s
                // WESTRING_BE_DLG_EDITVALUE=编辑可破坏物值 - %s
                // WESTRING_DE_DLG_EDITVALUE=编辑装饰物值 - %s
                // WESTRING_GE_DLG_EDITVALUE=编辑科技值 - %s

                Ini.Section s = (new Ini(text_)).GetSection("WorldEditStrings");
                string WESTRING_COD_TYPE_STRING = s.GetKeyString("WESTRING_COD_TYPE_STRING");
                string WESTRING_UE_DLG_EDITVALUE = s.GetKeyString("WESTRING_UE_DLG_EDITVALUE");
                string WESTRING_IE_DLG_EDITVALUE = s.GetKeyString("WESTRING_IE_DLG_EDITVALUE");
                string WESTRING_AE_DLG_EDITVALUE = s.GetKeyString("WESTRING_AE_DLG_EDITVALUE");
                string WESTRING_FE_DLG_EDITVALUE = s.GetKeyString("WESTRING_FE_DLG_EDITVALUE");
                string WESTRING_BE_DLG_EDITVALUE = s.GetKeyString("WESTRING_BE_DLG_EDITVALUE");
                string WESTRING_DE_DLG_EDITVALUE = s.GetKeyString("WESTRING_DE_DLG_EDITVALUE");
                string WESTRING_GE_DLG_EDITVALUE = s.GetKeyString("WESTRING_GE_DLG_EDITVALUE");

                dialogBoxTitles.Add(Format(WESTRING_UE_DLG_EDITVALUE, WESTRING_COD_TYPE_STRING));
                dialogBoxTitles.Add(Format(WESTRING_IE_DLG_EDITVALUE, WESTRING_COD_TYPE_STRING));
                dialogBoxTitles.Add(Format(WESTRING_AE_DLG_EDITVALUE, WESTRING_COD_TYPE_STRING));
                dialogBoxTitles.Add(Format(WESTRING_FE_DLG_EDITVALUE, WESTRING_COD_TYPE_STRING));
                dialogBoxTitles.Add(Format(WESTRING_BE_DLG_EDITVALUE, WESTRING_COD_TYPE_STRING));
                dialogBoxTitles.Add(Format(WESTRING_DE_DLG_EDITVALUE, WESTRING_COD_TYPE_STRING));
                dialogBoxTitles.Add(Format(WESTRING_GE_DLG_EDITVALUE, WESTRING_COD_TYPE_STRING));
            }

            return dialogBoxTitles;
        }

        [DllImport("Storm.dll", EntryPoint = "#279", CharSet = CharSet.Ansi, SetLastError = true)]
        private static extern int SFileLoadFile(string lpszFileName, out IntPtr pBuf, out int nSize, int nUnknownA, int nUnknownB);
        [DllImport("Storm.dll", EntryPoint = "#280", CharSet = CharSet.Ansi, SetLastError = true)]
        private static extern int SFileUnloadFile(IntPtr pBuf);
    }
}