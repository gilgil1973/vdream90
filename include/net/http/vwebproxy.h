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
#include <VHttpChunkBody>
#include <VDataChange>

// ----------------------------------------------------------------------------
// VWebProxyOutPolicy
// ----------------------------------------------------------------------------
class VWebProxyOutPolicy : public VXmlable, public VOptionable
{
public:
  enum Method {
    Auto,
    Http,
    Https,
  };

public:
  VWebProxyOutPolicy();

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
// VWebProxySessionStatus
// ----------------------------------------------------------------------------
typedef enum
{
  HeaderCaching,
  ContentCaching,
  Chunking,
  Streaming
} VWebProxySessionStatus;

// ----------------------------------------------------------------------------
// VWebProxyConnection
// ----------------------------------------------------------------------------
class VWebProxyConnection
{
public:
  VWebProxyConnection(VNetSession* inSession, VNetClient* outClient);

public:
  // bool operator==(const VWebProxyConnection& rhs); // gilgil temp 2014.04.08

public:
  VNetSession* inSession;
  VNetClient*  outClient;
  VTick        lastAccessTick;
};

// ----------------------------------------------------------------------------
// VWebProxyConnections
// ----------------------------------------------------------------------------
class VWebProxyConnections : public QList<VWebProxyConnection*>, public VLockable
{
};

// ----------------------------------------------------------------------------
// VWebProxyKeepAliveThread
// ----------------------------------------------------------------------------
class VWebProxyKeepAliveThread : public VThread
{
public:
  VWebProxyKeepAliveThread(void* owner);
  ~VWebProxyKeepAliveThread();

protected:
  VEvent event;

protected:
  virtual void run();
};

// ----------------------------------------------------------------------------
// VWebProxyOutInThread
// ----------------------------------------------------------------------------
class VWebProxy;
class VWebProxyOutInThread : public VThread
{
  friend class VWebProxy;

protected:
  VWebProxy*           webProxy;
  VWebProxyConnection* connection;

public:
  VWebProxyOutInThread(VWebProxyConnection* connection, void* owner);
  virtual ~VWebProxyOutInThread();

protected:
  bool closeInSessionOnEnd;

protected:
  virtual void run();
};

// ----------------------------------------------------------------------------
// VWebProxy
// ----------------------------------------------------------------------------
class VWebProxy : public VObject, public VOptionable
{
  Q_OBJECT

  friend class VWebProxyOutInThread;
  friend class VWebProxyKeepAliveThread;

public:
  VWebProxy(void* owner = NULL);
  virtual ~VWebProxy();

protected:
  virtual bool doOpen();
  virtual bool doClose();

public:
  static const int DEFAULT_HTTP_PORT = 80;
  static const int DEFAULT_SSL_PORT  = 443;
  static const int HTTP_PROXY_PORT   = 8080;
  static const int SSL_PROXY_PORT    = 4433;

public:
  bool                enabled;
  bool                httpEnabled;
  bool                httpsEnabled;
  int                 maxContentCacheSize;
  bool                disableLoopbackConnection;
  VTimeout            keepAliveTimeout;
  VTimeout            outInThreadTimeout;
  VWebProxyOutPolicy outPolicy;
  VTcpServer          tcpServer;
  VSslServer          sslServer;
  VDataChange         inboundDataChange;
  VDataChange         outboundDataChange;

public:
  VWebProxyConnections connections;

protected:
  VWebProxyKeepAliveThread* keepAliveThread;

public slots:
  void tcpRun(VTcpSession* tcpSession);
  void sslRun(VSslSession* sslSession);

protected:
  bool determineHostAndPort(VHttpRequest& request, int defaultPort, QString& host, int& port);

  QByteArray flushRequestHeader     (VHttpRequest&   request,                    VWebProxyConnection* connection);
  QByteArray flushRequestHeaderBody (VHttpRequest&   request,  QByteArray& body, VWebProxyConnection* connection);
  QByteArray flushRequestChunkBody  (VHttpChunkBody& chunkBody,                  VWebProxyConnection* connection);
  QByteArray flushRequestBuffer     (QByteArray&     buffer,                     VWebProxyConnection* connection);

  QByteArray flushResponseHeader    (VHttpResponse&  response,                   VWebProxyConnection* connection);
  QByteArray flushResponseHeaderBody(VHttpResponse&  response, QByteArray& body, VWebProxyConnection* connection);
  QByteArray flushResponseChunkBody (VHttpChunkBody& chunkBody,                  VWebProxyConnection* connection);
  QByteArray flushResponseBuffer    (QByteArray&     buffer,                     VWebProxyConnection* connection);

protected:
  void run(VNetSession* inSession);

signals:
  void onHttpRequestHeader (VHttpRequest*  request,  VWebProxyConnection* connection);
  void onHttpResponseHeader(VHttpResponse* response, VWebProxyConnection* connection);
  void onHttpRequestBody   (QByteArray*    body,     VWebProxyConnection* connection);
  void onHttpResponseBody  (QByteArray*    body,     VWebProxyConnection* connection);

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
