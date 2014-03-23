#ifdef GTEST
#include <gtest/gtest.h>
#include <VInt>
#include <VDesign>
#include <VDebugNew>

TEST( Design, instanceTest )
{
  {
    VInt& myInt = VInstance<VInt>::instance();
    int first = myInt; // 0
    EXPECT_TRUE( first == 0 );
    myInt = 1;
   }

  {
    VInt& myInt = VInstance<VInt>::instance();
    int second = myInt; // 1
    EXPECT_TRUE( second == 1 );
  }
}

class Log
{
private:
  Log()          { printf("Log::Log\n");  }
  virtual ~Log() { printf("Log::~Log\n"); }
public:
  static Log& instance()
  {
    static Log log;
    return log;
  }
  void write(char* msg)
  {
    printf("%s\n", msg);
  }
  static void dependency()
  {
    instance();
  }
};

class App : VDependencyWith<Log>
{
private:
  App()          { Log::instance().write("App::App");  }
  virtual ~App() { Log::instance().write("App::~App"); }
public:
  static App& instance()
  {
    static App app;
    return app;
  }
  void foo()
  {
    Log::instance().write("App::foo");
  }
};

TEST( Design, dependencyTest )
{
  //
  // App instance is called first in main application,
  // but Log instance must be initialized before App instance is initialized.
  //
  App::instance().foo();
}

TEST( Design, primitiveTest )
{
  typedef VPrimitive<int> MyInt;

  MyInt a = 1;
  MyInt b = 2;
  MyInt c = a + b;
  EXPECT_EQ(c, 3);
}

TEST( Design, baseTest )
{
  typedef VBase<int> MyInt;

  MyInt a = 1;
  MyInt b = 2;
  MyInt c = a + b;
  EXPECT_EQ(c, 3);
}

#endif // GTEST
