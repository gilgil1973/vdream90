#include <VSystem>
#include <VLog>
#include <QThread>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// __VMyThread__
// ----------------------------------------------------------------------------
class __VMyThread__ : public QThread
{
public:
  static void msleep(VTimeout msecs) { QThread::msleep(msecs); }
  static void sleep(VTimeout secs)   { QThread::sleep(secs);   }
  static void usleep(VTimeout usecs) { QThread::usleep(usecs); }
};

// ----------------------------------------------------------------------------
// sleep
// ----------------------------------------------------------------------------
void msleep(VTimeout msecs)
{
  __VMyThread__::msleep(msecs);
}

#ifdef WIN32
void sleep(VTimeout secs)
{
  __VMyThread__::sleep(secs);
}

void usleep(VTimeout usecs)
{
  __VMyThread__::usleep(usecs);
}
#endif // WIN32

// ----------------------------------------------------------------------------
// VEvent
// ----------------------------------------------------------------------------
VEvent::VEvent(bool manualReset, bool initialState)
{
  m_manualReset = manualReset;
  m_state       = initialState;
}

VEvent::~VEvent()
{
}

void VEvent::setEvent()
{
  m_mutex.lock();
  m_state = true;
  m_cond.wakeAll();
  m_mutex.unlock();
}

void VEvent::resetEvent()
{
  m_state = false;
}

bool VEvent::wait(VTimeout timeout)
{
  m_mutex.lock();
  bool res = true;
  if (!m_state)
  {
    res = m_cond.wait(&m_mutex, timeout);
  }
  if (!m_manualReset) m_state = false;
  m_mutex.unlock();
  return res;
}

// ----------------------------------------------------------------------------
// VProcess
// ----------------------------------------------------------------------------
bool VProcess::run(const char* command)
{
#ifdef WIN32
  UINT nRes = WinExec(command, SW_HIDE);
  if (nRes <= 31)
  {
    LOG_ERROR("WinExec(%s) return %u", qPrintable(command), nRes);
    return true;
  }
  return false;
#endif // WIN32
#ifdef linux
  int nRes = system(command);
  if (nRes == -1)
  {
    LOG_ERROR("system(%s) return %d", qPrintable(command), nRes);
    return false;
  }
  return true;
#endif // linux
}
