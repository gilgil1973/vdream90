#include <VLogFactory>
#include <VXmlDoc>
#include <VLogDBWin32>
#include <VLogFile>
#include <VLogHttp>
#include <VLogNull>
#include <VLogStderr>
#include <VLogStdout>
#include <VLogUdp>
#include <VLogList>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VLogFactory
// ----------------------------------------------------------------------------
void VLogFactory::explicitLink()
{
#ifdef WIN32
  VLogDBWin32 logDBWin32;
#endif // WIN32
  VLogFile    logFile;
  VLogHttp    logHTTP;
  VLogNull    logNull;
  VLogStderr  logStderr;
  VLogStdout  logStdout;
  VLogUdp     logUDP;
  VLogList    logList;
}

VLog* VLogFactory::createByClassName(const QString& className)
{
  return VFactory<VLog>::createByClassName(className);
}

VLog* VLogFactory::createByDefaultDoc(const QString& path)
{
  return VFactory<VLog>::createByDefaultDoc(path);
}

VLog* VLogFactory::createByURI(const QString& uri)
{
  VLog* res = NULL;
  VMetaClassList& list = VMetaClassMap::getList("VLog");
  foreach(VMetaClass* metaClass, list)
  {
    VLog* metaLog = (VLog*)(metaClass->createInstance());
    VLog* log = metaLog->createByURI(uri);
    delete metaLog;
    if (log != NULL)
    {
      res = log;
      break;
    }
    delete log;
  }
  if (res == NULL)
  {
    LOG_FATAL("can not create log for '%s'", qPrintable(uri));
  }
  return res;
}

VLog* VLogFactory::createDefaultLog()
{
  return new VLogUdp;
}
