#include <VUdpSession>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VUdpSession
// ----------------------------------------------------------------------------
VUdpSession::VUdpSession(void* owner) : VNetSession(owner)
{
  handle = INVALID_SOCKET;
  memset(&addr, 0, sizeof(addr));
}

VUdpSession::~VUdpSession()
{
  close();
}

bool VUdpSession::doOpen()
{
  // VLock lock(m_openCloseCS); // gilgil temp 2014.02.28

  if (handle == INVALID_SOCKET)
  {
    SET_ERROR(VError, "handle is zero", VERR_HANDLE_IS_ZERO);
    return false;
  }
  return true;
}

bool VUdpSession::doClose()
{
  // VLock lock(m_openCloseCS); // gilgil temp 2014.02.28

  if (handle == INVALID_SOCKET) return true;

  //
  // shutdown
  //
  int res = ::shutdown(handle, SD_BOTH);
  if (res == SOCKET_ERROR)
  {
#ifdef linux
    if (WSAGetLastError() != ENOTCONN)
#endif // linux
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

// ----- gilgil temp 2009.08.16 -----
void VUdpSession::logAddr(SOCKADDR_IN* sockAddr)
{
  LOG_DEBUG("[VDUDPSession.cpp] logAddr sin_family=%d sin_port=%d sin_addr=%d.%d.%d.%d",
    sockAddr->sin_family,
    ntohs(sockAddr->sin_port),
    (sockAddr->sin_addr.s_addr & 0xFF000000) >> 24,
    (sockAddr->sin_addr.s_addr & 0x00FF0000) >> 16,
    (sockAddr->sin_addr.s_addr & 0x0000FF00) >> 8,
    (sockAddr->sin_addr.s_addr & 0x000000FF) >> 0);
}
// ----------------------------------

int VUdpSession::doRead(char* buf, int size)
{
  VLock lock(m_readCS);

  SOCKADDR_IN tempAddr;
  socklen_t fromLen = sizeof(tempAddr);
  memset(&tempAddr, 0, sizeof(tempAddr));
  int res = recvfrom(handle, buf, size, 0, (SOCKADDR*)&tempAddr, &fromLen);
  // logAddr(&tempAddr); // gilgil temp 2012.05.29
  addr = tempAddr;
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

int VUdpSession::doWrite(char* buf, int size)
{
  LOG_DEBUG("log test"); // gilgil temp 2012.11.01

  VLock lock(m_writeCS);

  int res;
  int restSize = size;

  while (true)
  {
    if (onceWriteSize!= 0 && restSize > onceWriteSize) restSize = onceWriteSize;
    res = ::sendto(handle, buf, restSize, 0, (SOCKADDR*)&addr, sizeof(addr));;
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

Ip VUdpSession::getLocalIP()
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

Ip VUdpSession::getRemoteIP()
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

int VUdpSession::getLocalPort()
{
  socklen_t size;
  SOCKADDR_IN sockAddr;

  if (handle == INVALID_SOCKET) return 0;
  size = sizeof(sockAddr);
  if (::getsockname(handle, (SOCKADDR*)&sockAddr, &size) != 0) return 0;
  int res = ntohs(sockAddr.sin_port);
  return res;
}

int VUdpSession::getRemotePort()
{
  socklen_t size;
  SOCKADDR_IN sockAddr;

  if (handle == INVALID_SOCKET) return 0;
  size = sizeof(sockAddr);
  if (::getpeername(handle, (SOCKADDR*)&sockAddr, &size) != 0) return 0;
  int res = ntohs(sockAddr.sin_port);
  return res;
}

bool VUdpSession::operator == (const VUdpSession& rhs) const
{
  if (handle != rhs.handle) return false;
  if (memcmp(&addr, &rhs.addr, sizeof(addr)) != 0) return false;
  return true;
}
