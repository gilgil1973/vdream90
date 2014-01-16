#include <VLogList>
#include <VLogFactory>
#include <VDebugNew>

REGISTER_METACLASS(VLogList, VLog)

// ----------------------------------------------------------------------------
// VLogList
// ----------------------------------------------------------------------------
VLogList::VLogList()
{
  clear();
}

VLogList::~VLogList()
{
  close();
  clear();
}

void VLogList::clear()
{
  int _count = items.count();
  for (int i = 0; i < _count; i++)
  {
    VLog* log = items.at(i);
    delete log;
  }
  items.clear();
}

void VLogList::debug(const char* fmt, ...)
{
  if (level > LEVEL_DEBUG) return;
  va_list args;
  foreach(VLog* log, items)
  {
    if (log->level <= LEVEL_DEBUG)
    {
      va_start(args, fmt);
      log->doDebug(fmt, args);
      va_end(args);
    }
  }
}

void VLogList::info(const char* fmt, ...)
{
  if (level > LEVEL_INFO) return;
  va_list args;
  foreach(VLog* log, items)
  {
    if (log->level <= LEVEL_INFO)
    {
      va_start(args, fmt);
      log->doInfo(fmt, args);
      va_end(args);
    }
  }
}

void VLogList::warn(const char* fmt, ...)
{
  if (level > LEVEL_WARN) return;
  va_list args;
  foreach(VLog* log, items)
  {
    if (log->level <= LEVEL_WARN)
    {
      va_start(args, fmt);
      log->doWarn(fmt, args);
      va_end(args);
    }
  }
}

void VLogList::error(const char* fmt, ...)
{
  if (level > LEVEL_ERROR) return;
  va_list args;
  foreach(VLog* log, items)
  {
    if (log->level <= LEVEL_ERROR)
    {
      va_start(args, fmt);
      log->doError(fmt, args);
      va_end(args);
    }
  }
}

void VLogList::fatal(const char* fmt, ...)
{
  if (level > LEVEL_FATAL) return;
  va_list args;
  foreach(VLog* log, items)
  {
    if (log->level <= LEVEL_FATAL)
    {
      va_start(args, fmt);
      log->doFatal(fmt, args);
      va_end(args);
    }
  }
}

void VLogList::trace(const char* fmt, ...)
{
  va_list args;
  foreach(VLog* log, items)
  {
    va_start(args, fmt);
    log->doTrace(fmt, args);
    va_end(args);
  }
}

bool VLogList::open()
{
  int _count = items.count();
  for (int i = 0; i < _count; i++)
  {
    VLog* log = items.at(i);
    if (!log->open())
    {
      close();
      return false;
    }
  }
  return true;
}

bool VLogList::close()
{
  int _count = items.count();
  for (int i = 0; i < _count; i++)
  {
    VLog* log = items.at(i);
    log->close(); // do not check result
  }
  return true;
}

void VLogList::write(const char* buf, int len)
{
  Q_UNUSED(buf) Q_UNUSED(len)
  LOG_FATAL("virtual functio call %p", this);
}

VLog* VLogList::createByURI(const QString& uri)
{
  QStringList strList = uri.split(";");
  if (strList.size() > 1) // is equal or more than 2
  {
    VLogList* res = new VLogList;

    foreach(QString s, strList)
    {
      VLog* childLog = VLogFactory::createByURI(s);
      if (childLog != NULL)
      {
        res->items.push_back(childLog);
      }
    }
    return res;
  }
  return NULL;
}

void VLogList::load(VXml xml)
{
  VLog::load(xml);

  clear();
  xml_foreach(childXML, xml.childs())
  {
    QString className = childXML.getStr("_class", "");
    VLog* childLog = VLogFactory::createByClassName(className);
    if (childLog == NULL)
    {
      LOG_ERROR("childLog is NULL(%s)", qPrintable(className));
      return;
    }
    childLog->load(childXML);
    items.push_back(childLog);
  }
}

void VLogList::save(VXml xml)
{
  VLog::save(xml);
  xml.clearChild();
  foreach(VLog* childLog, items)
  {
    QString className = CLASS_NAME(*childLog);
    VXml childXML = xml.addChild("log");
    childLog->save(childXML);
  }
}
