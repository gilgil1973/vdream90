// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_UDP_CLIENT_H__
#define __V_UDP_CLIENT_H__

#include <VNetClient>
#include <VUdpSession>

// ----------------------------------------------------------------------------
// VUdpClient
// ----------------------------------------------------------------------------
class VUdpClient : public VNetClient
{
public:
  VUdpClient(void* owner = NULL);
  virtual ~VUdpClient();

protected:
  virtual bool doOpen();
  virtual bool doClose();
  virtual int  doRead(char* buf, int size);
  virtual int  doWrite(char* buf, int size);

public:
  /// read only
  VUdpSession *udpSession;

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public: // for VOptionable
  virtual void optionAddWidget(QLayout* layout);
  virtual void optionSaveDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_UDP_CLIENT_H__
