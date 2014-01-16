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

#include <VTcpClient>
#include <VUdpClient>
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
  VNetClient* netClient;

  App();
  virtual ~App();
  void createNetClient();
  void terminate();

  void inputAndSend();
  void recvAndOutput();

  virtual void run();
};

#endif // __MAIN_H__
