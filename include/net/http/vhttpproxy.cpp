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

// ----- gilgil temp 2014.04.08 -----
/*
bool VHttpProxyConnection::operator==(const VHttpProxyConnection& rhs)
{
  if (this->inSession != rhs.inSession) return false;
  if (this->outClient != rhs.outClient) return false;
  return true;
}
*/
// ----------------------------------

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
      VHttpProxyConnection* connection = *it;
      if (connection->lastAccessTick + httpProxy->keepAliveTimeout < nowTick)
      {
        connection->inSession->close();
        connection->outClient->close();
      }
    }
    httpProxy->connections.unlock();
  }
}

// ----------------------------------------------------------------------------
// VHttpProxyOutInThread
// ----------------------------------------------------------------------------
VHttpProxyOutInThread::VHttpProxyOutInThread(VHttpProxyConnection* connection, void* owner) : VThread(owner)
{
  this->httpProxy     = (VHttpProxy*)owner;
  this->connection    = connection;
  closeInSessionOnEnd = true;
}

VHttpProxyOutInThread::~VHttpProxyOutInThread()
{
  close();
}

void VHttpProxyOutInThread::run()
{
  // LOG_DEBUG("stt"); // gilgil temp 2014.03.14

  VNetClient* outClient  = connection->outClient;
  VNetSession* inSession = connection->inSession;

  QByteArray              buffer;
  VHttpProxySessionStatus status = HeaderCaching;
  VHttpResponse           response;
  int                     contentLength = 0;

  while (true)
  {
    QByteArray sendBuffer = "";

    //
    // read oneBuffer from outClient
    //
    QByteArray oneBuffer;
    int readLen = outClient->read(oneBuffer);
    if (readLen == VERR_FAIL)
    {
      if (status == ContentCaching)
      {
        sendBuffer = httpProxy->flushResponseHeaderBody(response, buffer, connection);
      } else
      {
        sendBuffer = httpProxy->flushResponseBuffer(buffer, connection);
      }
      if (sendBuffer != "")
      {
        inSession->write(sendBuffer);
      }
      break;
    }
    LOG_DEBUG("%s", qPrintable(oneBuffer)); // gilgil temp 2014.04.10
    connection->lastAccessTick = tick();
    buffer += oneBuffer;

    //
    // HeaderCaching
    //
    if (status == HeaderCaching)
    {
      if (response.parse(buffer)) // header parsing completed
      {
        contentLength = response.header.value("Content-Length", false).toInt();
        if (contentLength > 0)
        {
          status = ContentCaching;
          // LOG_DEBUG("ContentCaching"); // gilgil temp 2014.04.10
        } else
        if (response.header.value("Transfer-Encoding").toLower() == "chunked")
        {
          sendBuffer += httpProxy->flushResponseHeader(response, connection);
          status = Chunking;
          // LOG_DEBUG("Chunking"); // gilgil temp 2014.04.10
        } else
        {
          sendBuffer += httpProxy->flushResponseHeader(response, connection);
        }
      }
    }

    //
    // ContentCaching
    //
    if (status == ContentCaching)
    {
      int length = buffer.length();
      // LOG_DEBUG("length=%d contentLength=%d", length, contentLength); // gilgil temp 2014.04.02
      if (length >= contentLength) // body completed
      {
        if (length > contentLength)
        {
          LOG_WARN("length(%d) is bigger than contentLength(%d)", length, contentLength);
        }
        sendBuffer += httpProxy->flushResponseHeaderBody(response, buffer, connection);
        status = HeaderCaching;
        // LOG_DEBUG("HeaderCaching"); // gilgil temp 2014.04.10
      }
    }

    //
    // Chunking
    //
    if (status == Chunking)
    {
      VHttpChunkBody chunkBody;
      int count = chunkBody.parse(buffer);
      if (count > 0)
      {
        bool lastChunk = chunkBody.items.at(count - 1).first == 0; // may be end of chunking
        sendBuffer += httpProxy->flushResponseChunkBody(chunkBody, connection);
        if (lastChunk)
        {
          sendBuffer += buffer;
          buffer = "";
          status = HeaderCaching;
          // LOG_DEBUG("HeaderCaching"); // gilgil temp 2014.04.10
        }
      }
    }

    //
    // Streaming
    //
    if (status == Streaming)
    {
      if (buffer != "")
      {
        sendBuffer += httpProxy->flushResponseBuffer(buffer, connection);
      }
    }

    //
    // write sendBuffer to inSession
    //
    if (sendBuffer != "")
    {
      int writeLen = inSession->write(sendBuffer);
      if (writeLen == VERR_FAIL) break;
    }
  }

  // sleep(5); // gilgil temp 2014.04.02
  tag = 2080; // gilgil temp 2014.04.04
  outClient->close();
  tag = 2090; // gilgil temp 2014.04.04
  if (closeInSessionOnEnd) inSession->close();
  tag = 2010; // gilgil temp 2014.04.04
  // LOG_DEBUG("end"); // gilgil temp 2014.03.14
}

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

  VDataChangeItem changeItem;
  changeItem.pattern = "Accept-Encoding: gzip,";
  changeItem.syntax  = QRegExp::FixedString;
  changeItem.cs      = Qt::CaseSensitive;
  changeItem.minimal = false;
  changeItem.enabled = true;
  changeItem.log     = false;
  changeItem.replace = "Accept-Encoding:      ";
  outboundDataChange.push_back(changeItem);

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
    VHttpProxyConnection* connection = *it;
    connection->inSession->close();
    connection->outClient->close();
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

