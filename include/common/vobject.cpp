#include <VObject>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VObjectConnection
// ----------------------------------------------------------------------------
VObjectConnection::VObjectConnection(QByteArray signal, QObject* receiver, QByteArray slot)
{
  this->signal   = signal;
  this->receiver = receiver;
  this->slot     = slot;
}

VObjectConnection::VObjectConnection(const char* signal, QObject* receiver, const char* slot)
{
  this->signal   = signal;
  this->receiver = receiver;
  this->slot     = slot;
}

bool VObjectConnection::operator == (const VObjectConnection& r) const
{
  if (this->signal != r.signal)     return false;
  if (this->receiver != r.receiver) return false;
  if (this->slot != r.slot)         return false;
  return true;
}

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

bool VObject::connect(QObject* sender, const char* signal, QObject* receiver, const char* slot, Qt::ConnectionType type)
{
  //LOG_DEBUG("%s %s > %s %s %d", sender->metaObject()->className(), signal, receiver->metaObject()->className(), slot, (int)type); // gilgil temp 2014.03.10
  bool res = QObject::connect(sender, signal, receiver, slot, type);
  if (!res)
  {
    LOG_ERROR("VObject::connect(%s %s > %s %s %d) return false", sender->metaObject()->className(), signal, receiver->metaObject()->className(), slot, (int)type);
    return false;
  }
  VObject* vsender = dynamic_cast<VObject*>(sender);
  if (vsender != NULL)
  {
    VObjectConnection connection(signal, receiver, slot);
    vsender->connections.push_back(connection);
  }
  return res;
}

bool VObject::connect(QObject *sender, const QMetaMethod &signal, QObject *receiver, const QMetaMethod &slot, Qt::ConnectionType type)
{
  QByteArray baSignal = signal.methodSignature();
  baSignal = "2" + baSignal; // # define SIGNAL(a)   qFlagLocation("2"#a QLOCATION)
  QByteArray baSlot  = slot.methodSignature();
  baSlot = "1" + baSlot;    // # define SLOT(a)     qFlagLocation("1"#a QLOCATION)
  return VObject::connect(sender, baSignal.data(), receiver, baSlot.data(), type);
}

bool VObject::disconnect(QObject* sender, const char* signal, QObject* receiver, const char* slot)
{
  //LOG_DEBUG("%s %s > %s %s", sender->metaObject()->className(), signal, receiver->metaObject()->className(), slot); // gilgil temp 2014.03.10
  bool res = QObject::disconnect(sender, signal, receiver, slot);
  if (!res)
  {
    LOG_ERROR("VObject::disconnect(%s %s > %s %s) return false", sender->metaObject()->className(), signal, receiver->metaObject()->className(), slot);
    return false;
  }
  VObject* vsender = dynamic_cast<VObject*>(sender);
  if (vsender != NULL)
  {
    VObjectConnection connection(signal, receiver, slot);
    int index = vsender->connections.indexOf(connection);
    if (index != -1)
      vsender->connections.removeAt(index);
  }
  return res;
}

bool VObject::disconnect(QObject *sender, const QMetaMethod &signal, QObject *receiver, const QMetaMethod &slot)
{
  QByteArray baSignal = signal.methodSignature();
  baSignal = "2" + baSignal; // # define SIGNAL(a)   qFlagLocation("2"#a QLOCATION)
  QByteArray baSlot  = slot.methodSignature();
  baSlot = "1" + baSlot;     // # define SLOT(a)     qFlagLocation("1"#a QLOCATION)
  return VObject::disconnect(sender, baSignal.data(), receiver, baSlot.data());
}

QMetaMethod VObject::findMethod(QObject* object, QString methodName)
{
  if (object == NULL)
  {
    LOG_ERROR("object is null for method('%s')", qPrintable(methodName));
    QMetaMethod blankMethod;
    return blankMethod;
  }
  int _count = object->metaObject()->methodCount();
  for (int i = 0; i < _count; i++)
  {
    QMetaMethod res = object->metaObject()->method(i);
    QString signature = QString(res.methodSignature());
    if (signature == methodName)
    {
      return res;
    }
  }
  LOG_ERROR("can not find method('%s')", qPrintable(methodName));
  QMetaMethod blankMethod;
  return blankMethod;
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
