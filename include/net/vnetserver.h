// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_NET_SERVER_H__
#define __V_NET_SERVER_H__

#include <VNet>
#include <VObjectWidget>

// ----------------------------------------------------------------------------
// VNetServer
// ----------------------------------------------------------------------------
class VNetServer : public VNet, public VOptionable
{
public:
  VNetServer(void* owner = NULL);
  virtual ~VNetServer();

public:
  int onceWriteSize;

public:
  virtual void load(VXml &xml);
  virtual void save(VXml &xml);

#ifdef QT_GUI_LIB
public: // for VOptionable
  virtual void optionAddWidget(QLayout* layout);
  virtual void optionSaveDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_NET_CLIENT_H__
