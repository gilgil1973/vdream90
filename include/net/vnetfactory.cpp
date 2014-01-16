#include <VNetFactory>
#include <VTcpClient>
#include <VTcpServer>
#include <VUdpClient>
#include <VUdpServer>
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
}

VNet* VNetFactory::createByClassName(const QString& className)
{
  return VFactory<VNet>::createByClassName(className);
}

