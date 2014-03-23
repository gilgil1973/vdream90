#include <VLog>
#include <VLogFactory> // for createDefaultLog
#include <VThread>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VShowDateTime
// ----------------------------------------------------------------------------
VShowDateTime::VShowDateTime(const QString s)
{
  if (s == "None")          value = None;
  else if (s == "Time")     value = Time;
  else if (s == "DateTime") value = DateTime;
  else value = None;
}

QString VShowDateTime::str() const
{
  QString res;
  switch (value)
  {
    case None:     res = "None";     break;
    case Time:     res = "Time";     break;
    case DateTime: res = "DateTime"; break;
  }
  return res;
}

// ----------------------------------------------------------------------------
// VLogInstance
// ----------------------------------------------------------------------------
class VLogInstance
{
public:
  VLogInstance()
  {
    VThreadMgr::instance(); // for dependency // gilgil temp 2012.09.18
    if (VLog::g_log == NULL)
    {
      VLog* log = VLogFactory::createDefaultLog();
      VLog::g_log = log;
    }
  }

  virtual ~VLogInstance()
  {
    // ----- by gilgil 2012.09 13 -----
    // Even after g_log is deleted, log functions(LOG_...) can be called.
    // so, set g_log into NULL so as to prevent calling log functions.
    // changeLog(NULL); // delete g_log and set to NULL.
    VLog* oldLog = VLog::getLog();
    if (oldLog != NULL)
    {
      oldLog->close();
      VLog::setLog(NULL);
      delete oldLog;
    }
    // --------------------------------
  }
};

// ----------------------------------------------------------------------------
// VLog
// ----------------------------------------------------------------------------
VLog* VLog::g_log = NULL;
VLog::VLog()
{
#if _DEBUG
  level = LEVEL_DEBUG;
#else
  level = LEVEL_INFO;
#endif // _DEBUG
  showDateTime   = VShowDateTime::Time;
  showThreadID   = true;
}

VLog::~VLog()
{
  close();
}

int VLog::makeBuf(const char* buf, int len, const char* fmt, va_list args)
{
  int i, res = 0;
  char* p = (char*)buf;

  //
  // dateTime
  //
  if (showDateTime != VShowDateTime::None)
  {
    int i;
    QDateTime now = QDateTime::currentDateTime();
    switch (showDateTime)
    {
      case VShowDateTime::None: // for remove warning
        break;
      case VShowDateTime::Time:
        i = sprintf_s(p, len, "%02d:%02d:%02d %03d : ",
            now.time().hour(), now.time().minute(), now.time().second(), now.time().msec());
        res += i; p += i; len -= i;
        break;
      case VShowDateTime::DateTime:
        i= sprintf_s(p, len, "%02d.%02d %02d:%02d:%02d %03d : ",
          now.date().month(), now.date().day(),
          now.time().hour(), now.time().minute(), now.time().second(), now.time().msec());
        res += i; p += i; len -= i;
        break;
    }
  }

  //
  // threadID
  //
  if (showThreadID)
  {
    Qt::HANDLE id = QThread::currentThreadId();
    i = sprintf_s(p, len, "%08lX ", id);
    res += i; p += i; len -= i;
  }

  //
  // args
  //
  i = vsprintf_s(p, len, fmt, args);
  res += i; len -= i;

  LOG_ASSERT(len >= 0);

  return res;
}

void VLog::debug(const char* fmt, ...)
{
  if (level > LEVEL_DEBUG) return;
  va_list args;
  va_start(args, fmt);
  doDebug(fmt, args);
  va_end(args);
}

void VLog::info(const char* fmt, ...)
{
  if (level > LEVEL_INFO) return;
  va_list args;
  va_start(args, fmt);
  doInfo(fmt, args);
  va_end(args);
}

void VLog::warn(const char* fmt, ...)
{
  if (level > LEVEL_WARN) return;
  va_list args;
  va_start(args, fmt);
  doWarn(fmt, args);
  va_end(args);
}

