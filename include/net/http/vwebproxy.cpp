#include <VWebProxy>
#include <VDebugNew>

REGISTER_METACLASS(VWebProxy, VNet)

// ----------------------------------------------------------------------------
// VWebProxyOutPolicy
// ---------------------------------------------------------------------------
VWebProxyOutPolicy::VWebProxyOutPolicy()
{
  method = Auto;
  host   = "";
  port   = 0;
}

void VWebProxyOutPolicy::load(VXml xml)
{
  method = (Method)xml.getInt("method", (int)method);
  host   = xml.getStr("host", host);
  port   = xml.getInt("port", port);
}

void VWebProxyOutPolicy::save(VXml xml)
{
  xml.setInt("method", (int)method);
  xml.setStr("host", host);
  xml.setInt("port", port);
}

#ifdef QT_GUI_LIB
void VWebProxyOutPolicy::optionAddWidget(QLayout* layout)
{
  QStringList sl; sl << "Auto" << "HTTP" << "HTTPS";
  VOptionable::addComboBox(layout, "cbxMethod", "Method", sl, (int)method);
  VOptionable::addLineEdit(layout, "leHost", "Host", host);
  VOptionable::addLineEdit(layout, "lePort", "Port", QString::number(port));
}

void VWebProxyOutPolicy::optionSaveDlg(QDialog* dialog)
{
  method = (Method)(dialog->findChild<QComboBox*>("cbxMethod")->currentIndex());
  host   = dialog->findChild<QLineEdit*>("leHost")->text();
  port   = dialog->findChild<QLineEdit*>("lePort")->text().toInt();
}
#endif // QT_GUI_LIB

// ----------------------------------------------------------------------------
// VWebProxyConnection
// ----------------------------------------------------------------------------
VWebProxyConnection::VWebProxyConnection(VNetSession* inSession, VNetClient* outClient)
{
  this->inSession      = inSession;
  this->outClient      = outClient;
  this->lastAccessTick = tick();
}

// ----- gilgil temp 2014.04.08 -----
/*
bool VWebProxyConnection::operator==(const VWebProxyConnection& rhs)
{
  if (this->inSession != rhs.inSession) return false;
  if (this->outClient != rhs.outClient) return false;
  return true;
}
*/
// ----------------------------------

// ----------------------------------------------------------------------------
// VWebProxyKeepAliveThread
// ----------------------------------------------------------------------------
VWebProxyKeepAliveThread::VWebProxyKeepAliveThread(void* owner) : VThread(owner)
{
  event.resetEvent();
}

VWebProxyKeepAliveThread::~VWebProxyKeepAliveThread()
{
  event.setEvent();
  close();
}

void VWebProxyKeepAliveThread::run()
{
  VWebProxy* webProxy = (VWebProxy*)owner;
  LOG_ASSERT(webProxy != NULL);

  while (true)
  {
    bool res = event.wait(10000); // 10 sec
    if (res) break;
    VTick nowTick = tick();
    webProxy->connections.lock();
    for (VWebProxyConnections::iterator it = webProxy->connections.begin(); it != webProxy->connections.end(); it++)
    {
      VWebProxyConnection* connection = *it;
      if (connection->lastAccessTick + webProxy->keepAliveTimeout < nowTick)
      {
        connection->inSession->close();
        connection->outClient->close();
      }
    }
    webProxy->connections.unlock();
  }
}

// ----------------------------------------------------------------------------
// VWebProxyOutInThread
// ----------------------------------------------------------------------------
VWebProxyOutInThread::VWebProxyOutInThread(VWebProxyConnection* connection, void* owner) : VThread(owner)
{
  this->webProxy      = (VWebProxy*)owner;
  this->connection    = connection;
  closeInSessionOnEnd = true;
}

VWebProxyOutInThread::~VWebProxyOutInThread()
{
  close();
}

