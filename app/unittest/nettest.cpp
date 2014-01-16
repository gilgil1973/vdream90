#include "main.h"
#include <VNet>
#include <VDebugNew>

TEST( Net, ipTest )
{
  Ip ip = (QString)"127.0.0.1";
  QString s1 = ip.str();
  QString s2 = "127.0.0.1";
  EXPECT_EQ( s1, s2 );
}

TEST( Net, isIPV4AddressTest )
{
  EXPECT_TRUE( VNet::isIPV4Address("1.2.3.4") );
  EXPECT_FALSE( VNet::isIPV4Address("1.2.3.4.5") );
  EXPECT_FALSE( VNet::isIPV4Address("www.google.com") );
  EXPECT_FALSE( VNet::isIPV4Address("www.gilgil.net") );
}

class MyNet : public VNet // to initialize VNet
{
protected:
  virtual bool doOpen()  { return true; }
  virtual bool doClose() { return true; }
  virtual int  doRead(char* buf,  int size) {  Q_UNUSED(buf) Q_UNUSED(size) return 0; }
  virtual int  doWrite(char* buf, int size) {  Q_UNUSED(buf) Q_UNUSED(size) return 0; }
};

TEST( Net, resolveTest )
{
  MyNet myNet;
  {
    Ip ip = (quint32)0x7F000001;
    //Ip ip = VNet::resolve("127.0.0.1");
    LOG_DEBUG("ip for 127.0.0.1 is %s", qPrintable(ip.str()));
    EXPECT_EQ( ip, 0x7F000001 );
  }

  {
    Ip ip = VNet::resolve("www.google.com");
    LOG_DEBUG("ip for www.google.com is %s", qPrintable(ip.str()));
    EXPECT_TRUE( ip != 0 );
  }
}

