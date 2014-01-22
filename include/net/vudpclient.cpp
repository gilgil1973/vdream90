#include <VUdpClient>
#include <VDebugNew>

REGISTER_METACLASS(VUdpClient, VNet)

// ----------------------------------------------------------------------------
// VUdpClient
// ----------------------------------------------------------------------------
VUdpClient::VUdpClient(void* owner) : VNetClient(owner)
{
  udpSession = new VUdpSession(this);
  m_ip       = 0;
  host       = "";
  port       = 0;
  localHost  = "";
  localPort  = 0;
}

VUdpClient::~VUdpClient()
{
  close();
  SAFE_DELETE(udpSession);
}

bool VUdpClient::doOpen()
{
  if (port == 0)
  {
    SET_ERROR(VNetError, "port is zero", VERR_PORT_IS_ZERO);
    return false;
  }

  // ------------------------------------------------------------------------
  // socket
  // ------------------------------------------------------------------------
  udpSession->handle = socket(AF_INET, SOCK_DGRAM, 0);
  if (udpSession->handle == INVALID_SOCKET)
  {
    SET_ERROR(VSocketError, "error in socket", WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // bind
  // ------------------------------------------------------------------------
  udpSession->addr.sin_family = AF_INET;
  udpSession->addr.sin_port = htons(quint16(localPort));
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

  int res = bind(udpSession->handle, (SOCKADDR*)&udpSession->addr, sizeof(udpSession->addr));
  if (res == SOCKET_ERROR)
  {
    SET_ERROR(VSocketError, qformat("error in bind(%s:%d)", qPrintable(localHost), localPort), WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // connect(nothing)
  // ------------------------------------------------------------------------
  udpSession->addr.sin_family = AF_INET;
  udpSession->addr.sin_port = htons(quint16(port));
  if (host == "")
  {
    SET_ERROR(VNetError, "host is invalid", VERR_INVALID_HOST);
    return false;
  }
  m_ip = VNet::resolve(host);
  if (m_ip == 0)
  {
    SET_ERROR(VNetError, qformat("can not resolve host(%s)", qPrintable(host)), VERR_CAN_NOT_RESOLVE_HOST);
    return false;
  }
  udpSession->addr.sin_addr.s_addr = htonl(m_ip);
  memset(udpSession->addr.sin_zero, 0, sizeof(udpSession->addr.sin_zero));

  // ------------------------------------------------------------------------
  // ok
  // ------------------------------------------------------------------------
  udpSession->onceWriteSize = this->onceWriteSize;
  if (!udpSession->open()) return false;

  return true;
}

bool VUdpClient::doClose()
{
  udpSession->close();
  return true;
}

int  VUdpClient::doRead(char* buf, int size)
{
  int res = udpSession->read(buf, size);
  if (res == VERR_FAIL)
    error = udpSession->error;
  return res;
}

int  VUdpClient::doWrite(char* buf, int size)
{
  int res = udpSession->write(buf, size);
  if (res == VERR_FAIL)
    error = udpSession->error;
  return res;
}

void VUdpClient::load(VXml xml)
{
  VNetClient::load(xml);
  host = xml.getStr("host", host);
  port = xml.getInt("port", port);
  localHost = xml.getStr("localHost", localHost);
  localPort = xml.getInt("localPort", localPort);
}

void VUdpClient::save(VXml xml)
{
  VNetClient::save(xml);
  xml.setStr("host", host);
  xml.setInt("port", port);
  if (localHost != "") xml.setStr("localHost", localHost);
  if (localPort != 0)  xml.setInt("localPort", localPort);
}

#ifdef QT_GUI_LIB
void VUdpClient::addOptionWidget(QLayout* layout)
{
  VNetClient::addOptionWidget(layout);
  VOptionable::addLineEdit(layout, "leHost",      "Host",       host);
  VOptionable::addLineEdit(layout, "lePort",      "Port",       QString::number(port));
  VOptionable::addLineEdit(layout, "leLocalHost", "Local Host", localHost);
  VOptionable::addLineEdit(layout, "leLocalPort", "Local Port", QString::number(localPort));
}

void VUdpClient::saveOptionDlg(QDialog* dialog)
{
  VNetClient::saveOptionDlg(dialog);
  host      = dialog->findChild<QLineEdit*>("leHost")->text();
  port      = dialog->findChild<QLineEdit*>("lePort")->text().toInt();
  localHost = dialog->findChild<QLineEdit*>("leLocalHost")->text();
  localPort = dialog->findChild<QLineEdit*>("leLocalPort")->text().toInt();
}
#endif // QT_GUI_LIB
