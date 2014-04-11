// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_COMMON_H__
#define __V_COMMON_H__

#define WIN32_LEAN_AND_MEAN
#include <QtCore>
#include <VDesign>
#include <VSecureLib>
#include <VString>
#include <VSystem>
#ifdef WIN32
#include <VWindows>
#endif // WIN32
#ifdef linux
#include <VLinux>
#endif // linux
#include <VLinkLibrary>

// ----------------------------------------------------------------------------
// VDream Version
// ----------------------------------------------------------------------------
extern const char* VDREAM_VERSION;

// ----------------------------------------------------------------------------
// __FILENAME__ and __FUNCNANE__ macros
// ----------------------------------------------------------------------------
const char* __extractFileName__(const char* fileName);
const char* __extractFuncName__(const char* funcName);
#define __FILENAME__ __extractFileName__(__FILE__)
#define __FUNCNAME__ __extractFuncName__(__FUNCTION__)

// ----------------------------------------------------------------------------
// Common Define
// ----------------------------------------------------------------------------
#ifndef SAFE_DELETE
#define SAFE_DELETE(A) { if(A != NULL) { delete A; A = NULL; } }
#endif // SAFE_DELETE

#ifndef SAFE_FREE
#define SAFE_FREE(A)   { if(A != NULL) { free(A); A = NULL; } }
#endif // SAFE_FREE

#ifndef CRLF
#define CRLF "\r\n"
#endif // CRLF

#ifndef ASSERT
#ifdef _DEBUG
  #define ASSERT(a) LOG_ASSERT(a)
#else
  #define ASSERT(a)
#endif // _DEBUG
#endif // ASSERT

// ----------------------------------------------------------------------------
// Class
// ----------------------------------------------------------------------------
QString getClassName(const char* value);
#define IS_CLASS(obj, type) (dynamic_cast<type>(obj)!=NULL)
#define CLASS_NAME(obj)     getClassName(typeid(obj).name())

// ----------------------------------------------------------------------------
// Constant
// ----------------------------------------------------------------------------
namespace vd
{
  static const int      DEFAULT_BUF_SIZE          = 65536;  // bytes
  static const VTimeout DEFAULT_TIMEOUT           = 10000; // 10 sec
  static const int      DEFAULT_PORT              = 10065;
  static const int      DEFAULT_MEDIAGATEWAY_PORT = 10066;
};

// ----------------------------------------------------------------------------
// VState
// ----------------------------------------------------------------------------
class VState
{
public:
  enum _VState
  {
    None,    // 0
    Closed,  // 1
    Opening, // 2
    Opened,  // 3
    Closing  // 4
  };

protected:
  _VState value;

public:
  VState()                    {                      } // default ctor
  VState(const _VState value) { this->value = value; } // conversion ctor
  operator _VState() const    { return value;        } // cast operator

public:
  VState(const QString s);
  QString str() const;
};

// ----------------------------------------------------------------------------
// VMode
// ----------------------------------------------------------------------------
class VMode
{
  enum _VMode
  {
    None,     // 0
    Read,     // 1
    Write,    // 2
    ReadWrite // 3
  };

protected:
  _VMode value;

public:
  VMode()                   {                      } // default ctor
  VMode(const _VMode value) { this->value = value; } // conversion ctor
  operator _VMode() const   { return value;        } // cast operator

public:
  VMode(const QString s);
  QString str() const;
};

#endif // __V_COMMON_H__
