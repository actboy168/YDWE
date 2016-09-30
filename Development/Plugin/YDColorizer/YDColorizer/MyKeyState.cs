using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace YDColorizer
{
    class MyKeyState
    {
        /// <summary>
        /// 获取按键状态
        /// </summary>
        /// <param name="nVirtKey">按键的键值</param>
        /// <returns>小于0，按下；大于0，没按下</returns>
        [DllImport("user32.dll", EntryPoint = "GetKeyState")]
        private static extern short GetKeyState(int nVirtKey);

        /// <summary>
        /// 按键是否按下
        /// </summary>
        /// <param name="keyValue"></param>
        /// <returns></returns>
        public static bool IsKeyDown(int keyValue)
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
    }
}