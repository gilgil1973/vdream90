#include <VHttpProxy>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VHttpProxyOutInThread
// ----------------------------------------------------------------------------
class VHttpProxyOutInThread : public VThread
{
protected:
  VTcpClient*  outClient;
  VTcpSession* inSession;

public:
  VHttpProxyOutInThread(VTcpClient* outClient, VTcpSession* inSession, void* owner) : VThread(owner)
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
      int readLen = outClient->tcpSession->read(msg);
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
  tcpServer.port = PORT;
  VObject::connect(&tcpServer, SIGNAL(runned(VTcpSession*)), this, SLOT(run(VTcpSession*)), Qt::DirectConnection);
  autoOpen = true;
}

VHttpProxy::~VHttpProxy()
{
  close();
}

bool VHttpProxy::doOpen()
{
  if (!tcpServer.open())
  {
    error = tcpServer.error;
    return false;
  }
  return true;
}

bool VHttpProxy::doClose()
{
  tcpServer.close();

  return true;
}

void VHttpProxy::load(VXml xml)
{
  tcpServer.load(xml.gotoChild("tcpServer"));
  autoOpen = xml.getBool("autoOpen", autoOpen);
}

void VHttpProxy::save(VXml xml)
{
  tcpServer.save(xml.gotoChild("tcpServer"));
  xml.setBool("autoOpen", autoOpen);
}

void VHttpProxy::run(VTcpSession* tcpSession)
{
  LOG_DEBUG("stt"); // gilgil temp 2013.10.19
  VTcpSession*          inSession = tcpSession;
  VHttpRequest           request;
  VTcpClient            outClient;
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
      if (outClient.active())
      {
        outClient.tcpSession->write(totalPacket);
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
      if (port == -1) port = 80;

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

    if (outClient.host != host || outClient.port != port)
    {
      outClient.close();
      if (thread != NULL) delete thread;

      outClient.host = host;
      outClient.port = port;
      if (!outClient.open())
      {
        LOG_ERROR("%s", outClient.error.msg);
        break;
      }
      thread = new VHttpProxyOutInThread(&outClient, inSession, this);
      thread->open();
    }

    emit beforeRequest(request, inSession, &outClient);
    outClient.tcpSession->write(request.toByteArray());
    totalPacket = "";
  }
  LOG_DEBUG("end"); // gilgil temp 2013.10.19
  outClient.close();
  if (thread != NULL) delete thread;
}
