// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_LOG_UDP_H__
#define __V_LOG_UDP_H__

#include <VLog>

// ----------------------------------------------------------------------------
// VLogUdp
// ----------------------------------------------------------------------------
class VMyLogUDPClient;
class VLogUdp : public VLog
{
public:
  static const char* DEFAULT_HOST; // "127.0.0.1"
  static const int   DEFAULT_PORT; // 8908

public:
  VLogUdp(const char* host = DEFAULT_HOST, const int port = DEFAULT_PORT);
  virtual ~VLogUdp();

public:
  QString host;
  int     port;

protected:
  virtual bool open();
  virtual bool close();

protected:
  virtual void write(const char* buf, int len);

public:
  virtual VLog* createByURI(const QString& uri);

protected:
  SOCKET      handle;
  SOCKADDR_IN addr;

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

#endif // __V_LOG_UDP_H__
