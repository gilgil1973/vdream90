#include <VSslServer>
#include <VFile>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VSslServerSession
// ----------------------------------------------------------------------------
VSslServerSession::VSslServerSession(void* owner) : VSslSession(owner)
{
  s_cert         = NULL;
  s_key          = NULL;
}

VSslServerSession::~VSslServerSession()
{
  close();
}

bool VSslServerSession::doOpen()
{
  LOG_DEBUG("bef SSL_CTX_set_tlsext_servername_callback");
  bool callbackOk = true;
  if (!SSL_CTX_set_tlsext_servername_callback(ctx, ssl_servername_cb))
  {
    LOG_ERROR("SSL_CTX_set_tlsext_servername_callback return false");
    callbackOk = false;
  }
  if (!SSL_CTX_set_tlsext_servername_arg(ctx, this))
  {
    LOG_ERROR("SSL_CTX_set_tlsext_servername_arg return false");
    callbackOk = false;
  }

  if (!callbackOk)
  {
    VSslServer* server = (VSslServer*)owner;

    //
    // Fie check
    //
    QString fileName = server->certificatePath + server->defaultKeyCrtFileName;
    if (QFile::exists(fileName))
    {
      // if (!setup(fileName)) return false; // gilgil temp 2014.02.26
    } else
    {
      LOG_WARN("file(%s) not exist", qPrintable(fileName));
    }
  }

  return VSslSession::doOpen();
}

bool VSslServerSession::doClose()
{
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

  return VSslSession::doClose();
}

bool VSslServerSession::setup(QString fileName)
{
  LOG_DEBUG("------------------------------------------");
  LOG_DEBUG("fileName=%s", qPrintable(fileName));
  LOG_DEBUG("------------------------------------------");
  if (!loadKey(fileName)) return false;
  if (!loadCrt(fileName)) return false;
  if (!setCrtKeyStuff()) return false;
  return true;
}

bool VSslServerSession::loadKey(QString fileName)
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

bool VSslServerSession::loadCrt(QString fileName)
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

bool VSslServerSession::setCrtKeyStuff()
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

  // res = SSL_CTX_use_certificate(ctx, s_cert); // gilgil temp 2014.02.27
  res = SSL_use_certificate(con, s_cert); // gilgil temp 2014.02.27
  if (res <= 0)
  {
    SET_ERROR(VSslError, qformat("SSL_CTX_use_certificate return %d", res), VERR_IN_SSL_CTX_USE_CERTIFICATE);
    return false;
  }

  // res = SSL_CTX_use_PrivateKey(ctx, s_key); // gilgil temp 2014.02.27
  res = SSL_use_PrivateKey(con, s_key); // gilgil temp 2014.02.27
  if (res <= 0)
  {
    SET_ERROR(VSslError, qformat("SSL_CTX_use_PrivateKey return %d", res), VERR_SSL_CTX_USER_PRIVATEKEY);
    return false;
  }

  // res = SSL_CTX_check_private_key(ctx); // gilgil temp 2014.02.27
  res = SSL_check_private_key(con); // gilgil temp 2014.02.27
  if (!res)
  {
    SET_ERROR(VSslError, qformat("SSL_CTX_check_private_key return %d", res), VERR_SSL_CTX_CHECK_PRIVATEKEY);
    return false;
  }

  //LOG_DEBUG("bef SSL_set_SSL_CTX ctx=%p", ctx);
  //ctx = SSL_set_SSL_CTX(con, ctx);
  //LOG_DEBUG("aft SSL_set_SSL_CTX ctx=%p", ctx);

  return true;
}

int VSslServerSession::ssl_servername_cb(SSL *s, int *ad, void *arg)
{
  Q_UNUSED(ad)
  VSslServerSession* session = (VSslServerSession*)arg;
  VSslServer*        server  = (VSslServer*)(session->owner);

  const char* serverName = SSL_get_servername(s, TLSEXT_NAMETYPE_host_name);

  QString fileName;
  if (serverName != NULL)
  {
    LOG_DEBUG("serverName=%s", serverName);
    fileName = server->certificatePath + serverName + ".pem";
  } else
  {
    LOG_WARN("serverName is null");
    fileName = server->certificatePath + server->defaultKeyCrtFileName;
  }

  VLock lock(server->certificateCs); // protect file create critical section
  if (!QFile::exists(fileName))
  {
    QProcess process;

    QString path = server->certificatePath;
    LOG_DEBUG("path=%s", qPrintable(path));
    process.setWorkingDirectory(path);

    QString command = qformat("%s_ssl_make_site.bat %s 2>&1", qPrintable(path), qPrintable(serverName));
    LOG_DEBUG("command=%s", qPrintable(command));

    process.start(command);
    LOG_DEBUG("pid=%p", process.pid());

    if(!process.waitForStarted())
    {
      LOG_FATAL("process.waitForStarted(%s) return false", qPrintable(command));
    }
    while(process.waitForReadyRead())
    {
        LOG_DEBUG("%s", process.readAll().data());
    }
    // ----- gilgil temp 23014.02.26 -----
    /*
    if (!process.waitForFinished())
    {
      LOG_FATAL("process.waitForFinished(%s) return false", qPrintable(command));
    }
    */
    // sleep(3); // gilgil temp 2015.02.27
    // -----------------------------------
  }

  session->setup(fileName);

  return SSL_TLSEXT_ERR_NOACK;
}

