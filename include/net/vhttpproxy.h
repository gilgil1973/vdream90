// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_HTTP_PROXY_H__
#define __V_HTTP_PROXY_H__

#include <VObject>
#include <VTcpServer>
#include <VTcpClient>
#include <QRegExp>
#include <QUrl>
#include <VHttpRequest>

// ----------------------------------------------------------------------------
// VHttpProxy
// ----------------------------------------------------------------------------
class VHttpProxy : public VObject
{
  Q_OBJECT

public:
  VHttpProxy(void* owner = NULL);
  virtual ~VHttpProxy();

protected:
  virtual bool doOpen();
  virtual bool doClose();

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

public:
  const static int PORT = 8080;

public:
  VTcpServer tcpServer;
  bool       autoOpen;

public slots:
  void run(VTcpSession* tcpSession);

signals:
  void beforeRequest(VHttpRequest& request, VTcpSession* inSession, VTcpClient* outClient);
  void beforeResponse(QByteArray& msg, VTcpClient* outClient, VTcpSession* inSession);
};

#endif // __V_HTTP_PROXY_H__
