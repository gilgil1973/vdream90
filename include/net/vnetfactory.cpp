#include <VNetFactory>
#include <VTcpClient>
#include <VTcpServer>
#include <VUdpClient>
#include <VUdpServer>
#include <VSslClient>
#include <VSslServer>
#include <VWebProxy>
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
  VWebProxy httpProxy;
}

VNet* VNetFactory::createByClassName(const QString& className)
{
  return VFactory<VNet>::createByClassName(className);
}

