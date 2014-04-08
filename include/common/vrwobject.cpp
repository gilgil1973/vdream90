#include <VRwObject>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VRwObject
// ----------------------------------------------------------------------------
int VRwObject::read(char* buf, int size)
{
  if (m_state != VState::Opened)
  {
    SET_DEBUG_ERROR(VError, qformat("not opened state(%s %s)", qPrintable(className()), qPrintable(name)), VERR_NOT_OPENED_STATE);
    return VERR_FAIL;
  }

  return doRead(buf, size);
}

int VRwObject::read(QByteArray& ba, int size)
{
  ba.resize(size);
  int readLen = read(ba.data(), ba.size());
  ba.resize(readLen);
  return readLen;
}

QByteArray VRwObject::read(int size)
{
  if (m_state != VState::Opened)
  {
    SET_DEBUG_ERROR(VError, qformat("not opened state(%s %s)", qPrintable(className()), qPrintable(name)), VERR_NOT_OPENED_STATE);
    return "";
  }

  QByteArray ba(size, Qt::Uninitialized);
  int readLen = doRead(ba.data(), ba.size());
  if (readLen == VERR_FAIL)
  {
    return "";
  }
  ba.resize(readLen);
  return ba;
}

int VRwObject::write(char* buf, int size)
{
  if (m_state != VState::Opened)
  {
    SET_DEBUG_ERROR(VError, qformat("not opened state(%s %s)", qPrintable(className()), qPrintable(name)), VERR_NOT_OPENED_STATE);
    return VERR_FAIL;
  }

  return doWrite((char*)buf, size);
}

int VRwObject::write(char* buf)
{
  int size = (int)strlen(buf);
  return write(buf, size);
}

int VRwObject::write(const char* buf)
{
  int size = (int)strlen(buf);
  return write(buf, size);
}

int VRwObject::write(const char* buf, int size)
{
  return write((char*)buf, size);
}

int VRwObject::write(QString& s)
{
  return write(qPrintable(s), s.size());
}

int VRwObject::write(QByteArray& ba)
{
  return write(ba.data(), ba.size());
}
