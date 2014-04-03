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
#include <VTick>
#include <VHttpRequest>
#include <VHttpResponse>
#include <VDataChange>

// ----------------------------------------------------------------------------
// VHttpProxyOutPolicy
// ----------------------------------------------------------------------------
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
// VHttpProxyRecvStatus
// ----------------------------------------------------------------------------
typedef enum
{
  HeaderCaching,
  BodyCaching,
  BodyStreaming
} VHttpProxyRecvStatus;

// ----------------------------------------------------------------------------
// VHttpProxyConnection
// ----------------------------------------------------------------------------
class VHttpProxyConnection
{
public:
  VHttpProxyConnection(VNetSession* inSession, VNetClient* outClient);

public:
  bool operator==(const VHttpProxyConnection& rhs);

public:
  VNetSession* inSession;
  VNetClient*  outClient;
  VTick        lastAccessTick;
};

// ----------------------------------------------------------------------------
// VHttpProxyConnections
// ----------------------------------------------------------------------------
class VHttpProxyConnections : public QList<VHttpProxyConnection>, public VLockable
{
};

// ----------------------------------------------------------------------------
// VHttpProxyKeepAliveThread
// ----------------------------------------------------------------------------
class VHttpProxyKeepAliveThread : public VThread
{
public:
  VHttpProxyKeepAliveThread(void* owner);
  ~VHttpProxyKeepAliveThread();

protected:
  VEvent event;

protected:
  virtual void run();
};

// ----------------------------------------------------------------------------
// VHttpProxy
// ----------------------------------------------------------------------------
class VHttpProxy : public VObject, public VOptionable
{
  Q_OBJECT

  friend class VHttpProxyOutInThread;
  friend class VHttpProxyKeepAliveThread;

public:
  VHttpProxy(void* owner = NULL);
  virtual ~VHttpProxy();

protected:
  virtual bool doOpen();
  virtual bool doClose();

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
  int                 maxContentCacheSize;
  VTimeout            keepAliveTimeout;
  VDataChange         inboundDataChange;
  VDataChange         outboundDataChange;

public:
  VHttpProxyConnections connections;

protected:
  VHttpProxyKeepAliveThread* keepAliveThread;

public slots:
  void tcpRun(VTcpSession* tcpSession);
  void sslRun(VSslSession* sslSession);

protected:
  bool determineHostAndPort(VHttpRequest& request, int defaultPort, QString& host, int& port);
  bool flushHttpRequestHeaderAndBody (VHttpRequest&  request, QByteArray& body, VNetSession* inSession, VNetClient* outClient);
  bool flushHttpResponseHeaderAndBody(VHttpResponse& response, QByteArray& body, VNetClient*  outClient, VNetSession* inSession);
  void run(VNetSession* inSession);

signals:
  // void beforeMsg(QByteArray msg, VNetSession* session); // gilgil temp 2014.04.041
  // void beforeRequest(VHttpRequest& request, VNetSession* inSession, VNetClient* outClient); // gilgil temp 2014.04.041
  // void beforeResponse(QByteArray& msg, VNetClient* outClient, VNetSession* inSession); // gilgil temp 2014.04.041

  void onHttpRequestHeader (VHttpRequest*  header,                   VNetSession* inSession, VNetClient*  outClient);
  void onHttpRequestBody   (VHttpRequest*  header, QByteArray* body, VNetSession* inSession, VNetClient*  outClient);
  void onHttpResponseHeader(VHttpResponse* header,                   VNetClient*  outClient, VNetSession* inSession);
  void onHttpResponseBody  (VHttpResponse* header, QByteArray* body, VNetClient*  outClient, VNetSession* inSession);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public: // for VOptionable
  virtual void optionAddWidget(QLayout* layout);
  virtual void optionSaveDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_HTTP_PROXY_H__
