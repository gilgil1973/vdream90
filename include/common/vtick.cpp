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
