// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_SECURE_LIB_H__
#define __V_SECURE_LIB_H__

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef __STDC_WANT_SECURE_LIB__

#define vsprintf_s(buf, size, fmt, ...)  vsprintf(buf, fmt, ##__VA_ARGS__)
#define strcpy_s(dst, size, src)         strcpy(dst, src)
#define scanf_s(fmt, size, ...)          scanf(fmt, ##__VA_ARGS__)
#define memmove_s(dst, size, src, count) memmove(dst, src, count)
#define gets_s(buf, size)                gets(buf)

#define printf_s  printf
#define fprintf_s fprintf
#define sprintf_s snprintf
#define _strdup   strdup

#ifndef errno_t
typedef int errno_t;
#endif // errno_t

errno_t fopen_s(FILE** fileHandle, const char* fileName, const char *mode);
errno_t wcstombs_s(size_t* size, char*    dst, size_t dstSize, const wchar_t* src, size_t maxCount);
errno_t mbstowcs_s(size_t* size, wchar_t* dst, size_t dstSize, const char *   src, size_t maxCount);

#endif // __STDC_WANT_SECURE_LIB__

#endif // __V_SECURE_LIB_H__
