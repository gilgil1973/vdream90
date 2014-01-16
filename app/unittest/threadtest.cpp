#include "main.h"
#include <VThread>
#include <VMemoryLeak>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// basicThread
// ----------------------------------------------------------------------------
class MyThread : public VThread
{
protected:
  int m_n;
public:
  MyThread(int n) : m_n(n) {};
  virtual ~MyThread() { close(); }
protected:
  virtual void run()
  {
    for (int i = 0; i < 3; i ++)
    {
      msleep(100);
      printf("%08lX thread %d\n", VThread::currentID(), m_n);
    }
  }
};

TEST( Thread, basicThreadTest )
{
  static const int THREAD_CNT = 5;
  MyThread* myThread[THREAD_CNT];

  for (int i = 0; i < THREAD_CNT; i++)
  {
    myThread[i] = new MyThread(i);
    myThread[i]->open();
  }
  printf("%08lX closing all threads...\n", VThread::currentID());
  for (int i = 0; i < THREAD_CNT; i++)
  {
    myThread[i]->close();
    delete myThread[i];
  }
  printf("%08lX all threads closed\n", VThread::currentID());
}

// ----------------------------------------------------------------------------
// simpleThread
// ----------------------------------------------------------------------------
void threadFunc(void* p)
{
  LOG_DEBUG("threadFunc %p", p);
};

TEST( Thread, simpleThreadTest )
{
  LOG_DEBUG("simpleThread");
  VSimpleThread st(threadFunc, (void*)0x12345678);
  EXPECT_TRUE( st.open() );
  EXPECT_TRUE( st.close() );
}

// ----------------------------------------------------------------------------
// runnable
// ----------------------------------------------------------------------------
class MyRunnableObject : public VRunnable
{
protected:
  virtual void run()
  {
    LOG_DEBUG("running...");
  }
};

TEST( Thread, runnableTest )
{
  LOG_DEBUG("runnable");
  MyRunnableObject obj;
  EXPECT_TRUE( obj.runThread().open() );
  EXPECT_TRUE( obj.runThread().wait() );
  EXPECT_TRUE( obj.runThread().close() );
}

// ----------------------------------------------------------------------------
// openClose
// ----------------------------------------------------------------------------
volatile static int debugCnt = 0;
static VCS openCloseCS;
class OpenCloseThread : public VThread
{
protected:
  virtual void run() {
    openCloseCS.lock();
    debugCnt++;
    openCloseCS.unlock();
  }
};

TEST( Thread, openCloseTest )
{
  static const int OPEN_CLOSE_CNT = 1000; // gilgil temp 2012.05.29

  OpenCloseThread* thread[OPEN_CLOSE_CNT];
  for (int i= 0 ; i < OPEN_CLOSE_CNT; i++)
  {
    thread[i] = new OpenCloseThread;
  }
  for (int i = 0; i < OPEN_CLOSE_CNT; i++)
  {
    bool res = thread[i]->open();
    if (!res)
    {
      printf("thread.open() return false i=%d\n", i);
      msleep(1000);
    }
    EXPECT_TRUE( res );
  }
  for (int i = 0; i < OPEN_CLOSE_CNT; i++)
  {
    EXPECT_TRUE ( thread[i]->close() );
  }
  for (int i = 0; i < OPEN_CLOSE_CNT; i++)
  {
    delete thread[i];
  }

  LOG_DEBUG("debugCnt=%d",debugCnt);
  EXPECT_EQ( debugCnt, OPEN_CLOSE_CNT );
}

// --------------------------------------------------------------------------
// timeout
// --------------------------------------------------------------------------
class TimeoutThread : public VThread
{
protected:
  virtual void run()
  {
    msleep(1000);
  }
};

