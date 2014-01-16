#include "main.h"
#include <VError>
#include <VLog>
#include <VDebugNew>

#ifdef _MSC_VER
#include <windows.h> // gilgil temp 2012.04.30 for GetLastError used in VCATCH_STD
#endif // _MSC_VER

TEST( Error, defaultTest )
{
  VError error;
  QString className = error.className();
  EXPECT_TRUE( className == "VError" );
  QString s = error.msg;
  EXPECT_TRUE( s == "" );
  EXPECT_TRUE( error.code == VERR_OK );
}

TEST( Error, classNameTest )
{
  VDECLARE_ERROR_CLASS(MyError)

  {
    MyError myError;
    QString className = CLASS_NAME(myError);
    EXPECT_TRUE( className == "MyError" );
  }

  {
    VError error;
    QString className = error.className();
    EXPECT_TRUE( className == "VError" );

    MyError myError;
    className = myError.className();
    EXPECT_TRUE( className == "MyError");

    error = myError;
    className = error.className();
    EXPECT_TRUE( className == "MyError" );
  }
}

class MyErrorObject
{
public:
  VError error;
  void foo1()
  {
    setError<VError>(this->error, "foo1 error", VERR_FAIL);
  }

  void foo2()
  {
    SET_ERROR(VError, "foo2 error", VERR_UNKNOWN);
  }

  void foo3()
  {
    SET_DEBUG_ERROR(VError, "foo3 error", VERR_NOT_SUPPORTED);
  }

  void testCatch() // gilgil temp 2012.04.30
  {
    try
    {
#ifdef WIN32
      throw std::exception("std exception");
#endif // WIN32
#ifdef linux
      throw std::exception();
#endif // linux
    }
    VCATCH;
  }
};

TEST( Error, setErrorTest )
{
  {
    MyErrorObject obj;
    obj.foo1();
    QString s = obj.error.msg;
    EXPECT_TRUE( s == "foo1 error" );
    EXPECT_TRUE( obj.error.code == VERR_FAIL );
  }

  {
    MyErrorObject obj;
    obj.foo2();
    QString s = obj.error.msg;
    EXPECT_TRUE( s == "foo2 error" );
    EXPECT_TRUE( obj.error.code == VERR_UNKNOWN );
  }

  {
    MyErrorObject obj;
    obj.foo3();
    QString s = obj.error.msg;
    EXPECT_TRUE( s == "foo3 error" );
    EXPECT_TRUE( obj.error.code == VERR_NOT_SUPPORTED );
  }
}

TEST( Error, catchTest )
{
  MyErrorObject obj;
  obj.testCatch();
}
