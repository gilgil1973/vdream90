// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_LOG_STDOUT_H__
#define __V_LOG_STDOUT_H__

#include <VLog>

// ----------------------------------------------------------------------------
// VLogStdout
// ----------------------------------------------------------------------------
class VLogStdout : public VLog
{
public:
  VLogStdout();
  virtual ~VLogStdout();

public:
  bool autoFlush;

protected:
  virtual void write(const char* buf, int len);

public:
  virtual VLog* createByURI(const QString& uri);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

#endif // __V_LOG_STDOUT_H__
