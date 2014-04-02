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

    QByteArray             buffer;
    VHttpProxyRecvStatus   status = HeaderCaching;
    VHttpResponse          response;
    int                    contentLength   = 0;

    while (true)
    {
      //
      // read oneBuffer from inSession
      //
      QByteArray oneBuffer;
      int readLen = outClient->read(oneBuffer);
      if (readLen == VERR_FAIL) break;

      buffer += oneBuffer;

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

      //
      // BodyCaching
      //
      if (status == BodyCaching)
      {
        int length = buffer.length();
        // LOG_DEBUG("length=%d contentLength=%d", length, contentLength); // gilgil temp 2014.04.02
        if (length != 0)
        {
          if (length == contentLength) // body completed
          {
            if (!httpProxy->flushHttpResponseHeaderAndBody(response, buffer, outClient, inSession)) break;
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
          httpProxy->inboundDataChange.change(buffer);
          emit httpProxy->onHttpResponseBody(&response, &buffer, outClient, inSession);
          if (inSession->write(buffer) == VERR_FAIL) break;
          buffer = "";
        }
      }
    }

    // sleep(5); // gilgil temp 2014.04.02
    outClient->close();
    inSession->close();
    // LOG_DEBUG("end"); // gilgil temp 2014.03.14
  }
};

// ----------------------------------------------------------------------------
// VHttpProxy
// ----------------------------------------------------------------------------
VHttpProxy::VHttpProxy(void* owner) : VObject(owner)
{
  tcpEnabled          = true;
  sslEnabled          = true;
  maxContentCacheSize = 10485756; // 1MByte
  tcpServer.port      = HTTP_PROXY_PORT;
  sslServer.port      = SSL_PROXY_PORT;

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

  if (!inboundDataChange.prepare(error)) return false;
  if (!outboundDataChange.prepare(error)) return false;

  return true;
}

bool VHttpProxy::doClose()
{
  tcpServer.close();
  sslServer.close();

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
  QByteArray headerData = request.toByteArray();
  outboundDataChange.change(headerData);
  emit onHttpRequestHeader(&request, inSession, outClient);
  request.parse(headerData, NULL);

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
  if (thread != NULL) delete thread;
  delete outClient;
}

void VHttpProxy::load(VXml xml)
{
  VObject::load(xml);

  tcpEnabled          = xml.getBool("tcpEnabled", tcpEnabled);
  sslEnabled          = xml.getBool("sslEnabled", sslEnabled);
  maxContentCacheSize = xml.getInt("maxContentCacheSize", maxContentCacheSize);
  outPolicy.load(xml.gotoChild("outPolicy"));
  tcpServer.load(xml.gotoChild("tcpServer"));
  sslServer.load(xml.gotoChild("sslServer"));
  inboundDataChange.load(xml.gotoChild("inboundDataChange"));
  outboundDataChange.load(xml.gotoChild("outboundDataChange"));
}

void VHttpProxy::save(VXml xml)
{
  VObject::save(xml);

  xml.setBool("tcpEnabled", tcpEnabled);
  xml.setBool("sslEnabled", sslEnabled);
  xml.setInt("maxContentCacheSize", maxContentCacheSize);
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
  widget->setObjectName("httpProcyWidget");

  VOptionable::addCheckBox(widget->ui->glTcpServer, "chkTcpEnabled", "TCP Enabled", tcpEnabled);
  VOptionable::addCheckBox(widget->ui->glSslServer, "chkSslEnabled", "SSL Enabled", sslEnabled);
  VOptionable::addLineEdit(widget->ui->glOther,     "leMaxContentCacheSize", "Max Content Cache Size", QString::number(maxContentCacheSize));

  outPolicy.optionAddWidget(widget->ui->glExternal);
  tcpServer.optionAddWidget(widget->ui->glTcpServer);
  sslServer.optionAddWidget(widget->ui->glSslServer);
  inboundDataChange.optionAddWidget(widget->ui->glInbound);
  outboundDataChange.optionAddWidget(widget->ui->glOutbound);

  layout->addWidget(widget);
}

void VHttpProxy::optionSaveDlg(QDialog* dialog)
{
  VHttpProxyWidget* widget = dialog->findChild<VHttpProxyWidget*>("httpProcyWidget");
  LOG_ASSERT(widget != NULL);

  tcpEnabled = widget->findChild<QCheckBox*>("chkTcpEnabled")->checkState() == Qt::Checked;
  sslEnabled = widget->findChild<QCheckBox*>("chkSslEnabled")->checkState() == Qt::Checked;
  maxContentCacheSize = widget->findChild<QLineEdit*>("leMaxContentCacheSize")->text().toInt();

  outPolicy.optionSaveDlg((QDialog*)widget->ui->tabExternal);
  tcpServer.optionSaveDlg((QDialog*)widget->ui->tabTcpServer);
  sslServer.optionSaveDlg((QDialog*)widget->ui->tabSslServer);
  inboundDataChange.optionSaveDlg((QDialog*)widget->ui->tabInbound);
  outboundDataChange.optionSaveDlg((QDialog*)widget->ui->tabOutbound);
}
#endif // QT_GUI_LIB
