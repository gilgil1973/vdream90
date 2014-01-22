// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_TCP_CLIENT_H__
#define __V_TCP_CLIENT_H__

#include <VNetClient>
#include <VTcpSession>

// ----------------------------------------------------------------------------
// VTcpClient
// ----------------------------------------------------------------------------
class VTcpClient : public VNetClient
{
public:
  VTcpClient(void* owner = NULL);
  virtual ~VTcpClient();

protected:
  virtual bool doOpen();
  virtual bool doClose();
  virtual int  doRead(char* buf, int size);
  virtual int  doWrite(char* buf, int size);

protected:
  /// IP address of host(set when open is called)
  Ip m_ip;

public:
  /// read only
  VTcpSession *tcpSession;

public:
  /// IP address or HostName
  QString host;
  /// IP address(read only. set when open is completed successfully)
  Ip  ip() { return m_ip; }
  /// Port No
  int port;
  /// Local IP address or HostName
  QString localHost;
  /// Local Port No
  int localPort;

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public: // for VOptionable
  virtual void addOptionWidget(QLayout* layout);
  virtual void saveOptionDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_TCP_CLIENT_H__
