#include <gtest/gtest.h>
#include <VObject>
#include <VTcpClient>
#include <VTcpServer>

class MyTCPTest : public VObject
{
  Q_OBJECT

public:
  int        m_connectCnt;
  int        m_onceByteSize;
  int        m_loopCnt;
  int        m_readBufferSize;
  VTcpClient m_tcpClient;
  VTcpServer m_tcpServer;

public:
  MyTCPTest();

public:
  bool m_succeed;
  bool test();

protected:
  virtual bool doOpen()
  {
    return m_tcpServer.open();
  }

  virtual bool doClose()
  {
    m_tcpServer.close();
    return true;
  }

public slots:
  void run(VTcpSession* tcpSession);

public:
  virtual void load(VXml &xml)
  {
    m_connectCnt     = xml.getInt("connectCnt", m_connectCnt);
    m_onceByteSize   = xml.getInt("onceByteSize", m_onceByteSize);
    m_loopCnt        = xml.getInt("loopCnt", m_loopCnt);
    m_readBufferSize = xml.getInt("readBufferSize", m_readBufferSize);
    m_tcpClient.load(xml.gotoChild("tcpClient"));
    m_tcpServer.load(xml.gotoChild("tcpServer"));
  }

  virtual void save(VXml &xml)
  {
    xml.setInt("connectCnt", m_connectCnt);
    xml.setInt("onceByteSize", m_onceByteSize);
    xml.setInt("loopCnt", m_loopCnt);
    xml.setInt("readBufferSize", m_readBufferSize);
    m_tcpClient.save(xml.gotoChild("tcpClient"));
    m_tcpServer.save(xml.gotoChild("tcpServer"));
  }
};