QByteArray VHttpProxy::flushRequestHeader(VHttpRequest& request, VHttpProxyConnection* connection)
{
  QByteArray headerData = request.toByteArray();
  outboundDataChange.change(headerData);
  emit onHttpRequestHeader(&request, connection);
  request.parse(headerData);

  QByteArray res = request.toByteArray();
  request.clear();
  return res;
}

QByteArray VHttpProxy::flushRequestHeaderBody(VHttpRequest& request, QByteArray& body, VHttpProxyConnection* connection)
{
  QByteArray headerData = request.toByteArray();
  outboundDataChange.change(headerData);
  emit onHttpRequestHeader(&request, connection);
  request.parse(headerData);

  int oldBodyLen = body.length();
  outboundDataChange.change(body);
  emit onHttpRequestBody(&body, connection);
  int newBodyLen = body.length();

  int contentLength = request.header.value("Content-Length").toInt();
  if (contentLength != 0 && newBodyLen != oldBodyLen)
  {
    contentLength += newBodyLen - oldBodyLen;
    request.header.setValue("Content-Length", QByteArray::number(contentLength));
    LOG_DEBUG("content length change from %d > %d", oldBodyLen, newBodyLen);
  }

  QByteArray res = request.toByteArray() + body;
  request.clear();
  body.clear();
  return res;
}

QByteArray VHttpProxy::flushRequestChunkBody(VHttpChunkBody& chunkBody, VHttpProxyConnection* connection)
{
  for (VHttpChunkBody::Items::iterator it = chunkBody.items.begin(); it != chunkBody.items.end(); it++)
  {
    VHttpChunkBody::Item& item = *it;
    if (outboundDataChange.change(item.second))
    {
      item.first = item.second.length();
    }
  }

  QByteArray res = chunkBody.toByteArray();
  emit onHttpRequestBody(&res, connection);
  chunkBody.clear();
  return res;
}

QByteArray VHttpProxy::flushRequestBuffer(QByteArray& buffer, VHttpProxyConnection* connection)
{
  outboundDataChange.change(buffer);
  emit onHttpRequestBody(&buffer, connection);
  QByteArray res = buffer;
  buffer = "";
  return res;
}

QByteArray VHttpProxy::flushResponseHeader(VHttpResponse& response, VHttpProxyConnection* connection)
{
  QByteArray headerData = response.toByteArray();
  inboundDataChange.change(headerData);
  emit onHttpResponseHeader(&response, connection);
  response.parse(headerData);

  QByteArray res = response.toByteArray();
  response.clear();
  return res;
}

QByteArray VHttpProxy::flushResponseHeaderBody(VHttpResponse& response, QByteArray& body, VHttpProxyConnection* connection)
{
  QByteArray headerData = response.toByteArray();
  inboundDataChange.change(headerData);
  emit onHttpResponseHeader(&response, connection);
  response.parse(headerData);

  int oldBodyLen = body.length();
  inboundDataChange.change(body);
  emit onHttpResponseBody(&body, connection);
  int newBodyLen = body.length();

  int contentLength = response.header.value("Content-Length").toInt();
  if (contentLength != 0 && newBodyLen != oldBodyLen)
  {
    contentLength += newBodyLen - oldBodyLen;
    response.header.setValue("Content-Length", QByteArray::number(contentLength));
    LOG_DEBUG("content length change from %d > %d", oldBodyLen, newBodyLen);
  }

  QByteArray res = response.toByteArray() + body;
  response.clear();
  body.clear();
  return res;
}

