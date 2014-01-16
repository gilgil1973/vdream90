// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_THREAD_H__
#define __V_THREAD_H__

#include <QThread>
#include <VObject>
#include <VException>

// ----------------------------------------------------------------------------
// VQThread
// ----------------------------------------------------------------------------
class VQThread : public QThread
{
  friend class VThread;

public:
  VQThread(void* owner); // owner is VThread
  ~VQThread();

protected:
  void* owner; // VThread

protected:
  void _run();
  virtual void run();
};

// ----------------------------------------------------------------------------
// IVRunnable
// ----------------------------------------------------------------------------
class IVRunnable
{
protected:
  virtual void run() = 0;
};

// ----------------------------------------------------------------------------
// VThread
// ----------------------------------------------------------------------------
class VThread :
  public  VObject,
  public  VLockable,
  public  IVRunnable,
  private VNonCopyable
{
  friend class VQThread;

public:
  VThread(void* owner = NULL);
  ~VThread();

public:
  virtual bool open();
  virtual bool close();
  virtual bool close(bool wait, VTimeout timeout = vd::DEFAULT_TIMEOUT);
  virtual bool wait(VTimeout timeout = vd::DEFAULT_TIMEOUT);

protected:
  virtual bool doOpen();
  virtual bool doClose();

protected:
  virtual void run() /*= 0*/;

protected:
  Qt::HANDLE m_id;

public:
  VQThread* m_qthread;
  Qt::HANDLE id(){ return m_id; }

public:
  bool              freeOnTerminate;
  QThread::Priority threadPriority;

public:
  static Qt::HANDLE currentID() { return QThread::currentThreadId(); }

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

// ----------------------------------------------------------------------------
// VThreadMgr
// ----------------------------------------------------------------------------
class VThreadMgr : public QObject, public VLockable
{
  Q_OBJECT

  friend class VThread;

private: // singleton
  VThreadMgr();
  virtual ~VThreadMgr();

protected:
  bool m_suspending;
  QList<VThread*> threadList;

public:
  bool suspending() { return m_suspending;  }

public:
  void clear(bool checkLeak);
  void add(VThread* thread);
  void del(VThread* thread);
  void suspend();
  void resume();

public:
  static VThreadMgr& instance()
  {
    static VThreadMgr g_instance;
    return g_instance;
  }
};

// ----------------------------------------------------------------------------
// VSimpleThread
// ----------------------------------------------------------------------------
class VRunnable;
typedef void(*VThreadFunc)(void* p);
class VSimpleThread : public VThread
{
  friend class VRunnable;

protected:
  VThreadFunc m_threadFunc;
  void*       m_p;
  VRunnable*  m_runnable;

public:
  VSimpleThread(VThreadFunc threadFunc, void* p = NULL, VRunnable* runnable = NULL);
  virtual ~VSimpleThread();

public:
  virtual bool open();
protected:
  virtual void run();
};

// ----------------------------------------------------------------------------
// VRunnable
// ----------------------------------------------------------------------------
class VRunnable : public IVRunnable
{
private:
  VSimpleThread* m_thread;
  static void threadFunc(void* p)
  {
    VRunnable* runnable = (VRunnable*)p;
    runnable->run();
  }

public:
  VRunnable()
  {
    m_thread = NULL;
    createThread();
  }
  virtual ~VRunnable()
  {
    deleteThread();
  }
  VSimpleThread* createThread()
  {
    if (m_thread == NULL)
    {
      m_thread = new VSimpleThread(threadFunc, this, this);
    }
    return m_thread;
  }
  void deleteThread()
  {
    if (m_thread != NULL)
    {
      m_thread->close();
      delete m_thread;
      m_thread = NULL;
    }
  }
  VSimpleThread& runThread()
  {
    return *m_thread;
  }
};

// ----------------------------------------------------------------------------
// __thread
// ----------------------------------------------------------------------------
#ifdef WIN32
#define __thread __declspec(thread)
#endif // WIN32

// ----------------------------------------------------------------------------
// ThreadError
// ----------------------------------------------------------------------------
VDECLARE_ERROR_CLASS(VThreadError);

// ----------------------------------------------------------------------------
// ThreadError Code
// ----------------------------------------------------------------------------
static const int VERROR_IN_WAIT_OPEN        = VERR_CATEGORY_THREAD + 0;
//static const int THREAD_ALREADY_TERMINATED = VERR_CATEGORY_THREAD + 1; // gilgil temp 2012.05.28

#endif // __V_THREAD_H__
