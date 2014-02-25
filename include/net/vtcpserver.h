// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_TCP_SERVER_H__
#define __V_TCP_SERVER_H__

#include <VThread>
#include <VNetServer>
#include <VTcpSession>

// ----------------------------------------------------------------------------
// VTcpSessionThread
// ----------------------------------------------------------------------------
class VTcpServer;
class VTcpSessionThread : public VThread
{
public:
  VTcpServer*  tcpServer;  // read only
  VTcpSession* tcpSession; // read only

public:
  VTcpSessionThread(VTcpServer* owner, VTcpSession* tcpSession);
  virtual ~VTcpSessionThread();

protected:
  virtual void run();
};

// ----------------------------------------------------------------------------
// VTcpSessionThreadList
// ----------------------------------------------------------------------------
class VTcpSessionThreadList : public QList<VTcpSessionThread*>, public VLockable
{
};

// ----------------------------------------------------------------------------
// VTcpServer
// ----------------------------------------------------------------------------
class VTcpServer : public VNetServer, public VRunnable, protected VStateLockable
{
  Q_OBJECT

  friend class VTcpSessionThread;

public:
  VTcpServer(void* owner = NULL);
  virtual ~VTcpServer();

protected:
  virtual bool doOpen();
  virtual bool doClose();
  virtual int  doRead(char* buf, int size);
  virtual int  doWrite(char* buf, int size);

protected:
  VTcpSession* accept();
  virtual void run(); // for accept thread

public:
  /// read only(for accept)
  VTcpSession*          acceptSession;
  /// tcp client thread list
  VTcpSessionThreadList threadList;

public:
  /// Port No
  int port;
  // Local IP address or HostName
  QString localHost;

signals:
  void runned(VTcpSession* tcpSession);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public: // for VOptionable
  virtual void addOptionWidget(QLayout* layout);
  virtual void saveOptionDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_TCP_SERVER_H__