QByteArray VHttpProxy::flushResponseChunkBody(VHttpChunkBody& chunkBody, VHttpProxyConnection* connection)
{
  for (VHttpChunkBody::Items::iterator it = chunkBody.items.begin(); it != chunkBody.items.end(); it++)
  {
    VHttpChunkBody::Item& item = *it;
    if (inboundDataChange.change(item.second))
    {
      item.first = item.second.length();
    }
  }

  QByteArray res = chunkBody.toByteArray();
  emit onHttpResponseBody(&res, connection);
  chunkBody.clear();
  return res;
}

QByteArray VHttpProxy::flushResponseBuffer(QByteArray& buffer, VHttpProxyConnection* connection)
{
  inboundDataChange.change(buffer);
  emit onHttpResponseBody(&buffer, connection);
  QByteArray res = buffer;
  buffer = "";
  return res;
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
  connections.push_back(&connection);
  connections.unlock();

  QByteArray              buffer;
  VHttpProxySessionStatus status = HeaderCaching;
  VHttpRequest            request;
  int                     contentLength = 0;
  VHttpProxyOutInThread*  thread = NULL;

  while (true)
  {
    QByteArray sendBuffer = "";

    //
    // read oneBuffer from inSession
    //
    QByteArray oneBuffer;
    int readLen = inSession->read(oneBuffer);
    if (readLen == VERR_FAIL)
    {
      if (status == ContentCaching)
      {
        sendBuffer = flushRequestHeaderBody(request, buffer, &connection);
      } else
      {
        sendBuffer = flushRequestBuffer(buffer, &connection);
      }
      if (sendBuffer != "")
      {
        outClient->write(sendBuffer);
      }
      break;
    }
    connection.lastAccessTick = tick();
    buffer += oneBuffer;

    //
    // HeaderCaching
    //
    if (status == HeaderCaching)
    {
      if (request.parse(buffer)) // header parsing completed
      {
        //
        // Connect to server
        //
        QString host; int port;
        if (!determineHostAndPort(request, defaultPort, host, port))
        {
          LOG_ERROR("can not determine host and port %s", qPrintable(buffer));
          break;
        }
        // LOG_DEBUG("host=%s port=%d", qPrintable(host), port); // gilgil temp 2014.04.02
        if (outClient->host != host || outClient->port != port)
        {
          // if (thread != NULL) thread->closeInSessionOnEnd = false; // gilgil temp 2014.04.10
          outClient->close();
          if (thread != NULL)
          {
            thread->close(true, outInThreadTimeout);
            delete thread;
          }
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
            if (ip == 0) // can not resolve ip
            {
              break;
            }
          }
          if (!outClient->open())
          {
            LOG_ERROR("%s", outClient->error.msg);
            break;
          }
          thread = new VHttpProxyOutInThread(&connection, this);
          thread->open();
        }

        contentLength = request.header.value("Content-Length").toInt();
        if (contentLength > 0)
        {
          status = ContentCaching;
        } else
        if (request.header.value("Transfer-Encoding").toLower() == "chunked")
        {
          sendBuffer += flushRequestHeader(request, &connection);
          status = Chunking;
        } else
        {
          sendBuffer += flushRequestHeader(request, &connection);
        }
      }
    }

    //
    // ContentCaching
    //
    if (status == ContentCaching)
    {
      int length = buffer.length();
      // LOG_DEBUG("length=%d contentLength=%d", length, contentLength); // gilgil temp 2014.04.02
      if (length >= contentLength) // body completed
      {
        if (length > contentLength)
        {
          LOG_WARN("length(%d) is bigger than contentLength(%d)", length, contentLength);
        }
        sendBuffer += flushRequestHeaderBody(request, buffer, &connection);
        status = HeaderCaching;
      }
    }

    //
    // Chunking
    //
    if (status == Chunking)
    {
      VHttpChunkBody chunkBody;
      int count = chunkBody.parse(buffer);
      if (count > 0)
      {
        bool lastChunk = chunkBody.items.at(count - 1).first == 0; // may be end of chunking
        sendBuffer += flushRequestChunkBody(chunkBody, &connection);
        if (lastChunk)
        {
          sendBuffer += buffer;
          buffer = "";
          status = HeaderCaching;
        }
      }
    }

    //
    // Streaming
    //
    if (status == Streaming)
    {
      if (buffer != "")
      {
        sendBuffer += flushRequestBuffer(buffer, &connection);
      }
    }

    //
    // write sendBuffer to outClient
    //
    if (sendBuffer != "")
    {
      int writeLen = outClient->write(sendBuffer);
      if (writeLen == VERR_FAIL) break;
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
  int index = connections.indexOf(&connection);
  if (index == -1)
  {
    LOG_FATAL("index is -1");
  } else
  {
    connections.removeAt(index);
  }
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
