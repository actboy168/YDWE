using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace WinApi
{
    public class KeyBoard
    {
        [DllImport("user32.dll", EntryPoint = "GetKeyState", SetLastError = true)]
        private static extern int GetKeyState(int nVirtKey);

        /// <summary>
        /// 键盘上的某一个键是否被按下
        /// </summary>
        /// <param name="keyValue">键值</param>
        /// <returns></returns>
        public static bool IsDown(int keyValue)
        {
            if (GetKeyState(keyValue) < 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        /// <summary>
        /// 键盘上的某一个键是否被按下
        /// </summary>
        /// <param name="key">键</param>
        /// <returns></returns>
        public static bool IsDown(Keys key)
        {
            return IsDown((int)key);
        }

        /// <summary>
        /// A键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsADown()
        {
            return IsDown(65);
        }

        /// <summary>
        /// B键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsBDown()
        {
            return IsDown(66);
        }

        /// <summary>
        /// C键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsCDown()
        {
            return IsDown(67);
        }

        /// <summary>
        /// D键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsDDown()
        {
            return IsDown(68);
        }

        /// <summary>
        /// E键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsEDown()
        {
            return IsDown(69);
        }

        /// <summary>
        /// F键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsFDown()
        {
            return IsDown(70);
        }

        /// <summary>
        /// G键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsGDown()
        {
            return IsDown(71);
        }

        /// <summary>
        /// H键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsHDown()
        {
            return IsDown(72);
        }

        /// <summary>
        /// I键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsIDown()
        {
            return IsDown(73);
        }

        /// <summary>
        /// J键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsJDown()
        {
            return IsDown(74);
        }

        /// <summary>
        /// K键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsKDown()
        {
            return IsDown(75);
        }

        /// <summary>
        /// L键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsLDown()
        {
            return IsDown(76);
        }

        /// <summary>
        /// M键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsMDown()
        {
            return IsDown(77);
        }

        /// <summary>
        /// N键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNDown()
        {
            return IsDown(78);
        }

        /// <summary>
        /// O键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsODown()
        {
            return IsDown(79);
        }

        /// <summary>
        /// P键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsPDown()
        {
            return IsDown(80);
        }

        /// <summary>
        /// Q键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsQDown()
        {
            return IsDown(81);
        }

        /// <summary>
        /// R键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsRDown()
        {
            return IsDown(82);
        }

        /// <summary>
        /// S键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsSDown()
        {
            return IsDown(83);
        }

        /// <summary>
        /// T键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsTDown()
        {
            return IsDown(84);
        }

        /// <summary>
        /// U键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsUDown()
        {
            return IsDown(85);
        }

        /// <summary>
        /// V键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsVDown()
        {
            return IsDown(86);
        }

        /// <summary>
        /// W键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsWDown()
        {
            return IsDown(87);
        }

        /// <summary>
        /// X键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsXDown()
        {
            return IsDown(88);
        }

        /// <summary>
        /// Y键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsYDown()
        {
            return IsDown(89);
        }

        /// <summary>
        /// Z键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsZDown()
        {
            return IsDown(90);
        }

        /// <summary>
        /// 向左键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsLeftDown()
        {
            return IsDown(37);
        }

        /// <summary>
        /// 向上键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsUpDown()
        {
            return IsDown(38);
        }

        /// <summary>
        /// 向右键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsRightDown()
        {
            return IsDown(39);
        }

        /// <summary>
        /// 向下键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsDownDown()
        {

            return IsDown(40);
        }

        /// <summary>
        /// 0键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsD0Down()
        {
            return IsDown(48);
        }

        /// <summary>
        /// 1键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsD1Down()
        {
            return IsDown(49);
        }

        /// <summary>
        /// 2键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsD2Down()
        {
            return IsDown(50);
        }

        /// <summary>
        /// 3键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsD3Down()
        {
            return IsDown(51);
        }

        /// <summary>
        /// 4键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsD4Down()
        {
            return IsDown(52);
        }

        /// <summary>
        /// 5键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsD5Down()
        {
            return IsDown(53);
        }

        /// <summary>
        /// 6键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsD6Down()
        {
            return IsDown(54);
        }

        /// <summary>
        /// 7键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsD7Down()
        {
            return IsDown(55);
        }

        /// <summary>
        /// 8键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsD8Down()
        {
            return IsDown(56);
        }

        /// <summary>
        /// 9键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsD9Down()
        {
            return IsDown(57);
        }

        /// <summary>
        /// 小键盘0键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNumPad0Down()
        {
            return IsDown(96);
        }

        /// <summary>
        /// 小键盘1键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNumPad1Down()
        {
            return IsDown(97);
        }

        /// <summary>
        /// 小键盘2键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNumPad2Down()
        {
            return IsDown(98);
        }

        /// <summary>
        /// 小键盘3键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNumPad3Down()
        {
            return IsDown(99);
        }

        /// <summary>
        /// 小键盘4键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNumPad4Down()
        {
            return IsDown(100);
        }

        /// <summary>
        /// 小键盘5键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNumPad5Down()
        {
            return IsDown(101);
        }

        /// <summary>
        /// 小键盘6键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNumPad6Down()
        {
            return IsDown(102);
        }

        /// <summary>
        /// 小键盘7键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNumPad7Down()
        {
            return IsDown(103);
        }

        /// <summary>
        /// 小键盘8键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNumPad8Down()
        {
            return IsDown(104);
        }

        /// <summary>
        /// 小键盘9键是否被按下
        /// </summary>
        /// <returns></returns>
        public static bool IsNumPad9Down()
        {
            return IsDown(105);
        }
    }
}
