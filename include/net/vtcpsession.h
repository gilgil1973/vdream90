// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_TCP_SESSION_H__
#define __V_TCP_SESSION_H__

#include <VNetSession>

// ----------------------------------------------------------------------------
// VTcpSession
// ----------------------------------------------------------------------------
class VTcpSession : public VNetSession, protected VStateLockable
{
  friend class VTcpClient;

public:
  VTcpSession(void* owner = NULL);
  virtual ~VTcpSession();

protected:
  virtual bool doOpen();
  virtual bool doClose();
  virtual int  doRead(char* buf, int size);
  virtual int  doWrite(char* buf, int size);

public:
  /// socket handle. read only(set in other class)
  SOCKET      handle;
  /// SOCKADDR_IN structure. read only(set in other class)
  SOCKADDR_IN addr;

public:
  Ip  getLocalIP();
  Ip  getRemoteIP();
  int getLocalPort();
  int getRemotePort();

public:
  bool operator == (const VTcpSession& rhs) const;
};

#endif // __V_TCP_SESSION_H__
