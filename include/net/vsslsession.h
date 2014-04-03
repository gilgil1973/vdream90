// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_SSL_SESSION_H__
#define __V_SSL_SESSION_H__

#include <VTcpSession>
#include <VSslCommon>

// ----------------------------------------------------------------------------
// VSslSession
// ----------------------------------------------------------------------------
class VSslSession : public VNetSession, protected VStateLockable
{
  friend class VSslClient;

public:
  VSslSession(void* owner = NULL);
  virtual ~VSslSession();

protected:
  virtual bool doOpen();
  virtual bool doClose();
  virtual int  doRead(char* buf, int size);
  virtual int  doWrite(char* buf, int size);

public:
  SSL* con;
  BIO* sbio;

public:
  // SOCKET       handle;     // reference, used in open // gilgil temp 2014.04.03
  VTcpSession* tcpSession; // reference
  SSL_CTX*     ctx;        // reference

protected:
  static const int VSSL_SESSION_IDENTIFY_INDEX = 0;

public:
  Ip  getLocalIP();
  Ip  getRemoteIP();
  int getLocalPort();
  int getRemotePort();

public:
  bool operator == (const VSslSession& rhs) const;
};

#endif // __V_SSL_SESSION_H__

