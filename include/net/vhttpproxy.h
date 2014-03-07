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
#include <VSslServer>
#include <VSslClient>
#include <QRegExp>
#include <QUrl>
#include <VHttpRequest>

// ----------------------------------------------------------------------------
// VHttpProxyOutPolicy
// ---------------------------------------------------------------------------
class VHttpProxyOutPolicy : public VXmlable, public VOptionable
{
public:
  enum Method {
    Auto,
    Tcp,
    Ssl,
  };

public:
  VHttpProxyOutPolicy();

public:

  Method  method;
  QString host;
  int     port;

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public: // for VOptionable
  virtual void optionAddWidget(QLayout* layout);
  virtual void optionSaveDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

// ----------------------------------------------------------------------------
// VHttpProxy
// ----------------------------------------------------------------------------
class VHttpProxy : public VObject, public VOptionable
{
  Q_OBJECT

public:


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
  const static int DEFAULT_HTTP_PORT = 80;
  const static int DEFAULT_SSL_PORT  = 443;
  const static int HTTP_PROXY_PORT   = 8080;
  const static int SSL_PROXY_PORT    = 4433;

public:
  bool                tcpEnabled;
  bool                sslEnabled;
  VHttpProxyOutPolicy outPolicy;
  VTcpServer          tcpServer;
  VSslServer          sslServer;

public slots:
  void tcpRun(VTcpSession* tcpSession);
  void sslRun(VSslSession* sslSession);

protected:
  void run(VNetSession* inSession);

signals:
  void beforeMsg(QByteArray msg, VNetSession* session);
  void beforeRequest(VHttpRequest& request, VNetSession* inSession, VNetClient* outClient);
  void beforeResponse(QByteArray& msg, VNetClient* outClient, VNetSession* inSession);

#ifdef QT_GUI_LIB
public: // for VOptionable
  virtual void optionAddWidget(QLayout* layout);
  virtual void optionSaveDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_HTTP_PROXY_H__
