#include <VTcpSession>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VTcpSession
// ----------------------------------------------------------------------------
VTcpSession::VTcpSession(void* owner) : VNetSession(owner)
{
  handle = INVALID_SOCKET;
  memset(&addr, 0, sizeof(addr));
}

VTcpSession::~VTcpSession()
{
  close();
}

bool VTcpSession::doOpen()
{
  // VLock lock(m_openCloseCS); // gilgil temp 2014.02.28

  if (handle == INVALID_SOCKET)
  {
    SET_ERROR(VError, "handle is zero", VERR_HANDLE_IS_ZERO);
    return false;
  }
  return true;
}

bool VTcpSession::doClose()
{
  // VLock lock(m_openCloseCS); // gilgil temp 2014.02.28

  if (handle == INVALID_SOCKET) return true;

  //
  // shutdown
  //
  int res = ::shutdown(handle, SD_BOTH);
  if (res == SOCKET_ERROR)
  {
    SET_ERROR(VSocketError, "error in shutdown", WSAGetLastError());
  }

  //
  // closesocket
  //
#ifdef WIN32
  res = ::closesocket(handle);
#endif // WIN32
#ifdef linux
  res = ::close(handle);
#endif // linux
  if (res == SOCKET_ERROR)
  {
    SET_ERROR(VSocketError, "error in closesocket", WSAGetLastError());
  }

  handle = INVALID_SOCKET;
  return true;
}

int VTcpSession::doRead(char* buf, int size)
{
  VLock lock(m_readCS);

  int res = ::recv(handle, buf, size, 0);
  if (res == SOCKET_ERROR)
  {
    SET_DEBUG_ERROR(VSocketError, "error in recv", WSAGetLastError());
    return VERR_FAIL;
  }
  // sometimes, read length can be 0(zero), and check if return value is not zero.
  if (res == 0)
  {
    SET_DEBUG_ERROR(VNetError, "recv return zero", VERR_ERROR_IN_RECV);
    return VERR_FAIL;
  }
  return res;
}

int VTcpSession::doWrite(char* buf, int size)
{
  VLock lock(m_writeCS);

  int res;
  int restSize = size;

  while (true)
  {
    if (onceWriteSize!= 0 && restSize > onceWriteSize) restSize = onceWriteSize;
    res = ::send(handle, buf, restSize, 0);
    if (res == SOCKET_ERROR)
    {
      SET_ERROR(VSocketError, "error in send", WSAGetLastError());
      return VERR_FAIL;
    }
    buf += res;
    restSize -= res;
    if (restSize == 0) break;
  }
  return size;
}

Ip VTcpSession::getLocalIP()
{
  socklen_t size;
  SOCKADDR_IN sockAddr;

  if (handle == INVALID_SOCKET) return 0;
  size = sizeof(sockAddr);
  if (::getsockname(handle, (SOCKADDR*)&sockAddr, &size) != 0) return 0;
  Ip res = *((Ip*)&(sockAddr.sin_addr));
  res = ntohl(res);
  return res;
}

Ip VTcpSession::getRemoteIP()
{
  socklen_t size;
  SOCKADDR_IN sockAddr;

  if (handle == INVALID_SOCKET) return 0;
  size = sizeof(sockAddr);
  if (::getpeername(handle, (SOCKADDR*)&sockAddr, &size) != 0) return 0;
  Ip res = *((Ip*)&(sockAddr.sin_addr));
  res = ntohl(res);
  return res;
}

int VTcpSession::getLocalPort()
{
  socklen_t size;
  SOCKADDR_IN sockAddr;

  if (handle == INVALID_SOCKET) return 0;
  size = sizeof(sockAddr);
  if (::getsockname(handle, (SOCKADDR*)&sockAddr, &size) != 0) return 0;
  int res = ntohs(sockAddr.sin_port);
  return res;
}

int VTcpSession::getRemotePort()
{
  socklen_t size;
  SOCKADDR_IN sockAddr;

  if (handle == INVALID_SOCKET) return 0;
  size = sizeof(sockAddr);
  if (::getpeername(handle, (SOCKADDR*)&sockAddr, &size) != 0) return 0;
  int res = ntohs(sockAddr.sin_port);
  return res;
}

bool VTcpSession::operator == (const VTcpSession& rhs) const
{
  if (handle != rhs.handle) return false;
  if (memcmp(&addr, &rhs.addr, sizeof(addr)) != 0) return false;
  return true;
}
