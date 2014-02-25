// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_LOG_HTTP_H__
#define __V_LOG_HTTP_H__

#include <VLog>
#include <VTcpServer>

// ----------------------------------------------------------------------------
// VLogHttp
// ----------------------------------------------------------------------------
class VLogHttpTcpServer;
class VLogHttp : public VLog
{
public:
  static const int DEFAULT_PORT;

public:
  VLogHttp(const int port = DEFAULT_PORT);
  virtual ~VLogHttp();

public:
  int     port;

protected:
  virtual bool open();
  virtual bool close();

protected:
  virtual void write(const char* buf, int len);

public:
  virtual VLog* createByURI(const QString& uri);

protected:
  VLogHttpTcpServer* m_tcpServer;

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

// ----------------------------------------------------------------------------
// VLogHttpTcpServer
// ----------------------------------------------------------------------------
class VLogHttpTcpServer : public VTcpServer
{
  Q_OBJECT

public:
  VLogHttpTcpServer(void* owner = NULL);
  virtual ~VLogHttpTcpServer();

protected slots:
  void run(VTcpSession* tcpSession);
};

#endif // __V_LOG_HTTP_H__
