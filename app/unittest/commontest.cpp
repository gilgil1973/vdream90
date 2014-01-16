#include "main.h"
#include <VCommon>
#include <VDebugNew>

TEST( Common, versionTest )
{
  qDebug() << VDREAM_VERSION;
}

void testMacro()
{
  QString fileName = __FILENAME__;
  EXPECT_TRUE( fileName == "commontest.cpp" );

  QString funcName = __FUNCNAME__;
  EXPECT_TRUE( funcName == "testMacro" );
}

TEST( Common, macroTest )
{
  qDebug() << "__DATE__    =" << __DATE__;
  qDebug() << "__TIME__    =" << __TIME__;
  qDebug() << "__FILE__    =" << __FILE__;
  qDebug() << "__FUNCTION__=" << __FUNCTION__;
  qDebug() << "__FILENAME__=" << __FILENAME__;
  qDebug() << "__FUNCNAME__=" << __FUNCNAME__;

  testMacro();
}

TEST( Common, classNameTest )
{
  class MyObject {};
  MyObject obj;
  QString className = CLASS_NAME(obj);
  qDebug() << "MyObject class name is " << className;
  EXPECT_TRUE( className == "MyObject" );
}
