// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_LOG_H__
#define __V_LOG_H__

#include <VCommon>
#include <VMetaClass>
#include <VXml>

#ifndef LOG_DEBUG
#define LOG_DEBUG(fmt, ...) { VLog* log = getLog(); if (log != NULL && log->level <= VLog::LEVEL_DEBUG) log->debug("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#define LOG_INFO(fmt, ...)  { VLog* log = getLog(); if (log != NULL && log->level <= VLog::LEVEL_INFO ) log->info ("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#define LOG_WARN(fmt, ...)  { VLog* log = getLog(); if (log != NULL && log->level <= VLog::LEVEL_WARN ) log->warn ("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#define LOG_ERROR(fmt, ...) { VLog* log = getLog(); if (log != NULL && log->level <= VLog::LEVEL_ERROR) log->error("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#define LOG_FATAL(fmt, ...) { VLog* log = getLog(); if (log != NULL && log->level <= VLog::LEVEL_FATAL) log->fatal("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#define LOG_TRACE(fmt, ...) { VLog* log = getLog(); if (log != NULL)                                    log->trace("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#endif // LOG_DEBUG

// gilgil temp 2012.09.18 -----
/*
#define _LOG_DEBUG(fmt, ...) { VLog* log = ::getLog(); if (log != NULL && log->level <= VLog::LEVEL_DEBUG) log->debug("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#define _LOG_INFO(fmt, ...)  { VLog* log = ::getLog(); if (log != NULL && log->level <= VLog::LEVEL_INFO ) log->info ("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#define _LOG_WARN(fmt, ...)  { VLog* log = ::getLog(); if (log != NULL && log->level <= VLog::LEVEL_WARN ) log->warn ("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#define _LOG_ERROR(fmt, ...) { VLog* log = ::getLog(); if (log != NULL && log->level <= VLog::LEVEL_ERROR) log->error("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#define _LOG_FATAL(fmt, ...) { VLog* log = ::getLog(); if (log != NULL && log->level <= VLog::LEVEL_FATAL) log->fatal("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
#define _LOG_TRACE(fmt, ...) { VLog* log = ::getLog(); if (log != NULL)                                    log->trace("[%s:%d] %s "fmt, __FILENAME__, __LINE__, __FUNCNAME__, ##__VA_ARGS__); }
*/
// ----------------------------

#define LOG_DEBUG_MEMORY    LOG_DEBUG("%p %d %p", this, sizeof(*this), (char*)this + sizeof(*this))

#ifdef _DEBUG
#define LOG_ASSERT(expr) if (!(expr)) LOG_FATAL("LOG_ASSERT FAILED(%s)", (#expr))
#else // _DEBUG
#define LOG_ASSERT(expr)
#endif // _DEBUG

// ----------------------------------------------------------------------------
// VShowDateTime
// ----------------------------------------------------------------------------
class VShowDateTime
{
public:
  enum _VShowDateTime
  {
    None,
    Time,
    DateTime
  };

protected:
  _VShowDateTime value;

public:
  VShowDateTime()                           {                      } // default ctor
  VShowDateTime(const _VShowDateTime value) { this->value = value; } // conversion ctor
  operator _VShowDateTime() const           { return value;        } // cast operator

public:
  VShowDateTime(const QString s);
  QString str() const;
};

class VLog :
  public VXmlable
  // public VClassInitialize<VLog> // gilgil temp 2012.11.01
{
  friend class VLogList;
  friend class VLogFactory;

public:
  static const int LEVEL_DEBUG = 0;
  static const int LEVEL_INFO  = 1;
  static const int LEVEL_WARN  = 2;
  static const int LEVEL_ERROR = 3;
  static const int LEVEL_FATAL = 4;
  static const int LEVEL_NONE  = 5;

public:
  static const int DEFAULT_LOG_BUF_SIZE = 65536;

  //
  // Option
  //
public:
  int           level;
  VShowDateTime showDateTime;
  bool          showThreadID;

  //
  // Constructor and destructor
  //
public:
  VLog();
  virtual ~VLog();

  //
  // Used internally
  //
protected:
  int makeBuf(const char* buf, int len, const char* fmt, va_list args); // dateTime, threadID, args

  //
  // VLog function
  //
public:
  virtual void debug(const char* fmt, ...);
  virtual void info (const char* fmt, ...);
  virtual void warn (const char* fmt, ...);
  virtual void error(const char* fmt, ...);
  virtual void fatal(const char* fmt, ...);
  virtual void trace(const char* fmt, ...);

protected:
  virtual void doDebug(const char *fmt, va_list args);
  virtual void doInfo (const char *fmt, va_list args);
  virtual void doWarn (const char *fmt, va_list args);
  virtual void doError(const char *fmt, va_list args);
  virtual void doFatal(const char *fmt, va_list args);
  virtual void doTrace(const char *fmt, va_list args);

  //
  // Virtual function
  //
protected:
  virtual bool open();
  virtual bool close();

protected:
  virtual void write(const char* buf, int len) = 0; // gilgil temp 2012.11.16
  // virtual void write(const char* buf, int len){ Q_UNUSED(buf); Q_UNUSED(len); }

public:
  virtual VLog* createByURI(const QString& uri);

  //
  // XML
  //
public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

  //
  // Global
  //
protected:
  static VLog* g_log;
public:
  static VLog* getLog();             //{                                                         return g_log;  } // gilgil temp 2012.11.01
  static VLog* setLog(VLog* log);    //{ VLog* oldLog = g_log; g_log = log;                      return oldLog; } // gilgil temp 2012.11.01
  static VLog* changeLog(VLog* log); //{ VLog* oldLog = g_log; g_log = log; SAFE_DELETE(oldLog); return oldLog; } // gilgil temp 2012.11.01

private:
  // friend class VClassInitialize<VLog>; // gilgil temp 2012.11.01
  // static void doClassInitialize(); // gilgil temp 2012.11.01
  // static void doClassFinalize(); // gilgil temp 2012.11.01
  friend class VLogInstance;
};

// ----------------------------------------------------------------------------
// Global Function
// ----------------------------------------------------------------------------
inline VLog* getLog()             { return VLog::getLog();       }
inline VLog* setLog(VLog* log)    { return VLog::setLog(log);    }
inline VLog* changeLog(VLog* log) { return VLog::changeLog(log); }

// gilgil temp 2012.09.18 -----
/*
// ----------------------------------------------------------------------------
// IVGetLoggable
// ----------------------------------------------------------------------------
class IVGetLoggable
{
public:
  virtual VLog* getLog()             = 0;
  virtual VLog* setLog(VLog* log)    = 0;
  virtual VLog* changeLog(VLog* log) = 0;
};

// ----------------------------------------------------------------------------
// VGetLoggable
// ----------------------------------------------------------------------------
class VGetLoggable : public IVGetLoggable
{
private:
  VLog* m_log;
public:
  VGetLoggable()                     { m_log = VLog::getLog();                                                }
  virtual ~VGetLoggable()            {  do not delete m_log                                                   }
  virtual VLog* getLog()             {                                                         return m_log;  }
  virtual VLog* setLog(VLog* log)    { VLog* oldLog = m_log; m_log = log;                      return oldLog; }
  virtual VLog* changeLog(VLog* log) { VLog* oldLog = m_log; m_log = log; SAFE_DELETE(oldLog); return oldLog; }
};
*/
// ----------------------------

#endif // __V_LOG_H__
