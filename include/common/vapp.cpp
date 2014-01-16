#include <VApp>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VApp
// ----------------------------------------------------------------------------
VApp::VApp()
{
  m_argc = 0;
  m_argv = NULL;
}

VApp::~VApp()
{
}

#ifdef WIN32
#define NOMINMAX
#include <winsock2.h>
#include <windows.h>

QString VApp::filePath()
{
  char buf[vd::DEFAULT_BUF_SIZE];
  DWORD res = GetModuleFileNameA(NULL, buf, vd::DEFAULT_BUF_SIZE);
  if (res == vd::DEFAULT_BUF_SIZE && GetLastError() == ERROR_INSUFFICIENT_BUFFER)
  {
    return "";
  }
  return QString(buf);
}
#endif // WIN32

#ifdef linux

#ifndef min
#define min(A,B) (A)>(B)?(A):(B)
#endif // min

QString VApp::filePath()
{
  static QString fileName;
  if (fileName == "")
  {
    char buf[vd::DEFAULT_BUF_SIZE];
    memset(buf, 0, vd::DEFAULT_BUF_SIZE);
    char tmp[256];
    sprintf(tmp, "/proc/%d/exe", getpid());
    int res = min(readlink(tmp, buf, vd::DEFAULT_BUF_SIZE), vd::DEFAULT_BUF_SIZE - 1);
    if(res >= 0)
      buf[res] = '\0';
    fileName = QString(buf);
  }
  return fileName;
}
#endif // linux

QString VApp::currentPath()
{
  return (QString)QDir::currentPath();
}

bool VApp::setCurrentPath(QString path)
{
  return QDir::setCurrent(path);
}

void VApp::initialize(bool path, bool xml, QString uri)
{
  //---------------------------------------------------------------------------
  // set current path
  //---------------------------------------------------------------------------
  if (path)
  {
    VApp::setCurrentPath(QFileInfo(VApp::filePath()).path());
  }

  //---------------------------------------------------------------------------
  // load default xml file
  //---------------------------------------------------------------------------
  if (xml)
  {
    QString fileName = VXmlDoc::defaultFileName();
    // qDebug() << "initialize" << VXmlDoc::instance().toString(); // gilgil temp 2012.05.30
    if (QFile::exists(fileName))
      VXmlDoc::instance().loadFromFile(fileName);
    // qDebug() << "initialize" << VXmlDoc::instance().toString(); // gilgil temp 2012.05.30
  }

  //---------------------------------------------------------------------------
  // load default log
  //---------------------------------------------------------------------------
  if (uri != "")
  {
    VLog* log = VLogFactory::createByDefaultDoc("log");
    if (log == NULL)
    {
      log = VLogFactory::createByURI(uri);
    }
    if (log == NULL)
    {
      printf("***********\n");
      printf("log is null\n");
      printf("***********\n");
      return;
    }
    log->saveToDefaultDoc("log");
    VLog::changeLog(log);
  }
}

void VApp::finalize(bool xml)
{
  //---------------------------------------------------------------------------
  // save default xml file
  //---------------------------------------------------------------------------
  if (xml)
  {
    QString fileName = VXmlDoc::defaultFileName();
    VXmlDoc::instance().saveToFile(fileName);
    // qDebug() << "finalize" << VXmlDoc::instance().toString(); // gilgil temp 2012.05.30
  }
}