TEST( Thread, timeoutTest )
{
  {
    TimeoutThread thread;
    thread.open();

    LOG_INFO("Thread timeout will occur. Don't worry. :)");
    int oldLevel = getLog()->level;
    getLog()->level = VLog::LEVEL_NONE;

    bool res = thread.wait(500);
    EXPECT_TRUE( res == false );

    getLog()->level = oldLevel;
  }

  {
    TimeoutThread thread;
    thread.open();
    bool res = thread.wait(1500);
    EXPECT_TRUE( res == true );
  }
}

// --------------------------------------------------------------------------
// exception
// --------------------------------------------------------------------------
class ExceptionThread : public VThread
{
protected:
  virtual void run()
  {
    tag   = 100;
    int i = 5;
    tag   = 200;
    int j = 0;
    tag   = 300;
    int k = i / j; // occur exception
    tag   = 400;
    printf("i=%d j=%d k=%d\n", i, j, k);
    tag   = 500;
  }
};

TEST( Thread, exceptionTest )
{
  ExceptionThread thread;
  thread.open();
  LOG_INFO("Exception will occur. Don't worry. :)");
  bool res = thread.wait();
  EXPECT_TRUE( res == true );
  EXPECT_EQ( thread.tag , 300 );
}

// --------------------------------------------------------------------------
// multiWait
// --------------------------------------------------------------------------
class RunThread : public VThread
{
protected:
  virtual void run()
  {
    msleep(5000);
  }
};

class WaitThread : public VThread
{
public:
  RunThread* runThread;
public:
  WaitThread(RunThread* runThread) { this->runThread = runThread; }
protected:
  virtual void run()
  {
    msleep(1000);
    runThread->close();
  }
};

TEST( Thread, multiWaitTest )
{
  static const int WAIT_THREAD_CNT = 1;

  VMemoryLeak::start(false);
  {
    RunThread runThread;
    EXPECT_TRUE( runThread.open() );

    WaitThread* waitThread[WAIT_THREAD_CNT];
    for (int i = 0; i < WAIT_THREAD_CNT; i++)
    {
      waitThread[i] = new WaitThread(&runThread);
      EXPECT_TRUE( waitThread[i]->open() );
    }

    for (int i = 0; i < WAIT_THREAD_CNT; i++)
    {
      EXPECT_TRUE( waitThread[i]->close() );
    }

    for (int i = 0; i < WAIT_THREAD_CNT; i++)
    {
      waitThread[i]->close();
      delete waitThread[i];
    }

    runThread.close();
  }
  VMemoryLeak::stop(false);
}


// --------------------------------------------------------------------------
// virtualFunctionCallError
// --------------------------------------------------------------------------
static int succeedCnt = 0;
static VCS virtualFunctionCallErrorCS;
class NoDestructorThread : public VThread
{
public:
  // virtual ~NoDestructorThread() { close(); }
protected:
  virtual void run()
  {
    virtualFunctionCallErrorCS.lock();
    succeedCnt++;
    virtualFunctionCallErrorCS.unlock();
  }
};

TEST( Thread, virtualFunctionCallErrorTest )
{
  static const int THREAD_CNT = 1000;
  int oldLevel = getLog()->level;
  getLog()->level = VLog::LEVEL_NONE;
  for (int i = 0; i < THREAD_CNT; i++)
  {
    NoDestructorThread* thread = new NoDestructorThread;
    thread->open();
    // Deleting object witheout calling call() medhod can occur virtual function call error.
    delete thread;
  }
  getLog()->level = oldLevel;
  LOG_INFO("THREAD_CNT=%d succeedCnt=%d virtual function call count=%d\n", THREAD_CNT, succeedCnt, THREAD_CNT - succeedCnt);
  EXPECT_TRUE( succeedCnt != THREAD_CNT ); // May be at lease one error occurs!!!
}

TEST( Thread, freeOnTerminateTest )
{
  static const int COUNT = 5;
  for (int i = 0; i < COUNT; i++)
  {
    MyThread* myThread = new MyThread(i);
    myThread->freeOnTerminate = true;
    EXPECT_TRUE( myThread->open() );
  }
  sleep(1);
}
