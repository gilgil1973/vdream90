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

QString VApp::fileName()
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

QString VApp::fileName()
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

QString VApp::_filePath()
{
  QString res = QFileInfo(VApp::fileName()).path();
  if (!res.endsWith('/') && !res.endsWith('\\')) res += QDir::separator();
  return res;
}

QString VApp::currentPath()
{
  QString res = QDir::currentPath();
  if (!res.endsWith('/') && !res.endsWith('\\'))
    res += QDir::separator();
  return res;
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
    VApp::setCurrentPath(VApp::_filePath());
  }

  //---------------------------------------------------------------------------
  // load default xml file
  //---------------------------------------------------------------------------
  if (xml)
  {
    QString fileName = VXmlDoc::defaultFileName();
    if (QFile::exists(fileName))
      VXmlDoc::instance().loadFromFile(fileName);
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
  }
}

#ifdef GTEST
#include <gtest/gtest.h>

TEST( App, filePathTest )
{
  QString filePath = VApp::filePath();
  qDebug() << "filePath=" << filePath;
  QString fileName = QFileInfo(filePath).fileName();
#ifdef WIN32
  EXPECT_TRUE( fileName == "unittest.exe" );
#endif // WIN32
#ifdef linux
  EXPECT_TRUE( fileName == "unittest" );
#endif // linux
}

TEST( App, pathTest )
{
  QString rootFileName    = "__root__.txt";
  QString currentFileName = "__current__.txt";

  QString path = VApp::currentPath();
  qDebug() << "current path=" << path;

  {
    EXPECT_TRUE( VApp::setCurrentPath("/") == true );
    FILE* fp;
    fopen_s(&fp, rootFileName.toLatin1().data(), "wb");
    fclose(fp);
    EXPECT_TRUE( QFile::exists(rootFileName) );
    QFile::remove(rootFileName);
  }
  {
    EXPECT_TRUE( VApp::setCurrentPath(path) == true );
    FILE* fp;
    fopen_s(&fp, currentFileName.toLatin1().data(), "wb");
    fclose(fp);
    EXPECT_TRUE( QFile::exists(currentFileName) );
    QFile::remove(currentFileName);
  }
}

static char *argv[] =
{
  "test.exe",
  "argument1",
  "argument2"
};

TEST( App, argumentsTest )
{
  VApp::instance().initialize(0, NULL);

  VApp& app = VApp::instance();
  app.setArguments(3, argv);

  QString s;
  char** argv = app.argv();
  s = argv[0]; EXPECT_TRUE(  s == "test.exe"  );
  s = argv[1]; EXPECT_TRUE(  s == "argument1" );
  s = argv[2]; EXPECT_TRUE(  s == "argument2" );

  VApp::instance().initialize(0, NULL);
}

#endif // GTEST
