#include <VTcpServer>
#include <VTick> // for tick()
#include <VDebugNew>

REGISTER_METACLASS(VTcpServer, VNet)

// ----------------------------------------------------------------------------
// VTcpSessionThread
// ----------------------------------------------------------------------------
VTcpSessionThread::VTcpSessionThread(VTcpServer* owner, VTcpSession* tcpSession) : VThread(owner)
{
  // LOG_DEBUG("%p", this); // gilgil temp 2012.08.16
  this->freeOnTerminate = true;
  this->tcpServer       = owner;
  this->tcpSession      = tcpSession;
}

VTcpSessionThread::~VTcpSessionThread()
{
  // LOG_DEBUG("%p", this); // gilgil temp 2012.08.16
  SAFE_DELETE(tcpSession);
}

void VTcpSessionThread::run()
{
  tag = 20000; // gilgil temp 2014.04.04
  emit tcpServer->runned(tcpSession);
  tag = 20100; // gilgil temp 2014.04.04

  // sleep(1); // gilgil temp 2014.04.04
  tag = 20200; // gilgil temp 2014.04.04
  VTcpSessionThreadList& threadList = tcpServer->threadList;
  tag = 20300; // gilgil temp 2014.04.04
  threadList.lock();
  tag = 20400; // gilgil temp 2014.04.04
  threadList.removeAt(threadList.indexOf(this));
  tag = 20500; // gilgil temp 2014.04.04
  threadList.unlock();
  tag = 20600; // gilgil temp 2014.04.04
}

// ----------------------------------------------------------------------------
// VTcpServer
// ----------------------------------------------------------------------------
VTcpServer::VTcpServer(void* owner) : VNetServer(owner)
{
  acceptSession = new VTcpSession(this);
}

VTcpServer::~VTcpServer()
{
  close();
  SAFE_DELETE(acceptSession);
}

