#include <VHttpProxy>
#include <VDebugNew>

REGISTER_METACLASS(VHttpProxy, VNet)

// ----------------------------------------------------------------------------
// VHttpProxyOutPolicy
// ---------------------------------------------------------------------------
VHttpProxyOutPolicy::VHttpProxyOutPolicy()
{
  method = Auto;
  host   = "";
  port   = 0;
}

void VHttpProxyOutPolicy::load(VXml xml)
{
  method = (Method)xml.getInt("method", (int)method);
  host   = xml.getStr("host", host);
  port   = xml.getInt("port", port);
}

void VHttpProxyOutPolicy::save(VXml xml)
{
  xml.setInt("method", (int)method);
  xml.setStr("host", host);
  xml.setInt("port", port);
}

#ifdef QT_GUI_LIB
void VHttpProxyOutPolicy::optionAddWidget(QLayout* layout)
{
  QStringList sl; sl << "Auto" << "TCP" << "SSL";
  VOptionable::addComboBox(layout, "cbxMethod", "Method", sl, (int)method);
  VOptionable::addLineEdit(layout, "leHost", "Host", host);
  VOptionable::addLineEdit(layout, "lePort", "Port", QString::number(port));
}

void VHttpProxyOutPolicy::optionSaveDlg(QDialog* dialog)
{
  method = (Method)(dialog->findChild<QComboBox*>("cbxMethod")->currentIndex());
  host   = dialog->findChild<QLineEdit*>("leHost")->text();
  port   = dialog->findChild<QLineEdit*>("lePort")->text().toInt();
}
#endif // QT_GUI_LIB

// ----------------------------------------------------------------------------
// VHttpProxyConnection
// ----------------------------------------------------------------------------
VHttpProxyConnection::VHttpProxyConnection(VNetSession* inSession, VNetClient* outClient)
{
  this->inSession      = inSession;
  this->outClient      = outClient;
  this->lastAccessTick = tick();
}

bool VHttpProxyConnection::operator==(const VHttpProxyConnection& rhs)
{
  if (this->inSession != rhs.inSession) return false;
  if (this->outClient != rhs.outClient) return false;
  return true;
}

// ----------------------------------------------------------------------------
// VHttpProxyKeepAliveThread
// ----------------------------------------------------------------------------
VHttpProxyKeepAliveThread::VHttpProxyKeepAliveThread(void* owner) : VThread(owner)
{
  event.resetEvent();
}

VHttpProxyKeepAliveThread::~VHttpProxyKeepAliveThread()
{
  event.setEvent();
  close();
}

void VHttpProxyKeepAliveThread::run()
{
  VHttpProxy* httpProxy = (VHttpProxy*)owner;
  LOG_ASSERT(httpProxy != NULL);

  while (true)
  {
    bool res = event.wait(10000); // 10 sec
    if (res) break;
    VTick nowTick = tick();
    httpProxy->connections.lock();
    for (VHttpProxyConnections::iterator it = httpProxy->connections.begin(); it != httpProxy->connections.end(); it++)
    {
      VHttpProxyConnection& connection = *it;
      if (connection.lastAccessTick + httpProxy->keepAliveTimeout < nowTick)
      {
        connection.inSession->close();
        connection.outClient->close();
      }
    }
    httpProxy->connections.unlock();
  }
}

