// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_SSL_SERVER_H__
#define __V_SSL_SERVER_H__

#include <VThread>
#include <VTcpServer>
#include <VSslSession>

// ----------------------------------------------------------------------------
// VSslSessionList
// ----------------------------------------------------------------------------
class VSslSessionList : public QList<VSslSession*>, public VLockable
{
};

// ----------------------------------------------------------------------------
// VSslServer
// ----------------------------------------------------------------------------
class VSslServer : public VTcpServer
{
  Q_OBJECT

public:
  VSslServer(void* owner = NULL);
  virtual ~VSslServer();

protected:
  virtual bool doOpen();
  virtual bool doClose();
  virtual int  doRead(char* buf, int size);
  virtual int  doWrite(char* buf, int size);

public:
  /// ssl client thread list
  VSslSessionList sslSessionList;

public:
  VSslMethodType methodType;
  QString        fileName;

protected:
  X509*           s_cert;
  EVP_PKEY*       s_key;
  bool            loadKey();
  bool            loadCert();
  bool            setCertKeyStuff();

protected:
  SSL_METHOD*     meth;
  SSL_CTX*        ctx;

protected slots:
  void myRun(VTcpSession* tcpSession);

signals:
  void runned(VSslSession* sslSession);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public: // for VOptionable
  virtual void addOptionWidget(QLayout* layout);
  virtual void saveOptionDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_SSL_SERVER_H__
