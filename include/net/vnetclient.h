// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_NET_CLIENT_H__
#define __V_NET_CLIENT_H__

#include <VNet>
#include <VObjectWidget>

// ----------------------------------------------------------------------------
// VNetClient
// ----------------------------------------------------------------------------
class VNetClient : public VNet, public VShowOption
{
public:
  VNetClient(void* owner = NULL);
  virtual ~VNetClient();

public:
  int onceWriteSize;

public:
  virtual void load(VXml &xml);
  virtual void save(VXml &xml);

#ifdef QT_GUI_LIB
public: // for VShowOption
  virtual void addOptionWidget(QLayout* layout);
  virtual void saveOption(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_NET_CLIENT_H__
