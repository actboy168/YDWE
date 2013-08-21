/*
 * WE native functions
 */
#ifndef YDWE_EDITOR_NATIVE_FUNCTION_H_INCLUDED
#define YDWE_EDITOR_NATIVE_FUNCTION_H_INCLUDED

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include <aero/aero.hpp>

namespace NYDWE
{

/** \brief Translate string according to settings in WorldEditStrings.txt
 *
 * Translate a string according to WorldEditStrings.txt.
 * 
 * \param templateString String to translate, e.g. WESTRING_MODIFYTEXTURES
 * \param buffer Buffer to receive results
 * \param bufferSize Size of the buffer
 * \param silent If false, WE will show a text in case of error
 * \return aero::nbool_t TRUE if success, FALSE if fail
 */
aero::nbool_t WeTemplateStringTranslate(const char *templateString, char *buffer, size_t bufferSize, int silent);

/** \brief Show message in WE
 *
 * Show message in WE main window?
 *
 * \param message Message to show
 * \param flag Some flag, may be 1?
 */
void WeMessageShow(const char *message, int flag);

/** \brief Intialize natives
 *
 * Initialize natives
 */
void NativeInit();

}

#endif // YDWE_EDITOR_NATIVE_FUNCTION_H_INCLUDED
