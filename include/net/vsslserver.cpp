#include <VSslServer>
#include <VFile>
#include <VDebugNew>

// ----- gilgil temp 2014.02.25 -----
/*
// ----------------------------------------------------------------------------
// VSslSessionThread
// ----------------------------------------------------------------------------
VSslSessionThread::VSslSessionThread(VSslServer* owner, VSslSession* sslSession)
{
  LOG_DEBUG("%p", this); // gilgil temp 2014.02.25
  this->freeOnTerminate = true;
  this->sslServer       = owner;
  this->sslSession      = sslSession;
}

VSslSessionThread::~VSslSessionThread()
{
  LOG_DEBUG("%p", this); // gilgil temp 2014.02.25
  SAFE_DELETE(sslSession);
}

void VSslSessionThread::run()
{
  emit sslServer->runned(sslSession);

  if (freeOnTerminate)
  {
    VSslSessionThreadList& threadList = sslServer->threadList;
    threadList.lock();
    threadList.removeAt(threadList.indexOf(this));
    threadList.unlock();
  }
}
*/
// ----------------------------------

// ----------------------------------------------------------------------------
// VSslServer
// ----------------------------------------------------------------------------
VSslServer::VSslServer(void* owner) : VTcpServer(owner)
{
  VSslCommon::initialize();
  methodType = VSslMethodType::mtSSLV23;
  fileName   = "";
  s_cert     = NULL;
  s_key      = NULL;
  meth       = NULL;
  ctx        = NULL;

  QObject::connect(this, SIGNAL(runned(VTcpSession*)), this, SLOT(myRun(VTcpSession*)), Qt::DirectConnection);
}

VSslServer::~VSslServer()
{
  close();
}

bool VSslServer::doOpen()
{
  if (!VTcpServer::doOpen()) return false;

  //
  // Set server method
  //
  LOG_DEBUG("method=%s", qPrintable(methodType.str()));
  switch (methodType)
  {
    case VSslMethodType::mtSSLV2  : meth = (SSL_METHOD*)SSLv2_server_method();  break;
    case VSslMethodType::mtSSLV3  : meth = (SSL_METHOD*)SSLv3_server_method();  break;
    case VSslMethodType::mtSSLV23 : meth = (SSL_METHOD*)SSLv23_server_method(); break;
    case VSslMethodType::mtTLSV1  : meth = (SSL_METHOD*)TLSv1_server_method();  break;
    case VSslMethodType::mtDTLSV1 : meth = (SSL_METHOD*)DTLSv1_server_method(); break;
    case VSslMethodType::mtNone   :
    default       :
      SET_ERROR(VSslError, qformat("client method error(%s)", qPrintable(methodType.str())), VERR_SSL_METHOD);
      return false;
  }
  ctx = SSL_CTX_new(meth);

  //
  // Fie check
  //
  if (fileName == "")
  {
    SET_ERROR(VSslError, "file name must be specified", VERR_FILENAME_NOT_SPECIFIED);
    return false;
  }
  if (!QFile::exists(fileName))
  {
    SET_ERROR(VFileError, qformat("file(%s) not exist", qPrintable(fileName)), VERR_FILE_NOT_EXIST);
    return false;
  }

  //
  // Load Key
  //
  if (!loadKey()) return false;

  //
  // Load Cert
  //
  if (!loadCert()) return false;

  //
  // Set Cert Key Stuff
  //
  if (!setCertKeyStuff()) return false;

  return true;
}

bool VSslServer::doClose()
{
  sslSessionList.lock();
  for (int i = 0; i < sslSessionList.count(); i++)
  {
    VSslSession* session = sslSessionList.at(i);
    session->close();
  }
  sslSessionList.unlock();

  //
  // Free Cert
  //
  if (s_cert != NULL)
  {
    X509_free(s_cert);
    s_cert = NULL;
  }

  //
  // Free Key
  //
  if (s_key != NULL)
  {
    EVP_PKEY_free(s_key);
    s_key = NULL;
  }

  //
  // meth and ctx
  //
  if (meth != NULL)
  {
    meth = NULL;
  }
  if (ctx != NULL)
  {
    SSL_CTX_free(ctx);
    ctx = NULL;
  }

  VTcpServer::doClose();

  return true;
}

int VSslServer::doRead(char* buf, int size)
{
  Q_UNUSED(buf)
  Q_UNUSED(size)
  SET_ERROR(VError, "not readable", VERR_NOT_READABLE);
  return VERR_FAIL;
}

int VSslServer::doWrite(char* buf, int size)
{
  VLock lock(m_writeCS);

  sslSessionList.lock();
  for (int i = 0; i < sslSessionList.count(); i++)
  {
    VSslSession* session = sslSessionList.at(i);
    session->write(buf, size);
  }
  sslSessionList.unlock();
  return size;
}