// ----------------------------------------------------------------------------
// VHttpProxyOutInThread
// ----------------------------------------------------------------------------
class VHttpProxyOutInThread : public VThread
{
protected:
  VHttpProxy*  httpProxy;
  VNetClient*  outClient;
  VNetSession* inSession;

public:
  VHttpProxyOutInThread(VNetClient* outClient, VNetSession* inSession, void* owner) : VThread(owner)
  {
    this->httpProxy = (VHttpProxy*)owner;
    this->outClient = outClient;
    this->inSession = inSession;
  }
  virtual ~VHttpProxyOutInThread()
  {
    close();
  }

protected:
  virtual void run()
  {
    tag = 2000; // gilgil temp 2014.04.04
    VNetSession* outSession;
    {
      VTcpClient* tcpClient = dynamic_cast<VTcpClient*>(outClient);
      if (tcpClient != NULL) outSession = tcpClient->tcpSession;

      VSslClient* sslClient = dynamic_cast<VSslClient*>(outClient);
      if (sslClient != NULL) outSession = sslClient->tcpSession;

      if (outSession == NULL)
      {
        LOG_FATAL("outSession is NULL");
        return;
      }
    }
    // LOG_DEBUG("stt"); // gilgil temp 2014.03.14
    tag = 2010; // gilgil temp 2014.04.04

    QByteArray             buffer;
    VHttpProxyRecvStatus   status = HeaderCaching;
    VHttpResponse          response;
    int                    contentLength   = 0;

    while (true)
    {
      tag = 2020; // gilgil temp 2014.04.04
      //
      // read oneBuffer from inSession
      //
      QByteArray oneBuffer;
      int readLen = outClient->read(oneBuffer);
      tag = 2030; // gilgil temp 2014.04.04
      if (readLen == VERR_FAIL) break;

      buffer += oneBuffer;

      tag = 2040; // gilgil temp 2014.04.04
      //
      // HeaderCaching
      //
      if (status == HeaderCaching)
      {
        QByteArray body;
        if (response.parse(buffer, &body)) // header parsing completed
        {
          buffer        = body;
          contentLength = response.header.value("Content-Length").toInt();
          if (contentLength > 0)
          {
            status = BodyCaching;
          } else
          {
            QByteArray headerData = response.toByteArray();
            httpProxy->inboundDataChange.change(headerData);
            response.parse(headerData, NULL);
            emit httpProxy->onHttpResponseHeader(&response, outClient, inSession);
            if (inSession->write(response.toByteArray()) == VERR_FAIL) break;
            status = BodyStreaming;
          }
        }
      }
      tag = 2050; // gilgil temp 2014.04.04

      //
      // BodyCaching
      //
      if (status == BodyCaching)
      {
        tag = 2051; // gilgil temp 2014.04.04
        int length = buffer.length();
        // LOG_DEBUG("length=%d contentLength=%d", length, contentLength); // gilgil temp 2014.04.02
        if (length != 0)
        {
          tag = 2052; // gilgil temp 2014.04.04
          if (length == contentLength) // body completed
          {
            tag = 2053; // gilgil temp 2014.04.04
            if (!httpProxy->flushHttpResponseHeaderAndBody(response, buffer, outClient, inSession)) break;
            tag = 2054; // gilgil temp 2014.04.04
            status = HeaderCaching;
          } else
          if (length > contentLength)
          {
            tag = 2055; // gilgil temp 2014.04.04
            LOG_WARN("length(%d) is bigger than contentLength(%d)", length, contentLength);
            status = BodyStreaming;
          }
          tag = 2056; // gilgil temp 2014.04.04
        }
      }
      tag = 2060; // gilgil temp 2014.04.04

      //
      // BodyStreaming
      //
      if (status == BodyStreaming)
      {
        tag = 2062; // gilgil temp 2014.04.04
        if (buffer != "")
        {
          tag = 2064; // gilgil temp 2014.04.04
          httpProxy->inboundDataChange.change(buffer);
          tag = 2066; // gilgil temp 2014.04.04
          emit httpProxy->onHttpResponseBody(&response, &buffer, outClient, inSession);
          tag = 2068; // gilgil temp 2014.04.04
          if (inSession->write(buffer) == VERR_FAIL) break;
          tag = 2070; // gilgil temp 2014.04.04
          buffer = "";
          tag = 2072; // gilgil temp 2014.04.04
        }
      }
      tag = 2079; // gilgil temp 2014.04.04
    }

    // sleep(5); // gilgil temp 2014.04.02
    tag = 2080; // gilgil temp 2014.04.04
    outClient->close();
    tag = 2090; // gilgil temp 2014.04.04
    inSession->close();
    tag = 2010; // gilgil temp 2014.04.04
    // LOG_DEBUG("end"); // gilgil temp 2014.03.14
  }
};

