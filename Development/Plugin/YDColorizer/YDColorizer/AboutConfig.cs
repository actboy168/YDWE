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
    }
}