bool VSslServer::loadKey()
{
  BIO* key = NULL;
  bool ok  = false;

  while (true)
  {
    key = BIO_new(BIO_s_file());
    if (key == NULL)
    {
      SET_ERROR(VSslError, "BIO_s_file return NULL", VERR_IN_BIO_S_FILE);
      break;
    }

    long res = BIO_read_filename(key, qPrintable(fileName));
    if (res <= 0)
    {
      SET_ERROR(VSslError, qformat("BIO_read_filename(%s) return %d", qPrintable(fileName), res), VERR_IN_BIO_READ_FILENAME);
      break;
    }

    s_key = PEM_read_bio_PrivateKey(key, NULL,
      NULL, // (pem_password_cb*)password_callback // gilgil temp 2008.10.29
      NULL // &cb_data);
    );
    if (s_key == NULL)
    {
      SET_ERROR(VSslError, "PEM_read_bio_PrivateKey return NULL", VERR_IN_PEM_READ_BIO_PRIVATEKEY);
      break;
    }
    ok = true;
    break;
  }

  if (key!= NULL) BIO_free(key);
  return ok;
}

bool VSslServer::loadCert()
{
  BIO* cert = NULL;
  bool ok   = false;

  while (true)
  {
    cert = BIO_new(BIO_s_file());
    if (cert == NULL)
    {
      SET_ERROR(VSslError, "BIO_s_file return NULL", VERR_IN_BIO_S_FILE);
      break;
    }

    long res = BIO_read_filename(cert, qPrintable(fileName));
    if (res <= 0)
    {
      SET_ERROR(VSslError, qformat("BIO_read_filename(%s) %d", qPrintable(fileName), res), VERR_IN_BIO_READ_FILENAME);
      break;
    }

    s_cert = PEM_read_bio_X509_AUX(cert, NULL, NULL, NULL);
    if (s_cert == NULL)
    {
      SET_ERROR(VSslError, "PEM_read_bio_X509_AUX return NULL", VERR_IN_PEM_READ_BIO_X509_AUX);
      break;
    }
    ok = true;
    break;
  }

  if (cert != NULL) BIO_free(cert);
  return ok;
}

bool VSslServer::setCertKeyStuff()
{
  int res;

  if (s_cert ==  NULL)
  {
    LOG_ERROR("s_cert is NULL");
    return false;
  }
  if (s_key == NULL)
  {
    LOG_ERROR("s_key is NULL");
    return false;
  }

  res = SSL_CTX_use_certificate(ctx, s_cert);
  if (res <= 0)
  {
    SET_ERROR(VSslError, qformat("SSL_CTX_use_certificate return %d", res), VERR_IN_SSL_CTX_USE_CERTIFICATE);
    return false;
  }

  res = SSL_CTX_use_PrivateKey(ctx, s_key);
  if (res <= 0)
  {
    SET_ERROR(VSslError, qformat("SSL_CTX_use_PrivateKey return %d", res), VERR_SSL_CTX_USER_PRIVATEKEY);
    return false;
  }

  res = SSL_CTX_check_private_key(ctx);
  if (!res)
  {
    SET_ERROR(VSslError, qformat("SSL_CTX_check_private_key return %d", res), VERR_SSL_CTX_CHECK_PRIVATEKEY);
    return false;
  }

  return true;
}

void VSslServer::myRun(VTcpSession* tcpSession)
{
  VSslSession *session = new VSslSession;
  session->owner = this;
  session->tcpSession = tcpSession;
  session->sock = tcpSession->handle;
  session->ctx  = ctx;
  if (!session->open()) goto _end;

  SSL_set_accept_state(session->con);
  while (true)
  {
    if (SSL_is_init_finished(session->con)) break;
    int res = SSL_accept(session->con);
    if (res < 0)
    {
      LOG_DEBUG("[VDSSLServer.cpp] VDSSLSessionList::add SSL_accept return %d error=%d", res, SSL_get_error(session->con, res));
      goto _end;
    }
    else if (res == 0) // may be the TLS/SSL handshake was not successful
    {
      msleep(1);
      continue;
    } else // res > 0
    {
      break;
    }
  }

  sslSessionList.lock();
  sslSessionList.push_back(session);
  sslSessionList.unlock();

  emit runned(session);

  sslSessionList.lock();
  sslSessionList.removeOne(session);
  sslSessionList.unlock();

_end:
  delete session;
}

void VSslServer::load(VXml xml)
{
  VTcpServer::load(xml);

  methodType = xml.getStr("methodType", methodType.str());
  fileName   = xml.getStr("fileName", fileName);
}

void VSslServer::save(VXml xml)
{
  VTcpServer::save(xml);

  xml.setStr("methodType", methodType.str());
  xml.setStr("fileName", fileName);
}

#ifdef QT_GUI_LIB
void VSslServer::addOptionWidget(QLayout* layout)
{
  VTcpServer::addOptionWidget(layout);

  // gilgil temp 2014.02.25
}

void VSslServer::saveOptionDlg(QDialog* dialog)
{
  VTcpServer::saveOptionDlg(dialog);

  // gilgil temp 2014.02.25
}
#endif // QT_GUI_LIB