#ifndef BACKLOG
#define BACKLOG 10
#endif // BACKLOG
bool VTcpServer::doOpen()
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
  acceptSession->handle = ::socket(AF_INET, SOCK_STREAM, 0);
  if (acceptSession->handle == INVALID_SOCKET)
  {
    SET_ERROR(VSocketError, "error in socket", WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // bind
  // ------------------------------------------------------------------------
  acceptSession->addr.sin_family = AF_INET;
  acceptSession->addr.sin_port = htons(quint16(port));
  if (localHost == "")
  {
    acceptSession->addr.sin_addr.s_addr = INADDR_ANY;
  } else
  {
    Ip ip = VNet::resolve(localHost);
    if (ip == 0)
    {
      SET_ERROR(VNetError, qformat("can not resolve host(%s)", qPrintable(localHost)), VERR_CAN_NOT_RESOLVE_HOST);
      return false;
    }
    acceptSession->addr.sin_addr.s_addr = htonl(ip);
  }
  memset(&acceptSession->addr.sin_zero, 0, sizeof(acceptSession->addr.sin_zero));

  int res = ::bind(acceptSession->handle, (SOCKADDR*)&acceptSession->addr, sizeof(acceptSession->addr));
  if (res == SOCKET_ERROR)
  {
    SET_ERROR(VSocketError, qformat("error in bind(%s:%d)", qPrintable(localHost), port), WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // listen
  // ------------------------------------------------------------------------
  res = ::listen(acceptSession->handle, BACKLOG);
  if (res == SOCKET_ERROR)
  {
    SET_ERROR(VSocketError, "error in listen", WSAGetLastError());
    return false;
  }

  // ------------------------------------------------------------------------
  // ok
  // ------------------------------------------------------------------------
  if (!acceptSession->open()) return false;
  // runThread().setLog(getLog()); // gilgil temp 2012.09.18
  if (!runThread().open())
  {
    error = runThread().error;
    return false;
  }
  //this->moveToThread(runThread().m_qthread); // gilgil temp 2012.08.16

  return true;
}

bool VTcpServer::doClose()
{
  threadTag = 1011; // gilgil temp 2014.04.04
  VLock lock(stateCloseCs); // gilgil temp 2014.03.14
  threadTag = 1012; // gilgil temp 2014.04.04

  // ------------------------------------------------------------------------
  // Close acceptSession and thread
  // ------------------------------------------------------------------------
  acceptSession->close();
  runThread().close();


  threadTag = 1016; // gilgil temp 2014.04.04
  threadList.lock();
  // ------------------------------------------------------------------------
  // Close all connected socket
  // ------------------------------------------------------------------------
  for (VTcpSessionThreadList::iterator it = threadList.begin(); it != threadList.end(); it++)
  {
    VTcpSessionThread* thread = *it;
    // ----- by gilgil 2012.12.07 -----
    // When TcpServer is active, session thread is deleted automatically,
    // while, when TcpServer is closing state, session thread is deleted manually.
    // thread->freeOnTerminate = false; // gilgil temp 2012.12.07 // gilgil temp 2014.04.04
    // --------------------------------
    VTcpSession* tcpSession = thread->tcpSession;
    tcpSession->close();
  }
  threadList.unlock();
  threadTag = 1018; // gilgil temp 2014.04.04

  // ------------------------------------------------------------------------
  // Wait until all connection is disconnected
  // ------------------------------------------------------------------------
  VTick beg = tick();
  while (true)
  {
    msleep(1);
    threadList.lock();
    int count = threadList.count();
    threadList.unlock();
    if (count == 0) break;
    VTick now = tick();
    if (now - beg > vd::DEFAULT_TIMEOUT * 2) // gilgil temp 2014.04.04
    {
      LOG_FATAL("timeout session count=%d", threadList.count());
      // break; // gilgil temp 2012.11.27
    }
  }
  /*
  threadList.lock();
  int count = threadList.count();
  threaddList.un
  for (VTcpSessionThreadList::iterator it = threadList.begin(); it != threadList.end(); it++)
  {
    threadTag = 1020; // gilgil temp 2014.04.04
    VTcpSessionThread* thread = *it;
    threadTag = 1022; // gilgil temp 2014.04.04
    thread->wait(vd::DEFAULT_TIMEOUT + 2000); // gilgil temp 2014.04.04
    threadTag = 1023; // gilgil temp 2014.04.04
    delete thread;
    threadTag = 1024; // gilgil temp 2014.04.04
    VTick now = tick();
    if (now - beg > vd::DEFAULT_TIMEOUT * 2) // gilgil temp 2014.04.04
    {
      LOG_FATAL("timeout session count=%d", threadList.count());
      // break; // gilgil temp 2012.11.27
    }
    threadTag = 1025; // gilgil temp 2014.04.04
  }
  threadList.clear();
  threadList.unlock();
  threadTag = 1026; // gilgil temp 2014.04.04
  */

  return true;
}

int VTcpServer::doRead(char* buf, int size)
{
  // VLock lock(stateReadCs); // gilgil temp 2014.03.14

  Q_UNUSED(buf)
  Q_UNUSED(size)
  SET_ERROR(VError, "not readable", VERR_NOT_READABLE);
  return VERR_FAIL;
}

int VTcpServer::doWrite(char* buf, int size)
{
  VLock lock(stateWriteCs); // gilgil temp 2014.03.14

  threadList.lock();
  for (VTcpSessionThreadList::iterator it = threadList.begin(); it != threadList.end(); it++)
  {
    VTcpSessionThread* thread = *it;
    thread->tcpSession->write(buf, size); // do not check result
  }
  threadList.unlock();
  return size;
}

VTcpSession* VTcpServer::accept()
{
  socklen_t size;
  SOCKADDR_IN sockAddr;
  SOCKET newHandle;
  VTcpSession* newTCPSession;

  if (acceptSession->handle == INVALID_SOCKET)
  {
    SET_ERROR(VError, "invalid handle", VERR_INVALID_HANDLE);
    goto _error;
  }

  // ------------------------------------------------------------------------
  // accept
  // ------------------------------------------------------------------------
  size = sizeof(sockAddr);
  newHandle = ::accept(acceptSession->handle, (SOCKADDR*)&sockAddr, &size);
  if (newHandle == INVALID_SOCKET)
  {
    SET_DEBUG_ERROR(VNetError, "error in accept", WSAGetLastError());
    goto _error;
  }

  newTCPSession         = new VTcpSession;
  newTCPSession->owner  = this;
  newTCPSession->handle = newHandle;
  newTCPSession->addr   = sockAddr;
  return newTCPSession;

_error:
  return NULL;
}

void VTcpServer::run()
{
  while (runThread().active())
  {
    VTcpSession* newTCPSession = accept();
    if (newTCPSession == NULL)
    {
      SET_DEBUG_ERROR(VNetError, "error in accept", WSAGetLastError());
      break;
    }
    if (!newTCPSession->open())
    {
      error = newTCPSession->error;
      continue;
    }
    VTcpSessionThread* thread = new VTcpSessionThread(this, newTCPSession);
    //thread->moveToThread(runThread().m_qthread); // gilgil temp 2012.08.16
    thread->name = className();
    threadList.lock();
    threadList.append(thread);
    threadList.unlock();
    if (!thread->open())
    {
      error = thread->error;
      break;
    }
  }
}

void VTcpServer::load(VXml xml)
{
  VNetServer::load(xml);
}

void VTcpServer::save(VXml xml)
{
  VNetServer::save(xml);
}

#ifdef QT_GUI_LIB
void VTcpServer::optionAddWidget(QLayout* layout)
{
  VNetServer::optionAddWidget(layout);
}

void VTcpServer::optionSaveDlg(QDialog* dialog)
{
  VNetServer::optionSaveDlg(dialog);
}
#endif // QT_GUI_LIB
