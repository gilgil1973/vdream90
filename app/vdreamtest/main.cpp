#include <VTcpClient>
#include <VTcpServer>
#include <VUdpClient>
#include <VUdpServer>
#include <VLogUdp>

int main()
{
  {
    VTcpClient c;
    c.host = "www.gilgil.net";
    c.port = 0;
    c.open();
  }

  {
    VUdpClient c;
    c.host = "127.0.0.1";
    c.port = 8908;
    c.open();
    c.write("test");
    c.close();
  }

  {
    VLogUdp log;
    log.debug("test");
  }
}
