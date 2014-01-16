// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_NET_SESSION_H__
#define __V_NET_SESSION_H__

#include <VNet>

// ----------------------------------------------------------------------------
// VNetSession
// ----------------------------------------------------------------------------
class VNetSession : public VNet
{
public:
  VNetSession(void* owner = NULL) : VNet(owner) { onceWriteSize = 0; }

public:
  int onceWriteSize;
};

#endif // __V_NET_CLIENT_H__
