// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_EXCEPTION_H__
#define __V_EXCEPTION_H__

#include <VCommon>
#include <VLog>

// ----------------------------------------------------------------------------
// Define
// ----------------------------------------------------------------------------
#ifdef _MSC_VER
 //
 // You must set compile option (/EHa) to use this TRY EXCEPT mechanism
 //
#define NOMINMAX
#include <winsock2.h>
#include <windows.h>
  int evalException(const int exception, const EXCEPTION_POINTERS* pinfo);
  #ifndef TRY
  #define TRY    __try
  #define EXCEPT __except( evalException(GetExceptionCode(), GetExceptionInformation()) )
  #endif  // TRY
#else // WIN32
  #define TRY try
  #define EXCEPT catch(...)
#endif // _MSC_VER

#endif // __V_EXCEPTION_H__
