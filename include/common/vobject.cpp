#include <VObject>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VObject
// ----------------------------------------------------------------------------
VObject::VObject(void* owner) : QObject((QObject*)owner) // gilgil temp 2012.05.29
{
  this->owner = owner;
  m_state     = VState::Closed;
  tag         = 0;
  name        = "";
  error.clear();
}

VObject::~VObject()
{
  if (m_state != VState::Closed)
  {
    LOG_FATAL("%s close must be called in descendant of VObject(state=%s) %p", qPrintable(name), qPrintable(m_state.str()), this);
  }
}

bool VObject::open()
{
  if (m_state != VState::Closed)
  {
    SET_ERROR(VError, qformat("not closed state(%s) %s %s", qPrintable(m_state.str()), qPrintable(className()), qPrintable(name)), VERR_NOT_CLOSED_STATE);
    return false;
  }
  if (name == "") name = className();
  m_state = VState::Opening;
  tag     = 0;
  error.clear();

  bool res = false;
  try
  {
    res = doOpen();
  }
  VCATCH

  if (!res)
  {
    doClose();
    m_state = VState::Closed;
    return false;
  }

  m_state = VState::Opened;
  emit opened();
  return true;
}

bool VObject::close()
{
  if (m_state != VState::Opened)
  {
    return false; // no action
  }

  m_state = VState::Closing;

  bool res = false;
  try
  {
    res = doClose();
  }
  VCATCH

  if (!res)
  {
    // m_state = VState::Closed; // do not change state if doClose() return false;
    return false;
  }

  m_state = VState::Closed;
  emit closed();
  return true;
}

bool VObject::close(bool wait, VTimeout timeout)
{
  bool res = close();
  if (res && wait)
    res = this->wait(timeout);
  return res;
}

bool VObject::wait(VTimeout timeout)
{
  Q_UNUSED(timeout);
  return true;
}

bool VObject::doOpen()
{
  SET_ERROR(VError, qformat("virtual function call %s %s", qPrintable(className()), qPrintable(name)), VERR_VIRTUAL_FUNCTION_CALL);
  LOG_FATAL("virtual function call error %s %s", qPrintable(className()), qPrintable(name));
  return false;
}

bool VObject::doClose()
{
  SET_ERROR(VError, qformat("virtual function call %s %s", qPrintable(className()), qPrintable(name)), VERR_VIRTUAL_FUNCTION_CALL);
  LOG_FATAL("virtual function call error %s %s", qPrintable(className()), qPrintable(name));
  return false;
}

void VObject::load(VXml xml)
{
  name = xml.getStr("name", name);
}

void VObject::save(VXml xml)
{
  xml.setStr("_class", className());
  if (name != "" && name != className()) xml.setStr("name", name);
}
