#include <stdio.h>
#include "main.h"

// ----------------------------------------------------------------------------
// App
// ----------------------------------------------------------------------------
App::App()
{
  param     = new Param;
  netServer = NULL;
}

App::~App()
{
  terminate();
}

void App::terminate()
{
  if (netServer != NULL) netServer->close();
  runThread().close();
  SAFE_DELETE(param);
  SAFE_DELETE(netServer);
}

void App::inputAndSend()
{
  LOG_DEBUG("beg");

  char* buf = new char[param->bufSize];
  while (true)
  {
    char* p = gets_s(buf, param->bufSize);
    if (p == NULL) break;
    int readLen = (int)strlen(p);
    if (readLen == 0) continue;
    buf[readLen] = '\0';
    int writeLen = netServer->write(buf, readLen);
    if (writeLen == VERR_FAIL) break;
  }
  delete[] buf;

  LOG_DEBUG("end");
}

void App::createNetServer()
{
  if (param->netType == ntTCP)
  {
    VTcpServer* tcpServer = new VTcpServer(this);
    VObject::connect(tcpServer, SIGNAL(runned(VTcpSession*)), this, SLOT(runned(VTcpSession*)), Qt::DirectConnection);
    tcpServer->port = param->port;
    if (param->localHost != "") tcpServer->localHost = param->localHost;
    netServer = tcpServer;
  } else // ntUDP
  {
    VUdpServer* udpServer = new VUdpServer(this);
    udpServer->port = param->port;
    if (param->localHost != "") udpServer->localHost = param->localHost;
    netServer = udpServer;
  }
}

void App::run() // udp
{
  LOG_DEBUG("beg");

  VUdpServer*  udpServer = (VUdpServer*)netServer;
  VSockAddrList& sockAddrList = udpServer->sockAddrList; // for abbreviate
  VUdpSession* udpSession = udpServer->udpSession; // for abbreviate

  char* buf = new char[param->bufSize];
  while (runThread().active())
  {
    int readLen = udpSession->read(buf, param->bufSize);
    LOG_DEBUG("readLen=%d", readLen); // gilgi temp 2009.08.16

    //
    // Delete session
    //
    if (readLen == VERR_FAIL)
    {
      printf("%s\n", udpSession->error.msg);

      if (udpSession->error.code == 10054/*WSAECONNRESET*/) // ICMP Destination Unreachable // gilgil temp 2014.04.18
      {
        while (true)
        {
          VSockAddrList::iterator it = sockAddrList.findByInAddr(udpSession->addr.sin_addr);
          if (it == sockAddrList.end()) break;
          sockAddrList.erase(it);
        }
        LOG_DEBUG("count=%d", sockAddrList.size()); // gilgi temp 2009.08.16
      } else // other error
      {
        VSockAddrList::iterator it = sockAddrList.findBySockAddr(udpSession->addr);
        if (it != sockAddrList.end())
          sockAddrList.erase(it);
        LOG_DEBUG("count=%d", sockAddrList.size()); // gilgi temp 2009.08.16
      }
      udpSession->error.clear();
      continue;
    }

    //
    // Add session
    //
    if (sockAddrList.findBySockAddr(udpSession->addr) == sockAddrList.end())
    {
      SOCKADDR_IN* newSockAddr = new SOCKADDR_IN;
      *newSockAddr = udpSession->addr;
      sockAddrList.insert(*newSockAddr);
      LOG_DEBUG("count=%d", sockAddrList.size()); // gilgi temp 2009.08.16
    }

    buf[readLen] = '\0';
    int writeLen = printf_s("%s\n", buf, readLen);
    if (writeLen < 0) break;
    if (param->echo)
    {
      if (param->echoBroadcast)
      {
        udpServer->write(buf, readLen); // do not check result
      } else
      {
        writeLen = udpSession->write(buf, readLen);
        if (writeLen == VERR_FAIL) break;
      }
    }

  }
  LOG_DEBUG("end");
}

void App::runned(VTcpSession* tcpSession) // tcp
{
  LOG_DEBUG("beg");

  VTcpServer* tcpServer = (VTcpServer*)tcpSession->owner;
  printf("connected %s\n", qPrintable(tcpSession->getRemoteIP().str()));

  char* buf = new char[param->bufSize];
  while (true)
  {
    int readLen = tcpSession->read(buf, param->bufSize);
    if (readLen == VERR_FAIL)
    {
      printf("%s\n", tcpSession->error.msg);
      break;
    }
    buf[readLen] = '\0';
    int writeLen = printf_s("%s\n", buf, readLen);
    if (writeLen < 0) break;
    if (param->echo)
    {
      if (param->echoBroadcast)
      {
        tcpServer->write(buf, readLen); // do not check result
      } else
      {
        writeLen = tcpSession->write(buf, readLen);
        if (writeLen == VERR_FAIL) break;
      }
    }
  }
  delete[] buf;

  printf("disconnected %s\n", qPrintable(tcpSession->getRemoteIP().str()));
  LOG_DEBUG("end");
}

// ----------------------------------------------------------------------------
// main
// ----------------------------------------------------------------------------
void usage()
{
  printf("Network Server verion 5.0\n");
  printf("Copyright (c) NetworkLab Corporation All rights reserved\n");
  printf("\n");
  printf("nc [tcp|udp] <port> [option]\n");
  printf("\n");
  printf("  tcp  : use tcp protocol (default)\n");
  printf("  udp  : use udp protocol\n");
  printf("\n");
  printf("  port : local port\n");
  printf("\n");
  printf("  -lh  : local host (default = \"\")\n");
  printf("\n");
  printf("  -bs <buf size> : set read and write buffer size(default %d)\n", DEFAULT_BUF_SIZE);
  printf("  -ne  : do not echo received message\n");
  printf("  -eb  : echo broadcast received message\n");
  printf("  -log <log name>: change log\n");
  printf("\n");
  printf("example\n");
  printf("\n");
  printf("  ns 80\n");
  printf("  ns udp 53\n");
  printf("  ns 80 -lh 1.2.3.3\n");
  printf("  ns 1.2.3.4 80 -lp 1025\n");
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

  app.createNetServer();
  if (!app.netServer->open())
  {
    printf("%s\n", app.netServer->error.msg);
    return -1;
  }
  if (app.param->netType == ntUDP)
  {
    app.runThread().open();
  }

  app.inputAndSend();
  app.terminate();

  return 0;
}
