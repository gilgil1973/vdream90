#include <VUdpServer>
#include <VDebugNew>

REGISTER_METACLASS(VUdpServer, VNet)

// ----------------------------------------------------------------------------
// VSockAddrList
// ----------------------------------------------------------------------------
bool operator <(const SOCKADDR_IN& a, const SOCKADDR_IN& b)
{
  return memcmp((void*)&a, (void*)&b, sizeof(SOCKADDR_IN)) < 0;
}

VSockAddrList::iterator VSockAddrList::findBySockAddr(SOCKADDR_IN sockAddr)
{
  VLock lock(m_cs);
  VSockAddrList::iterator res = this->find(sockAddr);
  return res; 
}

VSockAddrList::iterator VSockAddrList::findByInAddr(IN_ADDR inAddr)
{
  VLock lock(m_cs);
  for (VSockAddrList::iterator it = begin(); it != end(); it++)
  {
    const SOCKADDR_IN& sockAddr = *it;
    if (sockAddr.sin_addr.s_addr == inAddr.s_addr) return it;
  }
  return end();
}

// ----------------------------------------------------------------------------
// VUdpServer
// ----------------------------------------------------------------------------
VUdpServer::VUdpServer(void* owner) : VNetServer(owner)
{
  udpSession = new VUdpSession(this);
  port = 0;
  localHost = "";
}

VUdpServer::~VUdpServer()
{
  close();
  SAFE_DELETE(udpSession);
}

bool VUdpServer::doOpen()
{
  VLock lock(m_openCloseCS);

  if (port == 0)
  {
    SET_ERROR(VNetError, "port is zero", VERR_PORT_IS_ZERO);
    return false;
  }

  // ------------------------------------------------------------------------
  // socket
  // ------------------------------------------------------------------------
  udpSession->handle = ::socket(AF_INET, SOCK_DGRAM, 0);
  if (udpSession->handle == INVALID_SOCKET)
    {
    SET_ERROR(VSocketError, "error in socket", WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // bind
  // ------------------------------------------------------------------------
  udpSession->addr.sin_family = AF_INET;
  udpSession->addr.sin_port = htons(quint16(port));
  if (localHost == "")
  {
    udpSession->addr.sin_addr.s_addr = INADDR_ANY;
  } else
  {
    Ip ip = VNet::resolve(localHost);
    if (ip == 0)
    {
      SET_ERROR(VNetError, qformat("can not resolve host(%s)", qPrintable(localHost)), VERR_CAN_NOT_RESOLVE_HOST);
      return false;
    }
    udpSession->addr.sin_addr.s_addr = htonl(ip);
  }
  memset(&udpSession->addr.sin_zero, 0, sizeof(udpSession->addr.sin_zero));

  int res = ::bind(udpSession->handle, (SOCKADDR*)&udpSession->addr, sizeof(udpSession->addr));
  if (res == SOCKET_ERROR)
  {
    SET_ERROR(VSocketError, qformat("error in bind(%s:%d)", qPrintable(localHost), port), WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // ok
  // ------------------------------------------------------------------------
  if (!udpSession->open()) return false;

  return true;
}

bool VUdpServer::doClose()
{
  VLock lock(m_openCloseCS);
  sockAddrList.lock();
  sockAddrList.clear();
  sockAddrList.unlock();
  udpSession->close();
  return true;
}

int VUdpServer::doRead(char* buf, int size)
{
  VLock lock(m_readCS);
  return udpSession->read(buf, size);
  SOCKADDR_IN sockAddr = udpSession->addr;
  sockAddrList.insert(sockAddr);
}

int VUdpServer::doWrite(char* buf, int size)
{
  VLock lock(m_writeCS);

  sockAddrList.lock();
  for (VSockAddrList::iterator it = sockAddrList.begin(); it != sockAddrList.end(); it++)
  {
    const SOCKADDR_IN& sockAddr = *it;
    udpSession->addr = sockAddr;
    udpSession->write(buf, size); // do not check result
  }
  sockAddrList.unlock();
  return size;
}

void VUdpServer::load(VXml xml)
{
  VNetServer::load(xml);

  port = xml.getInt("port", port);
  localHost = xml.getStr("localHost", localHost);
}

void VUdpServer::save(VXml xml)
{
  VNetServer::save(xml);

  xml.setInt("port", port);
  xml.setStr("localHost", localHost);
}

#ifdef QT_GUI_LIB
void VUdpServer::optionAddWidget(QLayout* layout)
{
  VNetServer::optionAddWidget(layout);

  VOptionable::addLineEdit(layout, "lePort",      "Port",       QString::number(port));
  VOptionable::addLineEdit(layout, "leLocalHost", "Local Host", localHost);
}

void VUdpServer::optionSaveDlg(QDialog* dialog)
{
  VNetServer::optionSaveDlg(dialog);

  port      = dialog->findChild<QLineEdit*>("lePort")->text().toInt();
  localHost = dialog->findChild<QLineEdit*>("leLocalHost")->text();
}
#endif // QT_GUI_LIB
