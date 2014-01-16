#include <QDebug>
#include "main.h"
#include <VSystem>
#include <VTick>
#include <VDebugNew>

TEST ( Tick, tickTest )
{
  VTick beg = tick();
  qDebug() << "beg=" << beg;
  const static int COUNT = 100;
  for (int i = 0; i < COUNT; i++)
  {
    VTick now = tick();
    qDebug() << i << now;
    msleep(1);
  }
  VTick end = tick();
  qDebug() << "end-beg=" << end - beg;
}
