#ifndef YDJAPI_H_INCLUDED
#define YDJAPI_H_INCLUDED

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#if !defined(_MSC_VER) || _MSC_VER >= 1600
#include <stdint.h>
#else
typedef int int32_t;
#endif
#include <windows.h>

/** \brief 初始化JAPI模块
 *
 * 初始化JAPI模块。会查找game.dll里面JAPI相关的一些函数并且做hook。
 *
 * \param gameDllModule - game.dll的HMODULE（基地址）
 * \return 成功返回TRUE, 失败返回FALSE
 */
BOOL InitJAPIFramework(HMODULE gameDllModule);

/** \brief 注册 注册JAPI的Callback
 *
 * 比较诡异的函数……但是必须这样做。这个函数用来向JAPI系统提供一个回调函数，当魔兽初始化common.j的时候，这个回
 * 调函数被调用。你应该在你的这个回调函数中使用<code>RegisterJAPINative</code>注册JAPI函数
 *
 * \param pfCallback - 回调函数
 * \see RegisterJAPINative
 */
void RegisterJAPICallback(void (*pfCallback)());

/** \brief 注册JAPI函数
 *
 * 这个函数用来注册JAPI函数。
 *
 * \param pfNativeFunction - 函数地址
 * \param nativeName - 函数名。Jass中以这个名称调用函数
 * \param nativeSignature - 函数签名
 * \see RegisterJAPICallback
 */
void RegisterJAPINative(void *pfNativeFunction, const char *nativeName, const char *nativeSignature);

/** \brief 从字符串对象中解出C字符串
 *
 * Jass的字符串传到JAPI函数中会变成一个指针，指向某字符串表中的对象。这个函数从字符串对象中把C风格字符串解出来供使用。
 *
 * \param stringObject - 字符串对象，实际上是指针
 * \return 解出的C字符串
 *
 * \note 字符串是UTF-8编码的
 */
const char *GetStringFromNativeParameter(int32_t stringObject);

/** \brief 把C字符串映射为魔兽内部的对象
 *
 * 这个函数和上面一个的作用相反：把C字符串映射为魔兽内部对象，这样可以用于向Jass返回string
 *
 * \param string - 需要返回和映射的C字符串
 * \return 映射的内部对象指针
 *
 * \note 字符串是UTF-8编码的
 */
int32_t MapStringToInternalObject(const char *string);

/// 跟踪级别
#define YDJAPI_LOG_TRACE 0
/// 调试级别
#define YDJAPI_LOG_DEBUG 1
/// 信息级别
#define YDJAPI_LOG_INFO 2
/// 警告级别（出现了小错误但是不影响功能）
#define YDJAPI_LOG_WARNING 3
/// 错误级别（出现了影响功能的错误）
#define YDJAPI_LOG_ERROR 4
/// 严重级别（检测出致命错误，导致系统无法运行）
#define YDJAPI_LOG_FATAL 5

/** \brief 日志函数
 *
 * 输出日志，日志单行最大长度8191字节
 *
 * \param level - 日志等级
 * \param format - 格式参数
 * \return 输出字节数
 */
int JAPILog(int level, const char *format, ...);

/** \brief 安装inline钩子
 *
 * 安装inline钩子。这个函数会自动构建trampoline并修改原来的指针
 *
 * \param [inout] ppOriginalFunction - （指向的指针）调用之前表示被hook的函数，调用之后会修改为trampoline地址
 * \param pSubstitutionFunction - 替代函数
 *
 * \return TRUE 表示成功，FALSE表示失败
 */
BOOL InstallInlineHook(void **ppOriginalFunction, void *pSubstitutionFunction);

/** \brief 卸载inline钩子
 *
 * 卸载inline钩子。
 *
 * \param [inout] ppOriginalFunction - （指向的指针）调用之前表示trampoline地址，调用之后会修改为被hook的函数
 * \param pSubstitutionFunction - 替代函数
 *
 * \return TRUE 表示成功，FALSE表示失败
 */
BOOL UninstallInlineHook(void **ppOriginalFunction, void *pSubstitutionFunction);

#endif // YDJAPI_H_INCLUDED