// ----------------------------------------------------------------------------
// VHttpProxy
// ----------------------------------------------------------------------------
VHttpProxy::VHttpProxy(void* owner) : VObject(owner)
{
  tcpEnabled                = true;
  sslEnabled                = true;
  maxContentCacheSize       = 10485756; // 1MByte
  disableLoopbackConnection = true;
  keepAliveTimeout          = 60000; // 60 sec
  outInThreadTimeout        = 30000; // 30 sec
  tcpServer.port            = HTTP_PROXY_PORT;
  sslServer.port            = SSL_PROXY_PORT;

  keepAliveThread     = NULL;

  VObject::connect(&tcpServer, SIGNAL(runned(VTcpSession*)), this, SLOT(tcpRun(VTcpSession*)), Qt::DirectConnection);
  VObject::connect(&sslServer, SIGNAL(runned(VSslSession*)), this, SLOT(sslRun(VSslSession*)), Qt::DirectConnection);
}

VHttpProxy::~VHttpProxy()
{
  close();
}

bool VHttpProxy::doOpen()
{
  if (tcpEnabled)
  {
    if (!tcpServer.open())
    {
      error = tcpServer.error;
      return false;
    }
  }

  if (sslEnabled)
  {
    if (!sslServer.open())
    {
      error = sslServer.error;
      return false;
    }
  }

  keepAliveThread = new VHttpProxyKeepAliveThread(this);
  keepAliveThread->open();

  if (!inboundDataChange.prepare(error)) return false;
  if (!outboundDataChange.prepare(error)) return false;

  return true;
}

bool VHttpProxy::doClose()
{
  connections.lock();
  for (VHttpProxyConnections::iterator it = connections.begin(); it != connections.end(); it++)
  {
    VHttpProxyConnection& connection = *it;
    connection.inSession->close();
    connection.outClient->close();
  }
  connections.unlock();

  tcpServer.close();
  sslServer.close();

  SAFE_DELETE(keepAliveThread);

  return true;
}

void VHttpProxy::tcpRun(VTcpSession* tcpSession)
{
  run(tcpSession);
}

void VHttpProxy::sslRun(VSslSession* sslSession)
{
  run(sslSession);
}

bool VHttpProxy::determineHostAndPort(VHttpRequest& request, int defaultPort, QString& host, int& port)
{
  QUrl url = request.requestLine.path;
  if (!url.isRelative())
  {
    host = url.host();
    port = url.port();

    QByteArray newPath = url.path().toUtf8();
    if (url.hasQuery())
      newPath += "?" + url.query(QUrl::FullyEncoded).toLatin1();
    request.requestLine.path = newPath;
  } else
  if (!request.findHost(host, port))
  {
    LOG_ERROR("can not find host:%s", request.toByteArray().data());
    return false;
  }
  if (port == -1) port = defaultPort;
  if (outPolicy.host != "") host = outPolicy.host;
  if (outPolicy.port != 0)  port = outPolicy.port;
  return true;
}

bool VHttpProxy::flushHttpRequestHeaderAndBody(VHttpRequest& request, QByteArray& body, VNetSession* inSession, VNetClient* outClient)
{
  // ----- by gilgil 2014.04.08 -----
  // headerData should be changed before connecto to server
  /*
  QByteArray headerData = request.toByteArray();
  outboundDataChange.change(headerData);
  emit onHttpRequestHeader(&request, inSession, outClient);
  request.parse(headerData, NULL);
  */
  // --------------------------------

  int oldBodyLen = body.length();
  outboundDataChange.change(body);
  emit onHttpRequestBody(&request, &body, inSession, outClient);
  int newBodyLen = body.length();

  int contentLength = request.header.value("Content-Length").toInt();
  if (contentLength != 0 && newBodyLen != oldBodyLen)
  {
    contentLength += newBodyLen - oldBodyLen;
    request.header.setValue("Content-Length", QByteArray::number(contentLength));
    LOG_DEBUG("content length change from %d > %d", oldBodyLen, newBodyLen);
  }

  if (outClient->write(request.toByteArray() + body) == VERR_FAIL) return false;

  body = "";

  return true;
}

