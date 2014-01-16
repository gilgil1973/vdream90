#include "tcptest.h"
#include <VXmlDoc>
#include <VDebugNew>

MyTCPTest::MyTCPTest()
{
  m_connectCnt     = 10;
  m_onceByteSize   = 10000;
  m_loopCnt        = 10;
  m_readBufferSize = 1000;
  m_tcpServer.port = vd::DEFAULT_PORT;
  m_tcpClient.host = "localhost";
  m_tcpClient.port = vd::DEFAULT_PORT;
  QObject::connect(&m_tcpServer, SIGNAL(runned(VTcpSession*)), this, SLOT(run(VTcpSession*)), Qt::DirectConnection);
  m_succeed          = true;
}

bool MyTCPTest::test()
{
  int connectCnt = 0;
  for (;connectCnt < m_connectCnt; connectCnt++)
  {
    if (!m_tcpClient.open())
    {
      LOG_FATAL("m_tcpClient.open return false %s", m_tcpClient.error.msg);
      m_succeed = false;
      break;
    }
    char* buf = new char[m_onceByteSize];
    memset(buf, 'A', m_onceByteSize);
    int loopCnt = 0;
    for (; loopCnt < m_loopCnt; loopCnt++)
    {
      int restLen = m_onceByteSize;
      while (restLen > 0)
      {
        int writeLen = m_tcpClient.write(buf, restLen);
        if (writeLen == VERR_FAIL)
        {
          LOG_FATAL("tcpClient.write return VERR_FAIL");
          m_succeed = false;
          break;
        }
        restLen -= writeLen;
        LOG_DEBUG("writeLen=%d restLen=%d", writeLen, restLen);
      }
      if (restLen < 0)
      {
        LOG_FATAL("restLen=%d", restLen);
        m_succeed = false;
        break;
      }
    }
    delete[] buf;
    if (loopCnt != m_loopCnt)
    {
      LOG_ERROR("loopCnt=%d m_loopCnt=%d", loopCnt, m_loopCnt);
    }
    // msleep(1000);
    if (!m_tcpClient.close())
    {
      LOG_FATAL("tcpClient.write return VERR_FAIL");
      m_succeed = false;
      break;
    }
  }
  LOG_INFO("connectCnt=%d m_connectCnt=%d", connectCnt, m_connectCnt);
  return m_succeed;
}

void MyTCPTest::run(VTcpSession* tcpSession)
{
  int totalReadLen = 0;
  char* buf = new char[m_readBufferSize];
  while (true)
  {
    int readLen = tcpSession->read(buf, m_readBufferSize);
    if (readLen == VERR_FAIL) break;
    totalReadLen += readLen;
    LOG_DEBUG("readLen=%d totalReadLen=%d", readLen, totalReadLen);
  }
  delete[] buf;
  if (totalReadLen != m_onceByteSize * m_loopCnt)
  {
    LOG_FATAL("totalReadLen(%d) != m_onceByteSize(%d) * m_loopCnt(%d)", totalReadLen, m_onceByteSize, m_loopCnt);
    m_succeed = false;
  }
}

TEST( TCP, localTest )
{
  MyTCPTest tcpTest;
  VXml xml = VXmlDoc::instance().root().gotoChilds("TCPTest/local");
  tcpTest.load(xml);

  EXPECT_TRUE( tcpTest.open()    );
  EXPECT_TRUE( tcpTest.test()    );
  EXPECT_TRUE( tcpTest.close()   );
  EXPECT_TRUE( tcpTest.m_succeed );
  // QCoreApplication::exec(); // gilgil temp 2012.08.17

  tcpTest.save(xml);
}
