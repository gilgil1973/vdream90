#include <VLogHttp>
#include <VDebugNew>

REGISTER_METACLASS(VLogHttp, VLog)

// ----------------------------------------------------------------------------
// VLogHttp
// ----------------------------------------------------------------------------
const int VLogHttp::DEFAULT_PORT = 8908;

VLogHttp::VLogHttp(const int port)
{
  showDateTime = VShowDateTime::Time;
  this->port = port;
  m_tcpServer = new VLogHttpTcpServer;

  open();
}

VLogHttp::~VLogHttp()
{
  close();
  SAFE_DELETE(m_tcpServer);
}

bool VLogHttp::open()
{
  m_tcpServer->port = port;
  return m_tcpServer->open();
}

bool VLogHttp::close()
{
  return m_tcpServer->close();
}

void VLogHttp::write(const char* buf, int len)
{
  m_tcpServer->write(buf, len);
  m_tcpServer->write("\r\n");
}

VLog* VLogHttp::createByURI(const QString& uri)
{
  QUrl _url(uri);
  if (uri == "http" || _url.scheme() == "http")
  {
    int port = _url.port();
    if (port == 0 || port == -1) port = DEFAULT_PORT;

    VLogHttp* logHTTP= new VLogHttp(port);
    return logHTTP;
  }
  return NULL;
}

void VLogHttp::load(VXml xml)
{
  VLog::load(xml);

  port = xml.getInt("port", port);
  close();
  open();
}

void VLogHttp::save(VXml xml)
{
  VLog::save(xml);

  xml.setInt("port", port);
}


// ----------------------------------------------------------------------------
// VLogHttpTcpServer
// ----------------------------------------------------------------------------
VLogHttpTcpServer::VLogHttpTcpServer(void* owner) : VTcpServer(owner)
{
  if (!QObject::connect(this, SIGNAL(runned(VTcpSession*)), this, SLOT(run(VTcpSession*)), Qt::DirectConnection))
  //if (!QObject::connect(this, SIGNAL(runned(VTcpSession*)), this, SLOT(run(VTcpSession*))))
  {
    LOG_ERROR("connect return false");
  }
  // setLog(NULL); // gilgil temp 2012.11.01
}

VLogHttpTcpServer::~VLogHttpTcpServer()
{
  close();
}

void VLogHttpTcpServer::run(VTcpSession* tcpSession)
{
  static QString httpResponse = \
"HTTP/1.1 200 OK\r\n"\
"Content-Type: text/html;charset=utf-8\r\n"\
"Connection: Keep-Alive\r\n"\
"\r\n"\
"<html><head><title>vlog</title></head><body><pre>";
  int size = httpResponse.size();
  tcpSession->write(qPrintable(httpResponse), size);

  while (true)
  {
    char buf[256];
    int readLen = tcpSession->read(buf, 256);
    if (readLen == VERR_FAIL) break;
  }
}
