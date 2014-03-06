// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_UDP_SERVER_H__
#define __V_UDP_SERVER_H__

#include <set>
#include <VNetServer>
#include <VUdpSession>

// ----------------------------------------------------------------------------
// VSockAddrList
// ----------------------------------------------------------------------------
struct SOCKADDR_IN_Compare
{
  bool operator() (const SOCKADDR_IN& lhs, const SOCKADDR_IN& rhs) const
  {
    return memcmp(&lhs, &rhs, sizeof(SOCKADDR_IN)) < 0;
  }
};

class VSockAddrList : public std::set<SOCKADDR_IN, SOCKADDR_IN_Compare>, public VLockable
{
public:
  VSockAddrList::iterator findBySockAddr(SOCKADDR_IN sockAddr);
  VSockAddrList::iterator findByInAddr(IN_ADDR inAddr);
};

// ----------------------------------------------------------------------------
// VUdpServer
// ----------------------------------------------------------------------------
class VUdpServer : public VNetServer, protected VStateLockable
{
public:
  VUdpServer(void* owner = NULL);
  virtual ~VUdpServer();

protected:
  virtual bool doOpen();
  virtual bool doClose();
  virtual int  doRead(char* buf, int size);
  virtual int  doWrite(char* buf, int size);

public:
  VUdpSession*  udpSession; // read only
  VSockAddrList sockAddrList;

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
private: // for VOptionable
  QLineEdit* lePort;
  QLineEdit* leLocalHost;
public: // for VOptionable
  virtual void optionAddWidget(QLayout* layout);
  virtual void optionSaveDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_UDP_SERVER_H__
