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
// VSslServerSession
// ----------------------------------------------------------------------------
class VSslServer;
class VSslServerSession : public VSslSession
{
public:
  VSslServerSession(void* owner = NULL);
  virtual ~VSslServerSession();

protected:
  virtual bool doOpen();
  virtual bool doClose();

protected:
  X509*     s_cert;
  EVP_PKEY* s_key;

  bool setup(QString fileName);
  bool loadKey(QString fileName);
  bool loadCrt(QString fileName);
  bool setCrtKeyStuff();
  static int ssl_servername_cb(SSL *s, int *ad, void *arg);
};

// ----------------------------------------------------------------------------
// VSslServerSessionList
// ----------------------------------------------------------------------------
class VSslServerSessionList : public QList<VSslServerSession*>, public VLockable
{
};

// ----------------------------------------------------------------------------
// VSslServer
// ----------------------------------------------------------------------------
class VSslServer : public VTcpServer
{
  Q_OBJECT

  friend class VSslServerSession;

public:
  VSslServer(void* owner = NULL);
  virtual ~VSslServer();

protected:
  virtual bool doOpen();
  virtual bool doClose();
  virtual int  doRead(char* buf, int size);
  virtual int  doWrite(char* buf, int size);

public:
  /// ssl client session list
  VSslServerSessionList sslSessionList;
  VCS certificateCs;

public:
  VSslMethodType methodType;
  QString        caPath;
  QString        defaultKeyCrtFileName;

protected:
  SSL_METHOD*     m_meth;
  SSL_CTX*        m_ctx;

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