bool VHttpProxy::flushHttpResponseHeaderAndBody(VHttpResponse& response, QByteArray& body, VNetClient* outClient, VNetSession* inSession)
{
  QByteArray headerData = response.toByteArray();
  inboundDataChange.change(headerData);
  emit onHttpResponseHeader(&response, outClient, inSession);
  response.parse(headerData, NULL);

  int oldBodyLen = body.length();
  inboundDataChange.change(body);
  emit onHttpResponseBody(&response, &body, outClient, inSession);
  int newBodyLen = body.length();

  int contentLength = response.header.value("Content-Length").toInt();
  if (contentLength != 0 && newBodyLen != oldBodyLen)
  {
    contentLength += newBodyLen - oldBodyLen;
    response.header.setValue("Content-Length", QByteArray::number(contentLength));
    LOG_DEBUG("content length change from %d > %d", oldBodyLen, newBodyLen);
  }

  if (inSession->write(response.toByteArray() + body) == VERR_FAIL) return false;

  body = "";

  return true;
}

void VHttpProxy::run(VNetSession* inSession)
{
  // LOG_DEBUG("stt inSession=%p", inSession); // gilgil temp 2014.03.14

  VNetClient* outClient = NULL;
  int defaultPort;

  //
  // determine outClient and defaultPort
  //
  switch (outPolicy.method)
  {
    case VHttpProxyOutPolicy::Auto:
      if (dynamic_cast<VTcpSession*>(inSession) != NULL)
      {
        outClient   = new VTcpClient;
        defaultPort = DEFAULT_HTTP_PORT;
      } else
      if (dynamic_cast<VSslSession*>(inSession) != NULL)
      {
        outClient   = new VSslClient;
        defaultPort = DEFAULT_SSL_PORT;
      } else
      {
        LOG_FATAL("invalid inSession type(%s)", qPrintable(inSession->className()));
        return;
      }
      break;
    case VHttpProxyOutPolicy::Tcp:
      outClient   = new VTcpClient;
      defaultPort = DEFAULT_HTTP_PORT;
      break;
    case VHttpProxyOutPolicy::Ssl:
      outClient   = new VSslClient;
      defaultPort = DEFAULT_SSL_PORT;
      break;
    default:
      LOG_FATAL("invalid method value(%d)", (int)outPolicy.method);
      return;
  }
  connections.lock();
  VHttpProxyConnection connection(inSession, outClient);
  connections.push_back(connection);
  connections.unlock();

  QByteArray           buffer;
  VHttpProxyRecvStatus status        = HeaderCaching;
  VHttpRequest         request;
  int                  contentLength = 0;
  VHttpProxyOutInThread* thread      = NULL;

  while (true)
  {
    //
    // read oneBuffer from inSession
    //
    QByteArray oneBuffer;
    int readLen = inSession->read(oneBuffer);
    if (readLen == VERR_FAIL) break;

    buffer += oneBuffer;

    //
    // HeaderCaching
    //
    if (status == HeaderCaching)
    {
      QByteArray body;
      if (request.parse(buffer, &body)) // header parsing completed
      {
        // ----- by gilgil 2014.04.08 -----
        // headerData should be changed before connecto to server
        QByteArray headerData = request.toByteArray();
        outboundDataChange.change(headerData);
        emit onHttpRequestHeader(&request, inSession, outClient);
        request.parse(headerData, NULL);
        // --------------------------------

        QString host;
        int port;
        if (!determineHostAndPort(request, defaultPort, host, port))
        {
          LOG_ERROR("can not determine host and port %s", qPrintable(buffer));
          break;
        }
        // LOG_DEBUG("host=%s port=%d", qPrintable(host), port); // gilgil temp 2014.04.02
        if (outClient->host != host || outClient->port != port)
        {
          outClient->close();
          if (thread != NULL) delete thread;
          outClient->host = host;
          outClient->port = port;
          LOG_DEBUG("opening %s:%d", qPrintable(host), port);

          if (disableLoopbackConnection)
          {
            Ip ip = VNet::resolve(host);
            if (ip.isLocalHost())
            {
              LOG_DEBUG("loopback(%s) connection refused", qPrintable(ip.str()));
              break;
            }
          }
          if (!outClient->open())
          {
            LOG_ERROR("%s", outClient->error.msg);
            break;
          }
          thread = new VHttpProxyOutInThread(outClient, inSession, this);
          thread->open();
        }
        buffer        = body;
        contentLength = request.header.value("Content-Length").toInt();
        if (contentLength > 0)
        {
          status = BodyCaching;
        } else
        {
          QByteArray headerData = request.toByteArray();
          outboundDataChange.change(headerData);
          request.parse(headerData, NULL);
          emit onHttpRequestHeader(&request, inSession, outClient);
          outClient->write(request.toByteArray());
          status = HeaderCaching;
        }
      }
    }

    //
    // BodyCaching
    //
    if (status == BodyCaching)
    {
      int length = buffer.length();
      if (length != 0)
      {
        if (length == contentLength) // body completed
        {
          if (!flushHttpRequestHeaderAndBody(request, buffer, inSession, outClient)) break;
          status = HeaderCaching;
        } else
        if (length > contentLength)
        {
          LOG_WARN("length(%d) is bigger than contentLength(%d)", length, contentLength);
          status = BodyStreaming;
        }
      }
    }

    //
    // BodyStreaming
    //
    if (status == BodyStreaming)
    {
      if (buffer != "")
      {
        outboundDataChange.change(buffer);
        emit onHttpRequestBody(&request, &buffer, inSession, outClient);
        outClient->write(buffer);
        buffer = "";
      }
    }
  }
  // LOG_DEBUG("end inSession=%p", inSession); // gilgil temp 2014.03.14
  inSession->close();
  outClient->close();
  if (thread != NULL)
  {
    thread->close(true, outInThreadTimeout);
    delete thread;
  }

  connections.lock();
  connections.removeAt(connections.indexOf(connection));
  connections.unlock();

  delete outClient;
}

