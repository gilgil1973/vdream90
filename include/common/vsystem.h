// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_SYSTEM_H__
#define __V_SYSTEM_H__

#ifdef WIN32
  #define NOMINMAX
  #include <winsock2.h>
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif // WIN32
#ifdef linux
  #include <unistd.h>
#endif // linux
#include <VLinkLibrary>

// ----------------------------------------------------------------------------
// VTimeout
// ----------------------------------------------------------------------------
typedef unsigned long VTimeout;

// ----------------------------------------------------------------------------
// sleep
// ----------------------------------------------------------------------------
void msleep(VTimeout msecs);
#ifdef WIN32
void sleep (VTimeout secs);
void usleep(VTimeout usecs);
#endif // WIN32

#include <QMutex>
#include <QWaitCondition>

// ----------------------------------------------------------------------------
// VCS (CriticalSection)
// ----------------------------------------------------------------------------
class VCS : public QMutex
{
public:
  VCS() : QMutex(Recursive) {}
};

// ----------------------------------------------------------------------------
// VLockable
// ----------------------------------------------------------------------------
class VLockable
{
  friend class VLock;
protected:
  VCS m_cs;
public:
  void lock()   { m_cs.lock();   }
  void unlock() { m_cs.unlock(); }
};

// ----------------------------------------------------------------------------
// VStateLockable
// ----------------------------------------------------------------------------
class VStateLockable
{
  friend class VLock;
protected:
  VCS stateOpenCs;
  VCS stateCloseCs;
  VCS stateReadCs;
  VCS stateWriteCs;
};

// ----------------------------------------------------------------------------
// VLock
// ----------------------------------------------------------------------------
class VLock
{
protected:
  VCS& m_cs;

public:
  VLock(VLockable& lockable) : m_cs(lockable.m_cs) { m_cs.lock();   }
  VLock(VCS& cs)             : m_cs(cs)            { m_cs.lock();   }
  virtual ~VLock()                                 { m_cs.unlock(); }
};

// ----------------------------------------------------------------------------
// VEvent
// ----------------------------------------------------------------------------
class VEvent
{
protected:
  QMutex         m_mutex;
  QWaitCondition m_cond;
  bool           m_manualReset;
  bool           m_state;

public:
  VEvent(bool manualReset = false, bool initialState = false);
  virtual ~VEvent();

public:
  bool state() { return m_state; }
  void setEvent();
  void resetEvent();
  bool wait(VTimeout timeout = ULONG_MAX);
};

// ----------------------------------------------------------------------------
// VProcess
// ----------------------------------------------------------------------------
class VProcess
{
public:
  static bool run(const char* command);
};

#endif // __V_SYSTEM_H__
