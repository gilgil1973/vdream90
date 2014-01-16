#include "main.h"
#include <VException>
#include <VDebugNew>

int divideByZero()
{
  int i, j, k;
  int debug = 0;
  TRY
  {
    i = 5;
    j = 0;
    k = i / j; // divide by zero occurred.
    printf("%d / %d = %d\n", i, j, k);
    debug = 1;
  } EXCEPT
  {
    LOG_FATAL("divide by zero occurred");
    debug = 2;
  }
  return debug;
}

int accessViolation()
{
  char* p = NULL;
  int debug = 0;
  TRY
  {
    *p = 'a';
    debug = 1;
  } EXCEPT
  {
    LOG_FATAL("access violcation occurred");
    debug = 2;
  }
  return debug;
}

TEST( Exception, exceptionTest )
{
  int debug;

  debug = divideByZero();
  EXPECT_EQ( debug, 2 );

  debug = accessViolation();
  EXPECT_EQ( debug, 2 );
}