void VHttpProxy::load(VXml xml)
{
  VObject::load(xml);

  tcpEnabled                = xml.getBool("tcpEnabled",                tcpEnabled);
  sslEnabled                = xml.getBool("sslEnabled",                sslEnabled);
  maxContentCacheSize       = xml.getInt("maxContentCacheSize",        maxContentCacheSize);
  disableLoopbackConnection = xml.getBool("disableLoopbackConnection", disableLoopbackConnection);
  keepAliveTimeout          = xml.getULong("keepAliveTimeout",         keepAliveTimeout);
  outInThreadTimeout        = xml.getULong("outInThreadTimeout",       outInThreadTimeout);
  outPolicy.load(xml.gotoChild("outPolicy"));
  tcpServer.load(xml.gotoChild("tcpServer"));
  sslServer.load(xml.gotoChild("sslServer"));
  if (!(xml.findChild("inboundDataChange").isNull()))  inboundDataChange.load(xml.gotoChild("inboundDataChange"));
  if (!(xml.findChild("outboundDataChange").isNull())) outboundDataChange.load(xml.gotoChild("outboundDataChange"));
}

void VHttpProxy::save(VXml xml)
{
  VObject::save(xml);

  xml.setBool("tcpEnabled",                tcpEnabled);
  xml.setBool("sslEnabled",                sslEnabled);
  xml.setInt("maxContentCacheSize",        maxContentCacheSize);
  xml.setBool("disableLoopbackConnection", disableLoopbackConnection);
  xml.setULong("keepAliveTimeout",         keepAliveTimeout);
  xml.setULong("outInThreadTimeout",       outInThreadTimeout);
  outPolicy.save(xml.gotoChild("outPolicy"));
  tcpServer.save(xml.gotoChild("tcpServer"));
  sslServer.save(xml.gotoChild("sslServer"));
  inboundDataChange.save(xml.gotoChild("inboundDataChange"));
  outboundDataChange.save(xml.gotoChild("outboundDataChange"));
}

