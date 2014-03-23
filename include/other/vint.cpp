#include <VInt>
#include <VDebugNew>

#ifdef GTEST
#include <gtest/gtest.h>

TEST ( Int, intTest )
{
  //
  // default ctor
  //
  {
    VInt i;
  }

  //
  // conversion ctor
  //
  {
    VInt i(1);
  }

  {
    VInt i(1);
    VInt j(2);
  }

  //
  // conversion ctor
  //
  {
    VInt i = 1;
    VInt j = 2;

    EXPECT_TRUE( i <  j );
    EXPECT_TRUE( i <= j );
    EXPECT_TRUE( i != j );
  }

  //
  // copy ctor
  //
  {
    VInt i = 1; // argument ctor
    VInt j = j; // copy ctor
  }
}

#endif // GTEST
