#include <VEventHandler>

// ----------------------------------------------------------------------------
// VDeleteObjectEvent
// ----------------------------------------------------------------------------
void VDeleteObjectEvent::process()
{
  // LOG_DEBUG("VDeleteObjectEvent::process object=%p", object); // gilgil temp 2013.10.21
  delete object;
}

// ----------------------------------------------------------------------------
// VEventHandler
// ----------------------------------------------------------------------------
VEventHandler::VEventHandler()
{
  // LOG_DEBUG("VEventHandler::VEventHandler %p", this); // gilgil temp 2012.08.23
}

VEventHandler::~VEventHandler()
{
  // LOG_DEBUG("VEventHandler::~VEventHandler %p", this); // gilgil temp 2012.08.23
  sysEvent.setEvent();
  close();
}
void VEventHandler::postEvent(VCustomEvent *event)
{
  VLock lock(cs);
  eventList.push_back(event);
  sysEvent.setEvent();
}

void VEventHandler::run()
{
  LOG_DEBUG("beg"); // gilgil temp 2012.08.23
  while (active())
  {
    sysEvent.wait();
    {
      VLock lock(cs);
      int _count = eventList.count();
      for (int i = 0; i < _count; i++)
      {
        VCustomEvent* e = eventList.at(i);
        e->process();
        if (e->autoDelete) delete e;
      }
      eventList.clear();
    }
  }

  LOG_DEBUG("end"); // gilgil temp 2012.08.23
}

VEventHandler& VEventHandler::instance()
{
  static VEventHandler g_instance;
  if (!g_instance.active()) g_instance.open();
  return g_instance;
}