#ifdef QT_GUI_LIB
#include "vhttpproxywidget.h"
#include "ui_vhttpproxywidget.h"
void VHttpProxy::optionAddWidget(QLayout* layout)
{
  VHttpProxyWidget* widget = new VHttpProxyWidget(layout->parentWidget());
  widget->setObjectName("httpProxyWidget");

  VOptionable::addCheckBox(widget->ui->glTcpServer, "chkTcpEnabled", "TCP Enabled", tcpEnabled);
  VOptionable::addCheckBox(widget->ui->glSslServer, "chkSslEnabled", "SSL Enabled", sslEnabled);

  outPolicy.optionAddWidget(widget->ui->glExternal);
  tcpServer.optionAddWidget(widget->ui->glTcpServer);
  sslServer.optionAddWidget(widget->ui->glSslServer);

  VOptionable::addLineEdit(widget->ui->glOther,     "leMaxContentCacheSize",        "Max Content Cache Size",      QString::number(maxContentCacheSize));
  VOptionable::addCheckBox(widget->ui->glOther,     "chkDisableLoopbackConnection", "Disable Loopback Connection", disableLoopbackConnection);
  VOptionable::addLineEdit(widget->ui->glOther,     "leKeepAliveTimeout",           "KeepAlive Timeout",           QString::number(keepAliveTimeout));
  VOptionable::addLineEdit(widget->ui->glOther,     "leOutInThreadTimeout",         "OutIn Thread Timeout",        QString::number(outInThreadTimeout));

  inboundDataChange.optionAddWidget(widget->ui->glInbound);
  outboundDataChange.optionAddWidget(widget->ui->glOutbound);

  layout->addWidget(widget);
}

void VHttpProxy::optionSaveDlg(QDialog* dialog)
{
  VHttpProxyWidget* widget = dialog->findChild<VHttpProxyWidget*>("httpProxyWidget");
  LOG_ASSERT(widget != NULL);

  tcpEnabled = widget->findChild<QCheckBox*>("chkTcpEnabled")->checkState() == Qt::Checked;
  sslEnabled = widget->findChild<QCheckBox*>("chkSslEnabled")->checkState() == Qt::Checked;


  outPolicy.optionSaveDlg((QDialog*)widget->ui->tabExternal);
  tcpServer.optionSaveDlg((QDialog*)widget->ui->tabTcpServer);
  sslServer.optionSaveDlg((QDialog*)widget->ui->tabSslServer);

  maxContentCacheSize       = widget->findChild<QLineEdit*>("leMaxContentCacheSize")->text().toInt();
  disableLoopbackConnection = widget->findChild<QCheckBox*>("chkDisableLoopbackConnection")->checkState() == Qt::Checked;
  keepAliveTimeout          = widget->findChild<QLineEdit*>("leKeepAliveTimeout")->text().toInt();
  outInThreadTimeout        = widget->findChild<QLineEdit*>("leOutInThreadTimeout")->text().toInt();

  inboundDataChange.optionSaveDlg((QDialog*)widget->ui->tabInbound);
  outboundDataChange.optionSaveDlg((QDialog*)widget->ui->tabOutbound);
}
#endif // QT_GUI_LIB
