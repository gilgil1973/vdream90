#include "main.h"
#include <VApp>
#include <VDebugNew>

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
