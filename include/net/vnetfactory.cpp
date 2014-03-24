#include <VNetFactory>
#include <VTcpClient>
#include <VTcpServer>
#include <VUdpClient>
#include <VUdpServer>
#include <VSslClient>
#include <VSslServer>
#include <VHttpProxy>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VNetFactory
// ----------------------------------------------------------------------------
void VNetFactory::explicitLink()
{
  VTcpClient tcpClient;
  VTcpServer tcpServer;
  VUdpClient udpClient;
  VUdpServer udpServer;
  VSslClient sslClient;
  VSslServer sslServer;
  VHttpProxy httpProxy;
}

VNet* VNetFactory::createByClassName(const QString& className)
{
  return VFactory<VNet>::createByClassName(className);
}