void VWebProxyOutInThread::run()
{
  // LOG_DEBUG("stt"); // gilgil temp 2014.03.14

  VNetClient* outClient  = connection->outClient;
  VNetSession* inSession = connection->inSession;

  QByteArray              buffer;
  VWebProxySessionStatus status = HeaderCaching;
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
        sendBuffer = webProxy->flushResponseHeaderBody(response, buffer, connection);
      } else
      {
        sendBuffer = webProxy->flushResponseBuffer(buffer, connection);
      }
      if (sendBuffer != "")
      {
        inSession->write(sendBuffer);
      }
      break;
    }
    // LOG_DEBUG("%s", qPrintable(oneBuffer)); // gilgil temp 2014.04.10
    connection->lastAccessTick = tick();
    buffer += oneBuffer;

    //
    // HeaderCaching
    //
    if (status == HeaderCaching)
    {
      if (response.parse(buffer)) // header parsing completed
      {
        contentLength = response.header.value("Content-Length").toInt();
        if (contentLength > 0)
        {
          status = ContentCaching;
          // LOG_DEBUG("ContentCaching"); // gilgil temp 2014.04.10
        } else
        if (response.header.value("Transfer-Encoding").toLower() == "chunked")
        {
          sendBuffer += webProxy->flushResponseHeader(response, connection);
          status = Chunking;
          // LOG_DEBUG("Chunking"); // gilgil temp 2014.04.10
        } else
        {
          sendBuffer += webProxy->flushResponseHeaderBody(response, buffer, connection);
          // status = Streaming; // by gilgil 2014.04.17
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
        sendBuffer += webProxy->flushResponseHeaderBody(response, buffer, connection);
        status = HeaderCaching;
        // LOG_DEBUG("HeaderCaching"); // gilgil temp 2014.04.10
      }
      if (length > webProxy->maxContentCacheSize)
      {
        LOG_DEBUG("length(%d) is bigger than maxContentCacheSize(%d)", length, webProxy->maxContentCacheSize);
        sendBuffer += webProxy->flushResponseHeaderBody(response, buffer, connection);
        status = Streaming;
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
        sendBuffer += webProxy->flushResponseChunkBody(chunkBody, connection);
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
        sendBuffer += webProxy->flushResponseBuffer(buffer, connection);
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

  outClient->close();
  if (closeInSessionOnEnd) inSession->close();

  // LOG_DEBUG("end %d"); // gilgil temp 2014.03.14
}

// ----------------------------------------------------------------------------
// VWebProxy
// ----------------------------------------------------------------------------
VWebProxy::VWebProxy(void* owner) : VObject(owner)
{
  enabled                   = true;
  httpEnabled               = true;
  httpsEnabled              = true;
  maxContentCacheSize       = 1048576; // 1MByte
  disableLoopbackConnection = true;
  keepAliveTimeout          = 60000; // 60 sec
  outInThreadTimeout        = 30000; // 30 sec
  tcpServer.port            = HTTP_PROXY_PORT;
  sslServer.port            = SSL_PROXY_PORT;

  VDataChangeItem changeItem;
  //
  // Accept-Encoding
  //
  changeItem.pattern = "\r\nAccept-Encoding: ";
  changeItem.syntax  = QRegExp::FixedString;
  changeItem.cs      = Qt::CaseSensitive;
  changeItem.enabled = true;
  changeItem.log     = false;
  changeItem.replace = "\r\nAccept-Encoding-SS: ";
  outboundDataChange.push_back(changeItem);

  //
  // If-Modified-Since
  //
  changeItem.pattern = "\r\nIf-Modified-Since:[^\r\n]*";
  changeItem.syntax  = QRegExp::RegExp;
  changeItem.cs      = Qt::CaseSensitive;
  changeItem.enabled = true;
  changeItem.log     = false;
  changeItem.replace = "\r\nIf-Modified-Since-SS: ";
  outboundDataChange.push_back(changeItem);

  //
  // If-None-Match
  //
  changeItem.pattern = "\r\nIf-None-Match:[^\r\n]*";
  changeItem.syntax  = QRegExp::RegExp;
  changeItem.cs      = Qt::CaseSensitive;
  changeItem.enabled = true;
  changeItem.log     = false;
  changeItem.replace = "\r\nIf-None-Match-SS: ";
  outboundDataChange.push_back(changeItem);

  keepAliveThread     = NULL;

  VObject::connect(&tcpServer, SIGNAL(runned(VTcpSession*)), this, SLOT(tcpRun(VTcpSession*)), Qt::DirectConnection);
  VObject::connect(&sslServer, SIGNAL(runned(VSslSession*)), this, SLOT(sslRun(VSslSession*)), Qt::DirectConnection);
}

VWebProxy::~VWebProxy()
{
  close();
}

bool VWebProxy::doOpen()
{
  if (!enabled)
  {
    LOG_DEBUG("enabled is false");
    return true;
  }

  if (httpEnabled)
  {
    if (!tcpServer.open())
    {
      error = tcpServer.error;
      return false;
    }
  }

  if (httpsEnabled)
  {
    if (!sslServer.open())
    {
      error = sslServer.error;
      return false;
    }
  }

  keepAliveThread = new VWebProxyKeepAliveThread(this);
  keepAliveThread->open();

  if (!inboundDataChange.prepare(error)) return false;
  if (!outboundDataChange.prepare(error)) return false;

  return true;
}

bool VWebProxy::doClose()
{
  if (!enabled)
  {
    LOG_DEBUG("enabled is false");
    return true;
  }

  connections.lock();
  for (VWebProxyConnections::iterator it = connections.begin(); it != connections.end(); it++)
  {
    VWebProxyConnection* connection = *it;
    connection->inSession->close();
    connection->outClient->close();
  }
  connections.unlock();

  tcpServer.close();
  sslServer.close();

  SAFE_DELETE(keepAliveThread);

  return true;
}

void VWebProxy::tcpRun(VTcpSession* tcpSession)
{
  run(tcpSession);
}

void VWebProxy::sslRun(VSslSession* sslSession)
{
  run(sslSession);
}

bool VWebProxy::determineHostAndPort(VHttpRequest& request, int defaultPort, QString& host, int& port)
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

QByteArray VWebProxy::flushRequestHeader(VHttpRequest& request, VWebProxyConnection* connection)
{
  QByteArray headerData = request.toByteArray();
  outboundDataChange.change(headerData);
  emit onHttpRequestHeader(&request, connection);
  request.parse(headerData);

  QByteArray res = request.toByteArray();
  request.clear();
  return res;
}

QByteArray VWebProxy::flushRequestHeaderBody(VHttpRequest& request, QByteArray& body, VWebProxyConnection* connection)
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

QByteArray VWebProxy::flushRequestChunkBody(VHttpChunkBody& chunkBody, VWebProxyConnection* connection)
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

QByteArray VWebProxy::flushRequestBuffer(QByteArray& buffer, VWebProxyConnection* connection)
{
  outboundDataChange.change(buffer);
  emit onHttpRequestBody(&buffer, connection);
  QByteArray res = buffer;
  buffer = "";
  return res;
}

QByteArray VWebProxy::flushResponseHeader(VHttpResponse& response, VWebProxyConnection* connection)
{
  QByteArray headerData = response.toByteArray();
  inboundDataChange.change(headerData);
  emit onHttpResponseHeader(&response, connection);
  response.parse(headerData);

  QByteArray res = response.toByteArray();
  response.clear();
  return res;
}

QByteArray VWebProxy::flushResponseHeaderBody(VHttpResponse& response, QByteArray& body, VWebProxyConnection* connection)
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
    LOG_DEBUG("content length change from %d > %d (%d)", oldBodyLen, newBodyLen, newBodyLen - oldBodyLen);
  }

  QByteArray res = response.toByteArray() + body;
  response.clear();
  body.clear();
  return res;
}

