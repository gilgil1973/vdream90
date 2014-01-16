// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_NET_FACTORY_H__
#define __V_NET_FACTORY_H__

#include <VFactory>
#include <VNet>

// ----------------------------------------------------------------------------
// VNetFactory
// ----------------------------------------------------------------------------
class VNetFactory : public VFactory<VNet>
{
public:
  static void explicitLink();
  static VNet* createByClassName(const QString& className);
};

#endif // __V_NET_FACTORY_H__
