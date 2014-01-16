// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_EVENT_HANDLER_H__
#define __V_EVENT_HANDLER_H__

#include <VThread>

// ----------------------------------------------------------------------------
// VCustomEvent
// ----------------------------------------------------------------------------
class VCustomEvent
{
public:
  enum Type
  {
    DeleteObject = 0
  };

public:
  bool autoDelete;

public:
  VCustomEvent() { autoDelete = true; }

public:
  virtual Type type() = 0;
  virtual void process() = 0;
};

// ----------------------------------------------------------------------------
// VDeleteObjectEvent
// ----------------------------------------------------------------------------
class VDeleteObjectEvent : public VCustomEvent
{
public:
  QObject* object;

public:
  VDeleteObjectEvent(QObject* object) { this->object = object; }

public:
  virtual Type type()    { return DeleteObject; }
  virtual void process();
};

// ----------------------------------------------------------------------------
// VEventHandler
// ----------------------------------------------------------------------------
class VEventHandler : VThread
{
private: // singleton
  VEventHandler();
  virtual ~VEventHandler();

public:
  void postEvent(VCustomEvent*event);

protected:
  VCS    cs;
  VEvent sysEvent;
  QList<VCustomEvent*> eventList;

protected:
  virtual void run();

public:
  static VEventHandler& instance();
};

#endif // __V_EVENT_HANDLER_H__