void VLog::error(const char* fmt, ...)
{
  if (level > LEVEL_ERROR) return;
  va_list args;
  va_start(args, fmt);
  doError(fmt, args);
  va_end(args);
}

void VLog::fatal(const char* fmt, ...)
{
  if (level > LEVEL_FATAL) return;
  va_list args;
  va_start(args, fmt);
  doFatal(fmt, args);
  va_end(args);
}

void VLog::trace(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  doTrace(fmt, args);
  va_end(args);
}

void VLog::doDebug(const char *fmt, va_list args)
{
  char buf[DEFAULT_LOG_BUF_SIZE];
  int len = makeBuf(buf, DEFAULT_LOG_BUF_SIZE, fmt, args);
  write(buf, len);
}

void VLog::doInfo(const char *fmt, va_list args)
{
  char buf[DEFAULT_LOG_BUF_SIZE];
  int len = makeBuf(buf, DEFAULT_LOG_BUF_SIZE, fmt, args);
  write(buf, len);
}

void VLog::doWarn(const char *fmt, va_list args)
{
  char buf[DEFAULT_LOG_BUF_SIZE];
  int len = makeBuf(buf, DEFAULT_LOG_BUF_SIZE, fmt, args);
  write(buf, len);
}

void VLog::doError(const char *fmt, va_list args)
{
  char buf[DEFAULT_LOG_BUF_SIZE];
  int len = makeBuf(buf, DEFAULT_LOG_BUF_SIZE, fmt, args);
  write(buf, len);
}

void VLog::doFatal(const char *fmt, va_list args)
{
  char buf[DEFAULT_LOG_BUF_SIZE];
  int len = makeBuf(buf, DEFAULT_LOG_BUF_SIZE, fmt, args);
  char* delimiter = new char[len + 1];
  memset(delimiter, '*', len);
  delimiter[len] = '\0';
  write(delimiter, len);
  write(buf, len);
  write(delimiter, len);
  delete[] delimiter;
}

void VLog::doTrace(const char *fmt, va_list args)
{
  char buf[DEFAULT_LOG_BUF_SIZE];
  int len = makeBuf(buf, DEFAULT_LOG_BUF_SIZE, fmt, args);
  write(buf, len);
}

bool VLog::open()
{
  return true;
}

bool VLog::close()
{
  return true;
}

VLog* VLog::createByURI(const QString& uri)
{
  Q_UNUSED(uri)
  return NULL;
}

void VLog::load(VXml xml)
{
  level        = xml.getInt("level", level);
  showDateTime = xml.getStr("showDateTime", showDateTime.str());
  showThreadID = xml.getBool("showThreadID", showThreadID);
}

void VLog::save(VXml xml)
{
  xml.setStr("_class", CLASS_NAME(*this));
  xml.setInt("level", level);
  xml.setStr("showDateTime", showDateTime.str());
  xml.setBool("showThreadID", showThreadID);
}

VLog* VLog::getLog()
{
  static VLogInstance mgr;
  return g_log;
}

VLog* VLog::setLog(VLog* log)
{
  VLog* oldLog = getLog(); // VLog* oldLog = g_log; // gilgil temp 2012.12.10
  g_log = log;
  return oldLog;
}

VLog* VLog::changeLog(VLog* log)
{
  VLog* oldLog = getLog(); // VLog* oldLog = g_log; // gilgil temp 2012.12.10
  if (oldLog == log) return NULL;
  g_log = log;
  SAFE_DELETE(oldLog);
  return oldLog;
}

// ------ gilgil temp 2012.05.07 -----
/*
bool VLog::loadFromDefaultDoc(const QString& path)
{
  return VXmlable::loadFromDefaultDoc(path);
}

bool VLog::saveToDefaultDoc(const QString& path)
{
  return VXmlable::saveToDefaultDoc(path);
}
*/
// -----------------------------------

// static VAutoClassInitializer<VLog> g; // gilgil temp 2012.11.01

#ifdef GTEST
#include <gtest/gtest.h>

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

#endif // GTEST
