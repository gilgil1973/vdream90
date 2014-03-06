#include <VHttpProxy>
#include <VDebugNew>

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
  VNetClient*  outClient;
  VNetSession* inSession;

public:
  VHttpProxyOutInThread(VNetClient* outClient, VNetSession* inSession, void* owner) : VThread(owner)
  {
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
    VHttpProxy* proxy = (VHttpProxy*)owner;
    LOG_DEBUG("stt"); // gilgil temp 2013.10.19
    while (true)
    {
      QByteArray msg;
      int readLen = outClient->read(msg);
      if (readLen == VERR_FAIL) break;
      emit proxy->beforeResponse(msg, outClient, inSession);
      int writeLen = inSession->write(msg);
      if (writeLen == VERR_FAIL) break;
     }
    inSession->close();
    outClient->close();
    LOG_DEBUG("end"); // gilgil temp 2013.10.19
  }
};

// ----------------------------------------------------------------------------
// VHttpProxy
// ----------------------------------------------------------------------------
VHttpProxy::VHttpProxy(void* owner) : VObject(owner)
{
  tcpEnabled     = true;
  sslEnabled     = false;

  tcpServer.port = HTTP_PROXY_PORT;
  sslServer.port = SSL_PROXY_PORT;

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

  return true;
}

bool VHttpProxy::doClose()
{
  tcpServer.close();
  sslServer.close();

  return true;
}

void VHttpProxy::load(VXml xml)
{
  tcpEnabled = xml.getBool("tcpEnabled", tcpEnabled);
  sslEnabled = xml.getBool("sslEnabled", sslEnabled);
  policy.load(xml.gotoChild("policy"));
  tcpServer.load(xml.gotoChild("tcpServer"));
  sslServer.load(xml.gotoChild("sslServer"));
}

void VHttpProxy::save(VXml xml)
{
  xml.setBool("tcpEnabled", tcpEnabled);
  xml.setBool("sslEnabled", sslEnabled);
  policy.save(xml.gotoChild("policy"));
  tcpServer.save(xml.gotoChild("tcpServer"));
  sslServer.save(xml.gotoChild("sslServer"));
}

void VHttpProxy::tcpRun(VTcpSession* tcpSession)
{
  run(tcpSession);
}

void VHttpProxy::sslRun(VSslSession* sslSession)
{
  run(sslSession);
}

void VHttpProxy::run(VNetSession* inSession)
{
  LOG_DEBUG("stt"); // gilgil temp 2013.10.19

  VNetClient* outClient = NULL;
  int defaultOutPort;
  switch (policy.method)
  {
    case VHttpProxyOutPolicy::Auto:
      if (dynamic_cast<VTcpSession*>(inSession) != NULL)
      {
        outClient      = new VTcpClient;
        defaultOutPort = DEFAULT_HTTP_PORT;
      } else
      if (dynamic_cast<VSslSession*>(inSession) != NULL)
      {
        outClient      = new VSslClient;
        defaultOutPort = DEFAULT_SSL_PORT;
      } else
      {
        LOG_FATAL("invalid inSession type(%s)", qPrintable(inSession->className()));
        return;
      }
      break;
    case VHttpProxyOutPolicy::Tcp:
      outClient      = new VTcpClient;
      defaultOutPort = DEFAULT_HTTP_PORT;
      break;
    case VHttpProxyOutPolicy::Ssl:
      outClient      = new VSslClient;
      defaultOutPort = DEFAULT_SSL_PORT;
      break;
    default:
      LOG_FATAL("invalid clientMethod value(%d)", (int)policy.method);
      return;
  }

  VHttpRequest           request;
  VHttpProxyOutInThread* thread = NULL;

  QByteArray totalPacket;
  while (true)
  {
    QByteArray packet;
    int readLen = inSession->read(packet);
    if (readLen == VERR_FAIL) break;
    // LOG_DEBUG("%s", packet.data()); // gilgil temp

    totalPacket += packet;
    if (!request.parse(totalPacket))
    {
      if (outClient->active())
      {
        outClient->write(totalPacket);
        totalPacket = "";
      }
      continue;
    }

    QString host;
    int port;
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
      LOG_ERROR("can not find host:%s", totalPacket.data());
      break;
    }
    if (port == -1) port = defaultOutPort;
    if (policy.host != "") host = policy.host;
    if (policy.port != 0)  port = policy.port;

    if (outClient->host != host || outClient->port != port)
    {
      outClient->close();
      if (thread != NULL) delete thread;

      outClient->host = host;
      outClient->port = port;
      if (!outClient->open())
      {
        LOG_ERROR("%s", outClient->error.msg);
        break;
      }
      thread = new VHttpProxyOutInThread(outClient, inSession, this);
      thread->open();
    }

    emit beforeRequest(request, inSession, outClient);
    outClient->write(request.toByteArray());
    totalPacket = "";
  }
  LOG_DEBUG("end"); // gilgil temp 2013.10.19
  outClient->close();
  if (thread != NULL) delete thread;
  delete outClient;
}

#ifdef QT_GUI_LIB
#include "vhttpproxywidget.h"
#include "ui_vhttpproxywidget.h"
void VHttpProxy::optionAddWidget(QLayout* layout)
{
  VHttpProxyWidget* widget = new VHttpProxyWidget(layout->parentWidget());
  widget->setObjectName("httpProcyWidget");

  VOptionable::addCheckBox(widget->ui->glCommon, "chkTcpEnabled", "TCP Enabled", tcpEnabled);
  VOptionable::addCheckBox(widget->ui->glCommon, "chkSslEnabled", "SSL Enabled", sslEnabled);

  policy.optionAddWidget(widget->ui->glPolicy);
  tcpServer.optionAddWidget(widget->ui->glTcpServer);
  sslServer.optionAddWidget(widget->ui->glSslServer);

  layout->addWidget(widget);
}

void VHttpProxy::optionSaveDlg(QDialog* dialog)
{
  VHttpProxyWidget* widget = dialog->findChild<VHttpProxyWidget*>("httpProcyWidget");
  LOG_ASSERT(widget != NULL);

  tcpEnabled = widget->findChild<QCheckBox*>("chkTcpEnabled")->checkState() == Qt::Checked;
  sslEnabled = widget->findChild<QCheckBox*>("chkSslEnabled")->checkState() == Qt::Checked;

  policy.optionSaveDlg((QDialog*)widget->ui->tabPolicy);
  tcpServer.optionSaveDlg((QDialog*)widget->ui->tabTcpServer);
  sslServer.optionSaveDlg((QDialog*)widget->ui->tabSslServer);
}
#endif // QT_GUI_LIB

