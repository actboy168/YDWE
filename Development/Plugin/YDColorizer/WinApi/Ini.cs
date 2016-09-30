using System.Text;
using System.Runtime.InteropServices;

namespace WinApi
{
    public class Ini
    {
        [DllImport("kernel32.dll", EntryPoint = "GetPrivateProfileInt", SetLastError = true)]
        private static extern int GetPrivateProfileInt(string lpApplicationName, string lpKeyName, int nDefault, string lpFileName);
        [DllImport("kernel32.dll", EntryPoint = "GetPrivateProfileString", SetLastError = true)]
        private static extern bool GetPrivateProfileString(string lpApplicationName, string lpKeyName, string lpDefault, StringBuilder lpReturnedString, int nSize, string lpFileName);
        [DllImport("kernel32.dll", EntryPoint = "WritePrivateProfileString", SetLastError = true)]
        private static extern bool WritePrivateProfileString(string lpApplicationName, string lpKeyName, string lpString, string lpFileName);
        [DllImport("kernel32.dll", EntryPoint = "GetPrivateProfileSection", SetLastError = true)]
        private static extern bool GetPrivateProfileSection(string lpAppName, StringBuilder lpReturnedString, int nSize, string lpFileName);
        [DllImport("kernel32.dll", EntryPoint = "WritePrivateProfileSection", SetLastError = true)]
        private static extern bool WritePrivateProfileSection(string lpAppName, string lpString, string lpFileName);

        /// <summary>
        /// 获取ini文件中的数值
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="section">节名</param>
        /// <param name="key">值名</param>
        /// <returns>数值</returns>
        public static int GetInt(string fileName, string section, string key)
        {
            return GetPrivateProfileInt(section, key, 0, fileName);
        }

        /// <summary>
        /// 获取ini文件中的字符串值
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="section">节名</param>
        /// <param name="key">值名</param>
        /// <returns>字符串值</returns>
        public static string GetString(string fileName, string section, string key)
        {
            StringBuilder sb = new StringBuilder(1024);
            GetPrivateProfileString(section, key, string.Empty, sb, sb.Capacity, fileName);
            return sb.ToString();
        }

        /// <summary>
        /// 设置ini文件中的数值
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="section">节名</param>
        /// <param name="key">值名</param>
        /// <param name="value">数值</param>
        public static void SetInt(string fileName, string section, string key, int value)
        {
            WritePrivateProfileString(section, key, value.ToString(), fileName);
        }

        /// <summary>
        /// 设置ini文件中的字符串值
        /// </summary>
        /// <param name="fileName">文件路径</param>
        /// <param name="section">节名</param>
        /// <param name="key">值名</param>
        /// <param name="value">字符串值</param>
        public static void SetString(string fileName, string section, string key, string value)
        {
            WritePrivateProfileString(section, key, value, fileName);
        }
    }
}