QByteArray VWebProxy::flushResponseChunkBody(VHttpChunkBody& chunkBody, VWebProxyConnection* connection)
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

QByteArray VWebProxy::flushResponseBuffer(QByteArray& buffer, VWebProxyConnection* connection)
{
  inboundDataChange.change(buffer);
  emit onHttpResponseBody(&buffer, connection);
  QByteArray res = buffer;
  buffer = "";
  return res;
}

void VWebProxy::run(VNetSession* inSession)
{
  // LOG_DEBUG("stt inSession=%p", inSession); // gilgil temp 2014.03.14

  VNetClient* outClient = NULL;
  int defaultPort;

  //
  // determine outClient and defaultPort
  //
  switch (outPolicy.method)
  {
    case VWebProxyOutPolicy::Auto:
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
    case VWebProxyOutPolicy::Http:
      outClient   = new VTcpClient;
      defaultPort = DEFAULT_HTTP_PORT;
      break;
    case VWebProxyOutPolicy::Https:
      outClient   = new VSslClient;
      defaultPort = DEFAULT_SSL_PORT;
      break;
    default:
      LOG_FATAL("invalid method value(%d)", (int)outPolicy.method);
      return;
  }
  connections.lock();
  VWebProxyConnection connection(inSession, outClient);
  connections.push_back(&connection);
  connections.unlock();

  QByteArray              buffer;
  VWebProxySessionStatus status = HeaderCaching;
  VHttpRequest            request;
  int                     contentLength = 0;
  VWebProxyOutInThread*  thread = NULL;

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
    // ----- gilgil temp 2014.04.15 -----
    if (buffer.startsWith(("HEAD")))
    {
      LOG_DEBUG("%s", qPrintable(buffer));
    }
    // ----------------------------------

    //
    // HeaderCaching
    //
    if (status == HeaderCaching)
    {
      if (request.parse(buffer)) // header parsing completed
      {
        // ----- gilgil temp 2014.04.12 -----
        // call outboundDataChange.change before determine host and port
        QByteArray headerData = request.toByteArray();
        outboundDataChange.change(headerData);
        request.parse(headerData);
        // ----------------------------------

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
          if (thread != NULL) thread->closeInSessionOnEnd = false;
          outClient->close();
          if (thread != NULL)
          {
            thread->close(true, outInThreadTimeout);
            delete thread;
            thread = NULL;
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
          thread = new VWebProxyOutInThread(&connection, this);
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
          sendBuffer += flushRequestHeaderBody(request, buffer, &connection);
          // status = Streaming; // by gilgil 2014.04.17
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
      if (length > maxContentCacheSize)
      {
        LOG_DEBUG("length(%d) is bigger than maxContentCacheSize(%d)", length, maxContentCacheSize);
        sendBuffer += flushRequestHeaderBody(request, buffer, &connection);
        status = Streaming;
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
    thread = NULL;
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

void VWebProxy::load(VXml xml)
{
  VObject::load(xml);

  enabled                   = xml.getBool("enabled",                   enabled);
  httpEnabled               = xml.getBool("httpEnabled",               httpEnabled);
  httpsEnabled              = xml.getBool("httpsEnabled",              httpsEnabled);
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

void VWebProxy::save(VXml xml)
{
  VObject::save(xml);

  xml.setBool("enabled",                   enabled);
  xml.setBool("httpEnabled",               httpEnabled);
  xml.setBool("httpsEnabled",              httpsEnabled);
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
#include "vwebproxywidget.h"
#include "ui_vwebproxywidget.h"
void VWebProxy::optionAddWidget(QLayout* layout)
{
  VWebProxyWidget* widget = new VWebProxyWidget(layout->parentWidget());
  widget->setObjectName("webProxyWidget");

  VOptionable::addCheckBox(widget->ui->glHttpServer,  "chkHttpEnabled",  "HTTP Enabled",  httpEnabled);
  VOptionable::addCheckBox(widget->ui->glHttpsServer, "chkHttpsEnabled", "HTTPS Enabled", httpsEnabled);

  outPolicy.optionAddWidget(widget->ui->glExternal);
  tcpServer.optionAddWidget(widget->ui->glHttpServer);
  sslServer.optionAddWidget(widget->ui->glHttpsServer);

  VOptionable::addCheckBox(widget->ui->glOther,     "chkEnabled",                   "Enabled",                     enabled);
  VOptionable::addLineEdit(widget->ui->glOther,     "leMaxContentCacheSize",        "Max Content Cache Size",      QString::number(maxContentCacheSize));
  VOptionable::addCheckBox(widget->ui->glOther,     "chkDisableLoopbackConnection", "Disable Loopback Connection", disableLoopbackConnection);
  VOptionable::addLineEdit(widget->ui->glOther,     "leKeepAliveTimeout",           "KeepAlive Timeout",           QString::number(keepAliveTimeout));
  VOptionable::addLineEdit(widget->ui->glOther,     "leOutInThreadTimeout",         "OutIn Thread Timeout",        QString::number(outInThreadTimeout));

  inboundDataChange.optionAddWidget(widget->ui->glInbound);
  outboundDataChange.optionAddWidget(widget->ui->glOutbound);

  layout->addWidget(widget);
}

void VWebProxy::optionSaveDlg(QDialog* dialog)
{
  VWebProxyWidget* widget = dialog->findChild<VWebProxyWidget*>("webProxyWidget");
  LOG_ASSERT(widget != NULL);

  httpEnabled  = widget->findChild<QCheckBox*>("chkHttpEnabled")->checkState() == Qt::Checked;
  httpsEnabled = widget->findChild<QCheckBox*>("chkHttpsEnabled")->checkState() == Qt::Checked;

  outPolicy.optionSaveDlg((QDialog*)widget->ui->tabExternal);
  tcpServer.optionSaveDlg((QDialog*)widget->ui->tabHttpServer);
  sslServer.optionSaveDlg((QDialog*)widget->ui->tabHttpsServer);

  enabled                   = widget->findChild<QCheckBox*>("chkEnabled")->isChecked();
  maxContentCacheSize       = widget->findChild<QLineEdit*>("leMaxContentCacheSize")->text().toInt();
  disableLoopbackConnection = widget->findChild<QCheckBox*>("chkDisableLoopbackConnection")->checkState() == Qt::Checked;
  keepAliveTimeout          = widget->findChild<QLineEdit*>("leKeepAliveTimeout")->text().toInt();
  outInThreadTimeout        = widget->findChild<QLineEdit*>("leOutInThreadTimeout")->text().toInt();

  inboundDataChange.optionSaveDlg((QDialog*)widget->ui->tabInbound);
  outboundDataChange.optionSaveDlg((QDialog*)widget->ui->tabOutbound);
}
#endif // QT_GUI_LIB