// ----------------------------------------------------------------------------
// VSslServer
// ----------------------------------------------------------------------------
VSslServer::VSslServer(void* owner) : VTcpServer(owner)
{
  VSslCommon::initialize();
  methodType            = VSslMethodType::mtTLSV1;
  certificatePath       = "./certificate/"; // gilgil temp 2014.02.27
  defaultKeyCrtFileName = "default.pem";
  m_meth                = NULL;
  m_ctx                 = NULL;

  bool res = QObject::connect(this, SIGNAL(runned(VTcpSession*)), this, SLOT(myRun(VTcpSession*)), Qt::DirectConnection);
  if (!res)
  {
    LOG_FATAL("QObject::connect return false");
  }
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
    case VSslMethodType::mtSSLV2  : m_meth = (SSL_METHOD*)SSLv2_server_method();  break;
    case VSslMethodType::mtSSLV3  : m_meth = (SSL_METHOD*)SSLv3_server_method();  break;
    case VSslMethodType::mtSSLV23 : m_meth = (SSL_METHOD*)SSLv23_server_method(); break;
    case VSslMethodType::mtTLSV1  : m_meth = (SSL_METHOD*)TLSv1_server_method();  break;
    case VSslMethodType::mtDTLSV1 : m_meth = (SSL_METHOD*)DTLSv1_server_method(); break;
    case VSslMethodType::mtNone   :
    default       :
      SET_ERROR(VSslError, qformat("client method error(%s)", qPrintable(methodType.str())), VERR_SSL_METHOD);
      return false;
  }
  m_ctx = SSL_CTX_new(m_meth);

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
  // m_meth and m_ctx
  //
  if (m_meth != NULL)
  {
    m_meth = NULL;
  }
  if (m_ctx != NULL)
  {
    SSL_CTX_free(m_ctx);
    m_ctx = NULL;
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

void VSslServer::myRun(VTcpSession* tcpSession)
{
  VSslServerSession *session = new VSslServerSession;
  session->owner       = this;
  session->tcpSession  = tcpSession;
  session->handle      = tcpSession->handle;
  session->ctx         = m_ctx;
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

  methodType            = xml.getStr("methodType", methodType.str());
  certificatePath       = xml.getStr("certificatePath", certificatePath);
  defaultKeyCrtFileName = xml.getStr("defaultKeyCrtFileName", defaultKeyCrtFileName);
}

void VSslServer::save(VXml xml)
{
  VTcpServer::save(xml);

  xml.setStr("methodType", methodType.str());
  xml.setStr("certificatePath", certificatePath);
  xml.setStr("defaultKeyCrtFileName", defaultKeyCrtFileName);
}

#ifdef QT_GUI_LIB
void VSslServer::addOptionWidget(QLayout* layout)
{
  VTcpServer::addOptionWidget(layout);

  QStringList methodTypes; methodTypes << "mtNone" << "mtSSLV2" << "mtSSLV3" << "mtSSLV23" << "mtTLSV1" << "mtDTLSV1";
  VOptionable::addComboBox(layout, "cbxMethodType", "Method Type", methodTypes, (int)methodType, methodType.str());
  VOptionable::addLineEdit(layout, "leCertificatePath", "Certificate Path", certificatePath);
  VOptionable::addLineEdit(layout, "leDefaultKeyCrtFileName", "Default KeyCrtFileName", defaultKeyCrtFileName);
}

void VSslServer::saveOptionDlg(QDialog* dialog)
{
  VTcpServer::saveOptionDlg(dialog);

  methodType            = (VSslMethodType)(dialog->findChild<QComboBox*>("cbxMethodType")->currentIndex());
  certificatePath       = dialog->findChild<QLineEdit*>("leCertificatePath")->text();
  defaultKeyCrtFileName = dialog->findChild<QLineEdit*>("leDefaultKeyCrtFileName")->text();
}
#endif // QT_GUI_LIB
