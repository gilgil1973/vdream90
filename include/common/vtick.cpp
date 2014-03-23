#include <VTick>
#include <QElapsedTimer>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// __VMyTimer__
// ----------------------------------------------------------------------------
class __VMyTimer__ : public QElapsedTimer
{
  friend VTick tick();

private:
  __VMyTimer__() { start(); }
};

// ----------------------------------------------------------------------------
// VTick
// ----------------------------------------------------------------------------
VTick tick()
{
  static __VMyTimer__ myTimer;
  return myTimer.elapsed();
}

#ifdef GTEST
#include <gtest/gtest.h>
#include <VSystem>
#include <iostream>

TEST ( Tick, tickTest )
{
  VTick beg = tick();
  std::cout << "beg=" << std::endl;
  const static int COUNT = 100;
  for (int i = 0; i < COUNT; i++)
  {
    VTick now = tick();
    std::cout << i << " " << now << std::endl;
    msleep(1);
  }
  VTick end = tick();
  std::cout << "end-beg=" << end - beg << std::endl;
}

#endif // GTEST
