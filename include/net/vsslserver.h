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
  friend class VSslServer;

public:
  VSslServerSession(void* owner = NULL);
  virtual ~VSslServerSession();

protected:
  virtual bool doOpen();
  virtual bool doClose();

protected:
  bool setup(QString fileName);
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

public:
  VSslMethodType methodType;
  QString        certificatePath;
  QString        defaultKeyCrtFileName;
  bool           ignoreConnectMessage;

public:
  SSL_METHOD*     m_meth;
  SSL_CTX*        m_ctx;

protected:
  static int ssl_servername_cb(SSL *s, int *ad, void *arg);
  static int ssl_servername_cb_debug(SSL *s, int *ad, void *arg, int* debug);

protected:
  bool             setup(QString fileName);

protected:
  static EVP_PKEY* loadKey(VError& error, QString fileName);
  static X509*     loadCrt(VError& error, QString fileName);
  static bool      setKeyCrtStuff(VError& error, SSL_CTX* ctx, EVP_PKEY* key, X509* crt);
  static bool      setKeyCrtStuff(VError& error, SSL*     con, EVP_PKEY* key, X509* crt);

protected:
    VCS           certificateCs;

protected slots:
  void myRun(VTcpSession* tcpSession);

signals:
  void runned(VSslSession* sslSession);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public: // for VOptionable
  virtual void optionAddWidget(QLayout* layout);
  virtual void optionSaveDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_SSL_SERVER_H__
