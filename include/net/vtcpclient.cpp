#include <VTcpClient>
#include <VDebugNew>

REGISTER_METACLASS(VTcpClient, VNet)

// ----------------------------------------------------------------------------
// VTcpClient
// ----------------------------------------------------------------------------
VTcpClient::VTcpClient(void* owner) : VNetClient(owner)
{
  tcpSession = new VTcpSession(this);
}

VTcpClient::~VTcpClient()
{
  close();
  SAFE_DELETE(tcpSession);
}

bool VTcpClient::close()
{
  //
  // Hang might be occurred when calling open in one thread and calling close in other thread.
  // So call tcpSession->doClose forcibly preventing hang.
  //
  tcpSession->doClose();
  return VNetClient::close();
}

bool VTcpClient::doOpen()
{
  VLock lock(stateOpenCs); // gilgil temp 2014.03.14

  if (port == 0)
  {
    SET_ERROR(VNetError, "port is zero", VERR_PORT_IS_ZERO);
    return false;
  }

  // ------------------------------------------------------------------------
  // socket
  // ------------------------------------------------------------------------
  tcpSession->handle = socket(AF_INET, SOCK_STREAM, 0);
  if (tcpSession->handle == INVALID_SOCKET)
  {
    SET_ERROR(VSocketError, "error in socket", WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // bind
  // ------------------------------------------------------------------------
  tcpSession->addr.sin_family = AF_INET;
  tcpSession->addr.sin_port = htons(quint16(localPort));
  if (localHost == 0)
  {
    tcpSession->addr.sin_addr.s_addr = INADDR_ANY;
  } else
  {
    Ip ip = VNet::resolve(localHost);
    if (ip == 0)
    {
      SET_ERROR(VNetError, qformat("can not resolve host(%s)", qPrintable(localHost)), VERR_CAN_NOT_RESOLVE_HOST);
      return false;
    }
    tcpSession->addr.sin_addr.s_addr = htonl(ip);
  }
  memset(&tcpSession->addr.sin_zero, 0, sizeof(tcpSession->addr.sin_zero));

  int res = bind(tcpSession->handle, (SOCKADDR*)&tcpSession->addr, sizeof(tcpSession->addr));
  if (res == SOCKET_ERROR)
  {
    SET_ERROR(VSocketError, qformat("error in bind(%s:%d)", qPrintable(localHost), localPort), WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // connect
  // ------------------------------------------------------------------------
  tcpSession->addr.sin_family = AF_INET;
  tcpSession->addr.sin_port = htons(quint16(port));
  if (host == "")
  {
    SET_ERROR(VNetError, "host is invalid", VERR_INVALID_HOST);
    return false;
  }
  Ip ip = VNet::resolve(host);
  if (ip == 0)
  {
    SET_ERROR(VNetError, qformat("can not resolve host(%s)", qPrintable(host)), VERR_CAN_NOT_RESOLVE_HOST);
    return false;
  }
  tcpSession->addr.sin_addr.s_addr = htonl(ip);
  memset(tcpSession->addr.sin_zero, 0, sizeof(tcpSession->addr.sin_zero));
  res = ::connect(tcpSession->handle, (SOCKADDR*)&tcpSession->addr, sizeof(tcpSession->addr));
  if (res == SOCKET_ERROR)
  {
    int lastError = WSAGetLastError();
    SET_ERROR(VSocketError, qformat("error in connect(%s:%d)(%d)", qPrintable(host), port, lastError), lastError);
    return false;
  }

  // ------------------------------------------------------------------------
  // ok
  // ------------------------------------------------------------------------
  tcpSession->onceWriteSize = this->onceWriteSize;
  if (!tcpSession->open()) return false;

  return true;
}

bool VTcpClient::doClose()
{
  VLock lock(stateCloseCs); // gilgil temp 2014.03.14

  tcpSession->close();
  return true;
}

int  VTcpClient::doRead(char* buf, int size)
{
  // VLock lock(stateReadCs); // gilgil temp 2014.03.14

  int res = tcpSession->read(buf, size);
  if (res == VERR_FAIL)
    error = tcpSession->error;
  return res;
}

int  VTcpClient::doWrite(char* buf, int size)
{
  // VLock lock(stateWriteCs); // gilgil temp 2014.03.14

  int res = tcpSession->write(buf, size);
  if (res == VERR_FAIL)
    error = tcpSession->error;
  return res;
}

void VTcpClient::load(VXml xml)
{
  VNetClient::load(xml);
}

void VTcpClient::save(VXml xml)
{
  VNetClient::save(xml);
}

#ifdef QT_GUI_LIB
void VTcpClient::optionAddWidget(QLayout* layout)
{
  VNetClient::optionAddWidget(layout);
}

void VTcpClient::optionSaveDlg(QDialog* dialog)
{
  VNetClient::optionSaveDlg(dialog);
}
#endif // QT_GUI_LIB
