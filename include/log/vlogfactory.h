// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_LOG_FACTORY_H__
#define __V_LOG_FACTORY_H__

#include <VFactory>
#include <VLog>

// ----------------------------------------------------------------------------
// VLogFactory
// ----------------------------------------------------------------------------
class VLogFactory : public VFactory<VLog>
{
public:
  static void  explicitLink();
  static VLog* createByClassName(const QString& className);
  static VLog* createByDefaultDoc(const QString& path);
  static VLog* createByURI(const QString& uri);
  static VLog* createDefaultLog();
};

#endif // __V_LOG_FACTORY_H__
