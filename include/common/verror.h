// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_ERROR_H__
#define __V_ERROR_H__

#include <VCommon>

// ----------------------------------------------------------------------------
// SET_ERROR
// ----------------------------------------------------------------------------
#define SET_ERROR(CLASS, MSG, CODE)                                               \
if (error.code == VERR_OK)                                                        \
{                                                                                 \
  setError<CLASS>(error, MSG, CODE);                                              \
  LOG_ERROR("%s type=%s code=%u(0x%X)",                                           \
  error.msg, qPrintable(getClassName(error.ti->name())), error.code, error.code); \
}

// ----------------------------------------------------------------------------
// SET_DEBUG_ERROR
// ----------------------------------------------------------------------------
#define SET_DEBUG_ERROR(CLASS, MSG, CODE)                                         \
if (error.code == VERR_OK)                                                        \
{                                                                                 \
  setError<CLASS>(error, MSG, CODE);                                              \
  LOG_DEBUG("%s type=%s code=%u(0x%X)",                                           \
  error.msg, qPrintable(getClassName(error.ti->name())), error.code, error.code); \
}

// ----------------------------------------------------------------------------
// Error Category
// ----------------------------------------------------------------------------
static const int  VERR_CATEGORY_COMMON      = 0;
static const int    VERR_CATEGORY_BOOST     = 1000;
static const int    VERR_CATEGORY_THREAD    = 2000;
static const int    VERR_CATEGORY_XML       = 3000;
static const int    VERR_CATEGORY_BUFFER    = 4000;
static const int    VERR_CATEGORY_FILE      = 5000;
static const int    VERR_CATEGORY_INTERFACE = 6000;
static const int    VERR_CATEGORY_REGEX     = 7000;

static const int  VERR_CATEGORY_NET         = 10000;
static const int   VERR_CATEGORY_SNOOP      = 11000;
static const int   VERR_CATEGORY_SSL        = 12000;
static const int   VERR_CATEGORY_VPN_DETECT = 13000;

static const int  VERR_CATEGORY_VIDEO       = 20000;

static const int  VERR_CATEGORY_AUDIO       = 30000;

static const int  VERR_CATEGORY_OTHER       = 40000;

// ----------------------------------------------------------------------------
// Error Code
// ----------------------------------------------------------------------------
static const int VERR_OK                    = 0;
static const int VERR_FAIL                  = -1;
static const int VERR_UNKNOWN               = 1;

static const int VERR_NOT_SUPPORTED         = VERR_CATEGORY_COMMON + 2;
static const int VERR_NOT_CLOSED_STATE      = VERR_CATEGORY_COMMON + 3;
static const int VERR_NOT_OPENED_STATE      = VERR_CATEGORY_COMMON + 4;
static const int VERR_HANDLE_IS_ZERO        = VERR_CATEGORY_COMMON + 5;
static const int VERR_LENGTH_IS_ZERO        = VERR_CATEGORY_COMMON + 6;
static const int VERR_VALUE_IS_ZERO         = VERR_CATEGORY_COMMON + 7;
static const int VERR_POINTER_IS_NULL       = VERR_CATEGORY_COMMON + 8;
static const int VERR_OBJECT_IS_NULL        = VERR_CATEGORY_COMMON + 9;
static const int VERR_OBJECT_IS_NOT_NULL    = VERR_CATEGORY_COMMON + 10;
static const int VERR_NOT_SPECIFIED         = VERR_CATEGORY_COMMON + 11;
static const int VERR_INVALID_INDEX         = VERR_CATEGORY_COMMON + 12;
static const int VERR_INVALID_HANDLE        = VERR_CATEGORY_COMMON + 13;
static const int VERR_TIMEOUT               = VERR_CATEGORY_COMMON + 14;
static const int VERR_NOT_READABLE          = VERR_CATEGORY_COMMON + 15;
static const int VERR_NOT_WRITABLE          = VERR_CATEGORY_COMMON + 16;
static const int VERR_ERROR_IN_MEMORY       = VERR_CATEGORY_COMMON + 17;
static const int VERR_NOT_ENOUGH_MEMORY     = VERR_CATEGORY_COMMON + 18;
static const int VERR_VIRTUAL_FUNCTION_CALL = VERR_CATEGORY_COMMON + 19;
static const int VERR_QCONNECT_FAIL         = VERR_CATEGORY_COMMON + 20;
static const int VERR_RUN_PROCESS           = VERR_CATEGORY_COMMON + 21;
static const int VERR_LOAD_FAIL             = VERR_CATEGORY_COMMON + 22;

// ----------------------------------------------------------------------------
// VError
// ----------------------------------------------------------------------------
class VError
{
  static const int DEFAULT_ERR_BUF_SIZE = 512;

public:
  std::type_info* ti;
  char            msg[DEFAULT_ERR_BUF_SIZE];
  int             code;
  QString         className();

public:
  VError(const char* msg = NULL, const int code = VERR_OK);
  VError& operator = (const VError& error);

public:
  void clear();
  void setErrorMsg(const char* msg);
  void setErrorCode(int code);
};

// ----------------------------------------------------------------------------
// VDECLARE_ERROR_CLASS
// ----------------------------------------------------------------------------
#define VDECLARE_ERROR_CLASS(ERROR_CLASS_NAME)                       \
class ERROR_CLASS_NAME : public VError                               \
{                                                                    \
public:                                                              \
  ERROR_CLASS_NAME(const char* msg = NULL, const int code = VERR_OK) \
    : VError(msg, code)                                              \
  {                                                                  \
    this->ti = (std::type_info*)&typeid(*this);                      \
  }                                                                  \
};

VDECLARE_ERROR_CLASS(VStdError)

// ----------------------------------------------------------------------------
// VCATCH
// ----------------------------------------------------------------------------
#define VCATCH                                          \
catch (std::exception& e)                               \
{                                                       \
  SET_ERROR(VStdError, e.what(), GetLastError());       \
}                                                       \
catch (...)                                             \
{                                                       \
  SET_ERROR(VError, "exceptin occurred", VERR_UNKNOWN); \
}

// ----------------------------------------------------------------------------
// setError
// ----------------------------------------------------------------------------
template <class T>
void setError(VError& error, const char* msg = NULL, const int code = VERR_UNKNOWN)
{
  error.ti = (std::type_info*)&typeid(T);
  error.setErrorMsg(msg);
  error.setErrorCode(code);
}

template <class T>
void setError(VError& error, const QString& msg, const int code = VERR_UNKNOWN)
{
  error.ti = (std::type_info*)&typeid(T);
  error.setErrorMsg(qPrintable(msg));
  error.setErrorCode(code);
}

#endif // __V_ERROR_H__
