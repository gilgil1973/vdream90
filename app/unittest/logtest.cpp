#include "main.h"
#include <VLogFactory>
#include <iostream> // for std::cout
#include <VLog>
#include <VLogDBWin32>
#include <VLogFile>
#include <VLogHttp>
#include <VLogNull>
#include <VLogStderr>
#include <VLogStdout>
#include <VLogUdp>
#include <VLogList>
#include <VLogFactory>
#include <VDebugNew>

TEST ( Log, defaultTest )
{
  LOG_DEBUG("LOG_DEBUG");
  LOG_INFO ("LOG_INFO");
  LOG_WARN ("LOG_WARN");
  LOG_ERROR("LOG_ERROR");
  LOG_FATAL("LOG_FATAL");
  LOG_TRACE("LOG_TRACE");
}

class MyLog : public VLog
{
protected:
  virtual void write(const char* buf, int len)
  {
    Q_UNUSED(len);
    std::cout << "*** MyLog *** " << buf << std::endl;
  }
};

TEST ( Log, setLogTest )
{
  MyLog myLog;
  myLog.showDateTime = VShowDateTime::None;
  myLog.showThreadID = false;

  VLog* oldLog = VLog::setLog(&myLog);
  LOG_TRACE("Test MyLog"); // MyLog

  VLog::setLog(NULL);
  LOG_TRACE("Test Null"); // This message must not be shown.

  VLog::setLog(oldLog);
  LOG_TRACE("Test Original Log"); // Original Log
}

TEST ( Log, factoryTest )
{
  VMetaClassList& list = VMetaClassMap::getList("VLog");
  foreach(VMetaClass* metaClass, list)
  {
    VLog* log = (VLog*)metaClass->createInstance();
    log->trace("testFactory");
    delete log;
  }
}

TEST ( Log, uriTest )
{
  static const char* uriList[][2] =
  {
  #ifdef WIN32
    { "dbwin32",                     "VLogDBWin32" },
  #endif // WIN32
    { "file",                        "VLogFile"    },
    { "file://log/",                 "VLogFile"    },
    { "file://access.log",           "VLogFile"    },
    { "http",                        "VLogHttp"    },
    { "http://127.0.0.1",            "VLogHttp"    },
    { "null",                        "VLogNull"    },
    { "stderr",                      "VLogStderr"  },
    { "stdout",                      "VLogStdout"  },
    { "udp",                         "VLogUdp"     },
    { "file;null;stderr;stdout;udp", "VLogList"    },
    { "__gilgil_end",                ""            }
  };

  int i = 0;
  while (true)
  {
    QString uri =       uriList[i][0];
    QString className = uriList[i][1];
    if (uri == "__gilgil_end") break;

    VLog* log = VLogFactory::createByURI(uri);
    EXPECT_TRUE( log != NULL );
    if (log != NULL)
    {
      QString className = CLASS_NAME(*log);
      EXPECT_TRUE( className == className );
      log->trace("[%s:%d] uri=%s className=%s", __FILENAME__, __LINE__, qPrintable(uri), qPrintable(className));
      delete log;
    }
    i++;
  }
}

#ifdef WIN32
TEST ( Log, logDBWin32Test )
{
  VLog* log = new VLogDBWin32;
  log->trace("");
  log->trace("[%s:%d] logDBWin32Test", __FILENAME__, __LINE__);
  delete log;
}
#endif // WIN32

TEST ( Log, logFileTest )
{
  VLog* log = new VLogFile;
  log->trace("");
  log->trace("[%s:%d] logFileTest", __FILENAME__, __LINE__);
  delete log;
}

TEST ( Log, logHTTPTest )
{
  VLog* log = new VLogHttp;
  log->trace("");
  log->trace("[%s:%d] logHTTPTest", __FILENAME__, __LINE__);
  delete log;
}

TEST ( Log, logNullTest )
{
  VLog* log = new VLogNull;
  log->trace("");
  log->trace("[%s:%d] ******************************", __FILENAME__, __LINE__);
  log->trace("[%s:%d] This message must not be shown", __FILENAME__, __LINE__);
  log->trace("[%s:%d] ******************************", __FILENAME__, __LINE__);
  delete log;
}

TEST ( Log, logStderrTest )
{
  VLog* log = new VLogStderr;
  log->trace("");
  log->trace("[%s:%d] logStderrTest", __FILENAME__, __LINE__);
  delete log;
}

TEST ( Log, logStdoutTest )
{
  VLog* log = new VLogStdout;
  log->trace("");
  log->trace("[%s:%d] logStdoutTest", __FILENAME__, __LINE__);
  delete log;
}

TEST ( Log, logUDPTest )
{
  VLog* log = new VLogUdp;
  log->trace("");
  log->trace("[%s:%d] logUDPTest", __FILENAME__, __LINE__);
  delete log;
}

TEST ( Log, logListTest )
{
  VLogList* log = new VLogList;
  log->items.push_back(new VLogStdout);
  log->items.push_back(new VLogStdout);
  log->trace("");
  log->trace("[%s:%d] logListTest", __FILENAME__, __LINE__);
  delete log;
}
