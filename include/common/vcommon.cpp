#include <VCommon>
#include <VDebugNew>

#ifdef linux
#include <cxxabi.h> // for __cxa_demangle
#endif // lunux

#include <VDebugNew>

// ----------------------------------------------------------------------------
// vdream Version
// ----------------------------------------------------------------------------
#ifdef _DEBUG
const char* VDREAM_VERSION = "VDream 9.0 Debug   Build( "__DATE__ " " __TIME__" )";
#else // RELEASE
const char* VDREAM_VERSION = "VDream 9.0 Release Build( "__DATE__ " " __TIME__" )";
#endif // _DEBUG

// ----------------------------------------------------------------------------
// __FILENAME__ and __FUNCNANE__ macros
// ----------------------------------------------------------------------------
const char* __extractFileName__(const char* fileName)
{
#ifdef WIN32
  const char* p1 = strrchr(fileName, '\\');
  const char* p2 = strrchr(fileName, '/');
  const char* p  = p1 > p2 ? p1 : p2;
#endif // WIN32
#ifdef linux
  const char* p = strrchr(fileName, '/');
#endif // linux
  return (p == NULL ? fileName : p + 1);
}

const char* __extractFuncName__(const char* funcName)
{
#ifdef WIN32
  const char* p = strrchr(funcName, ':');
  return (p == NULL ? funcName : p + 1);
#endif // WIN32
#ifdef linux
  return funcName;
#endif // linux
}

// ----------------------------------------------------------------------------
// Class
// ----------------------------------------------------------------------------
QString getClassName(const char* value)
{
  QString res = value;
#ifdef _MSC_VER
  if (res.left(6) == "class ")  res = res.mid(6);
  else
  if (res.left(7) == "struct ") res = res.mid(7);
#endif // _MSC_VER

#ifdef linux
  int status;
  char *realname = abi::__cxa_demangle(value, 0, 0, &status);
  res = realname;
#endif // linux

  int pos = res.lastIndexOf("::");     // gilgil temp 2012.04.16
  if (pos != -1) res = res.mid(pos + 2);
  return res;
}

// ----------------------------------------------------------------------------
// VState
// ----------------------------------------------------------------------------
VState::VState(const QString s)
{
  if (s == "None")         value = None;
  else if (s == "Closed")  value = Closed;
  else if (s == "Opening") value = Opening;
  else if (s == "Opened")  value = Opened;
  else if (s == "Closing") value = Closing;
  else value = None;
}

QString VState::str() const
{
  QString res;
  switch (value)
  {
    case None:    res = "None";    break;
    case Closed:  res = "Closed";  break;
    case Opening: res = "Opening"; break;
    case Opened:  res = "Opened";  break;
    case Closing: res = "Closing"; break;
    default:      res = "None";    break;
  }
  return res;
}

// ----------------------------------------------------------------------------
// VMode
// ----------------------------------------------------------------------------
VMode::VMode(const QString s)
{
  if (s == "None")           value = None;
  else if (s == "Read")      value = Read;
  else if (s == "Write")     value = Write;
  else if (s == "ReadWrite") value = ReadWrite;
  else value = None;
}

QString VMode::str() const
{
  QString res;
  switch (value)
  {
  case None      : res = "None";      break;
  case Read      : res = "Read";      break;
  case Write     : res = "Write";     break;
  case ReadWrite : res = "ReadWrite"; break;
  }
  return res;
}

#ifdef GTEST
#include <gtest/gtest.h>

TEST( Common, versionTest )
{
  qDebug() << VDREAM_VERSION;
}

void testMacro()
{
  QString fileName = __FILENAME__;
  EXPECT_TRUE( fileName == "vcommon.cpp" );

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

#endif // GTEST
