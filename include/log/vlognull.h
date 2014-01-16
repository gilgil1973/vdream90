// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_LOG_NULL_H__
#define __V_LOG_NULL_H__

#include <VLog>

// ----------------------------------------------------------------------------
// VLogNull
// ----------------------------------------------------------------------------
class VLogNull : public VLog
{
public:
  VLogNull()
  {
    level        = LEVEL_NONE;
    showDateTime = VShowDateTime::None;
    showThreadID = false;
  }
  virtual ~VLogNull() {}

public:
  virtual void debug(const char* fmt, ...) { Q_UNUSED(fmt) }
  virtual void info (const char* fmt, ...) { Q_UNUSED(fmt) }
  virtual void warn (const char* fmt, ...) { Q_UNUSED(fmt) }
  virtual void error(const char* fmt, ...) { Q_UNUSED(fmt) }
  virtual void fatal(const char* fmt, ...) { Q_UNUSED(fmt) }
  virtual void trace(const char* fmt, ...) { Q_UNUSED(fmt) }

protected:
  virtual void write(const char* buf, int len)     { Q_UNUSED(buf) Q_UNUSED(len) }

public:
  virtual VLog* createByURI(const QString& uri);
};

#endif // __V_LOG_NULL_H__
