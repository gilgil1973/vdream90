#include <stdio.h>
#include "main.h"

// ----------------------------------------------------------------------------
// App
// ----------------------------------------------------------------------------
App::App()
{
  param     = new Param;
  netClient = NULL;
}

App::~App()
{
  terminate();
}

void App::terminate()
{
  if (netClient != NULL) netClient->close();
  runThread().close();
  SAFE_DELETE(param);
  SAFE_DELETE(netClient);
}

void App::createNetClient()
{
  if (param->netType == ntTCP)
  {
    VTcpClient* tcpClient = new VTcpClient(this);
    tcpClient->host = param->host;
    tcpClient->port = param->port;
    if (param->localHost != "") tcpClient->localHost = param->localHost;
    if (param->localPort != 0)  tcpClient->localPort = param->localPort;
    netClient = tcpClient;
  } else // ntUDP
  {
    VUdpClient* udpClient = new VUdpClient(this);
    udpClient->host = param->host;
    udpClient->port = param->port;
    if (param->localHost != "") udpClient->localHost = param->localHost;
    if (param->localPort != 0)  udpClient->localPort = param->localPort;
    netClient = udpClient;
  }
}

void App::inputAndSend()
{
  LOG_DEBUG("beg");

  char* buf = new char[param->bufSize];
  while (runThread().active())
  {
    char* p = gets_s(buf, param->bufSize);
    if (p == NULL) break;
    int readLen = (int)strlen(p);
    if (readLen == 0) continue;
    buf[readLen] = '\0';
    int writeLen = netClient->write(buf, readLen);
    if (writeLen == VERR_FAIL) break;
  }
  delete[] buf;

  LOG_DEBUG("end");
}

void App::recvAndOutput()
{
  LOG_DEBUG("beg");
  if (IS_CLASS(netClient, VTcpClient*))
  {
    VTcpClient* tcpClient = (VTcpClient*) netClient;
    printf("connected %s\n", qPrintable(tcpClient->tcpSession->getRemoteIP().str()));
  }

  char* buf = new char[param->bufSize];
  while (runThread().active())
  {
    int readLen = netClient->read(buf, param->bufSize);
    if (readLen == VERR_FAIL)
    {
      printf("%s\n", netClient->error.msg);
      break;
    }
    buf[readLen] = '\0';
    int writeLen = printf_s("%s\n", buf, readLen);
    if (writeLen < 0) break;
  }
  delete[] buf;

  if (IS_CLASS(netClient, VTcpClient*))
  {
    VTcpClient* tcpClient = (VTcpClient*) netClient;
    printf("disconnected %s\n", qPrintable(tcpClient->tcpSession->getRemoteIP().str()));
  }
  LOG_DEBUG("end");
  exit(0); // force program termination if network is disconnected.
};

void App::run()
{
  recvAndOutput();
}

// ----------------------------------------------------------------------------
// main
// ----------------------------------------------------------------------------
void usage()
{
  printf("Network Client verion 5.0\n");
  printf("Copyright (c) NetworkLab Corporation All rights reserved\n");
  printf("\n");
  printf("nc [tcp|udp] <host> <port> [option]\n");
  printf("\n");
  printf("  tcp  : use tcp protocol (default)\n");
  printf("  udp  : use udp protocol\n");
  printf("\n");
  printf("  host : remote ip or host name\n");
  printf("  port : remote port\n");
  printf("\n");
  printf("  -lh  : local host (default = \"\")\n");
  printf("  -lp  : local port (default = 0)\n");
  printf("\n");
  printf("  -bs <buf size> : set read and write buffer size(default %d)\n", DEFAULT_BUF_SIZE);
  printf("  -log <log name>: change log\n");
  printf("\n");
  printf("example\n");
  printf("\n");
  printf("  nc 1.2.3.4 80\n");
  printf("  nc udp 1.2.3.4 53\n");
  printf("  nc 1.2.3.4 80 -lh 1.2.3.3\n");
  printf("  nc 1.2.3.4 80 -lp 1025\n");
}

App app;
int main(int argc, char* argv[])
{
  if (argc == 1)
  {
    usage();
    return -1;
  }

  if (!app.param->parse(argc, argv)) return -1;

  if (app.param->log != "")
  {
    VLog* log = VLogFactory::createByURI(app.param->log);
    if (log == NULL)
    {
      printf("can not create log(%s)\n", qPrintable(app.param->log));
      return -1;
    }
    VLog::changeLog(log);
  }

  app.createNetClient();
  if (!app.netClient->open())
  {
    printf("%s\n", app.netClient->error.msg);
    return -1;
  }

  app.runThread().open(); // recv and output
  app.inputAndSend();
  app.terminate();

  return 0;
}
