// ----------------------------------------------------------------------------
//
// vdream Component Suite version 8.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __MAIN_H__
#define __MAIN_H__

#include <VTcpServer>
#include <VUdpServer>
#include <VThread>
#include <VLogFactory>
#include "param.h"

// ----------------------------------------------------------------------------
// App
// ----------------------------------------------------------------------------
class App : public VObject, public VRunnable
{
  Q_OBJECT

public:
  Param*      param;
  VNetServer* netServer;

  App();
  virtual ~App();
  void createNetServer();
  void terminate();

  void inputAndSend();

  virtual void run();

public slots:
  void runned(VTcpSession* tcpSession);
};

#endif // __MAIN_H__
