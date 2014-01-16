#include "main.h"
#include <VSystem>
#include <VTick>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// msleep
// ----------------------------------------------------------------------------
TEST( System, msleepTest )
{
  VTick begTick = tick();
  msleep(1000); // 1 sec
  VTick endTick = tick();

  EXPECT_TRUE( endTick > begTick );
  EXPECT_TRUE( endTick - begTick >= 900 && endTick - begTick <= 1100 );
}

// ----------------------------------------------------------------------------
// sleep
// ----------------------------------------------------------------------------
TEST( System, sleepTest )
{
  VTick begTick = tick();
  sleep(1); // 1 sec
  VTick endTick = tick();

  EXPECT_TRUE( endTick > begTick );
  EXPECT_TRUE( endTick - begTick >= 900 && endTick - begTick <= 1100 );
}

// ----------------------------------------------------------------------------
// usleep
// ----------------------------------------------------------------------------
TEST( System, usleepTest )
{
  VTick begTick = tick();
  usleep(1000000); // 1 sec
  VTick endTick = tick();

  EXPECT_TRUE( endTick > begTick );
  EXPECT_TRUE( endTick - begTick >= 900 && endTick - begTick <= 1100 );
}
