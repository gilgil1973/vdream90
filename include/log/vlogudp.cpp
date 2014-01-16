#include <VLogUdp>
#include <VNet> // for Ip
#include <VDebugNew>

REGISTER_METACLASS(VLogUdp, VLog)

// ----------------------------------------------------------------------------
// VLogUdp
// ----------------------------------------------------------------------------
const char* VLogUdp::DEFAULT_HOST = "127.0.0.1";
const int   VLogUdp::DEFAULT_PORT = 8908;

VLogUdp::VLogUdp(const char* host, const int port)
{
  VNetInstance::instance();

  showDateTime = VShowDateTime::None;
  this->host   = host;
  this->port   = port;
  handle       = INVALID_SOCKET;
  memset(&addr, 0, sizeof(addr));

  open();
}

VLogUdp::~VLogUdp()
{
  close();
}

bool VLogUdp::open()
{
  // ------------------------------------------------------------------------
  // socket
  // ------------------------------------------------------------------------
  handle = socket(AF_INET, SOCK_DGRAM, 0);
  if (handle == INVALID_SOCKET)
  {
    return false;
  }

  // ------------------------------------------------------------------------
  // bind
  // ------------------------------------------------------------------------
  addr.sin_family = AF_INET;
  addr.sin_port = 0;
  addr.sin_addr.s_addr = INADDR_ANY;
  memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

  int res = bind(handle, (SOCKADDR*)&addr, sizeof(addr));
  if (res == SOCKET_ERROR)
  {
    return false;
  }

  // ------------------------------------------------------------------------
  // connect(nothing)
  // ------------------------------------------------------------------------
  addr.sin_family = AF_INET;
  addr.sin_port = htons(quint16(port));
  if (host == "")
  {
    return false;
  }
  Ip ip = VNet::resolve(host);
  if (ip == 0)
  {
    return false;
  }
  addr.sin_addr.s_addr = htonl(ip);
  memset(addr.sin_zero, 0, sizeof(addr.sin_zero));

  return true;
}

bool VLogUdp::close()
{
  //
  // shutdown
  //
  shutdown(handle, SD_BOTH);

  //
  // closesocket
  //
#ifdef WIN32
  ::closesocket(handle);
#endif // WIN32
#ifdef linux
  ::close(handle);
#endif // linux

  handle = INVALID_SOCKET;

  return true;
}

void VLogUdp::write(const char* buf, int len)
{
  ::sendto(handle, buf, len, 0, (SOCKADDR*)&addr, sizeof(addr));;
}

VLog* VLogUdp::createByURI(const QString& uri)
{
  QUrl _url(uri);
  if (uri == "udp" || _url.scheme() == "udp")
  {
    QString host = _url.host();
    if (host == "") host = DEFAULT_HOST;
    int port = _url.port();
    if (port == 0 || port == -1) port = DEFAULT_PORT;

    VLogUdp* logUDP = new VLogUdp(qPrintable(host), port);
    return logUDP;
  }
  return NULL;
}

void VLogUdp::load(VXml xml)
{
  VLog::load(xml);

  host = xml.getStr("host", host);
  port = xml.getInt("port", port);
  close();
  open();
}

void VLogUdp::save(VXml xml)
{
  VLog::save(xml);

  xml.setStr("host", host);
  xml.setInt("port", port);
}
