// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_STRING_H__
#define __V_STRING_H__

#include <string>
#include <QString>

// ----------------------------------------------------------------------------
// String Function
// ----------------------------------------------------------------------------
std::string  format       (const char* fmt, ...);
std::wstring wformat      (const char* fmt, ...);
QString      qformat      (const char* fmt, ...);

size_t       char_to_wchar(wchar_t*       ws, size_t ws_len, const char*        s, size_t  s_len = 0);
std::wstring char_to_wcs  (const char*    s,  size_t s_len = 0);
size_t       mbs_to_wchar (wchar_t*       ws, size_t ws_len, const std::string  s);
std::wstring mbs_to_wcs   (const std::string s);

size_t       wchar_to_char(char*          s,  size_t s_len,  const wchar_t* ws, size_t ws_len = 0);
std::string  wchar_to_mbs (const wchar_t* ws, size_t ws_len = 0);
size_t       wcs_to_char  (char*          s,  size_t s_len,  const std::wstring ws);
std::string  wcs_to_mbs   (const std::wstring  ws);

#endif // __V_STRING_H__
