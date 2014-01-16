#include "mylog.h"

// ----------------------------------------------------------------------------
// MyLog
// ----------------------------------------------------------------------------
MyLog::MyLog()
{
  udpServer       = new VUdpServer;
  udpServer->port = VLogUdp::DEFAULT_PORT;
  autoOpen        = true;
  timeShow        = true;
}

MyLog::~MyLog()
{
  close();
  SAFE_DELETE(udpServer);
}

bool MyLog::doOpen()
{
  bool res = udpServer->open();
  if (!res)
  {
    error = udpServer->error;
    return false;
  }

  res = runThread().open();
  if (!res)
  {
    error = runThread().error;
    return false;
  }

  return true;
}

bool MyLog::doClose()
{
  udpServer->close();
  runThread().close();
  return true;
}

void MyLog::run()
{
  char buf[vd::DEFAULT_BUF_SIZE + 1];
  while (runThread().active())
  {
    int readLen = udpServer->udpSession->read(buf, vd::DEFAULT_BUF_SIZE);
    if (readLen == VERR_FAIL) continue;
    buf[readLen] = '\0';
    QString qs = QString::fromLocal8Bit(buf);
    emit sync.onMessage(qs);
  }
}

void MyLog::load(VXml xml)
{
  VObject::load(xml);
  udpServer->load(xml.gotoChild("udpServer"));
  runThread().load(xml.gotoChild("thread"));
  autoOpen = xml.getBool("autoOpen", autoOpen);
  timeShow = xml.getBool("timeShow", timeShow);
}

void MyLog::save(VXml xml)
{
  VObject::save(xml);
  udpServer->save(xml.gotoChild("udpServer"));
  runThread().save(xml.gotoChild("thread"));
  xml.setBool("autoOpen", autoOpen);
  xml.setBool("timeShow", timeShow);
}
