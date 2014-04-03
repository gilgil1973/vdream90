#include <VSslClient>
#include <VDebugNew>

REGISTER_METACLASS(VSslClient, VNet)

// ----------------------------------------------------------------------------
// VSslClient
// ----------------------------------------------------------------------------
VSslClient::VSslClient(void* owner) : VTcpClient(owner)
{
  VSslCommon::instance();

  sslSession = new VSslSession(this);
  sslSession->tcpSession = tcpSession;
  methodType = VSslMethodType::mtTLSV1;
  m_meth     = NULL;
  m_ctx      = NULL;
}

VSslClient::~VSslClient()
{
  close();
  SAFE_DELETE(sslSession);
}

bool VSslClient::close()
{
  //
  // Hang might be occurred when calling open in one thread and calling close in other thread.
  // So call tcpSession->doClose forcibly preventing hang.
  //
  sslSession->doClose();
  return VTcpClient::close();
}

bool VSslClient::doOpen()
{
  VLock lock(stateOpenCs); // gilgil temp 2014.03.14

  if (!VTcpClient::doOpen()) return false;

  //
  // set client method
  //
  LOG_DEBUG("method=%s", qPrintable(methodType.str()));
  switch (methodType)
  {
    case VSslMethodType::mtSSLV2  : m_meth = (SSL_METHOD*)SSLv2_client_method();  break;
    case VSslMethodType::mtSSLV3  : m_meth = (SSL_METHOD*)SSLv3_client_method();  break;
    case VSslMethodType::mtSSLV23 : m_meth = (SSL_METHOD*)SSLv23_client_method(); break;
    case VSslMethodType::mtTLSV1  : m_meth = (SSL_METHOD*)TLSv1_client_method();  break;
    case VSslMethodType::mtDTLSV1 : m_meth = (SSL_METHOD*)DTLSv1_client_method(); break;
    case VSslMethodType::mtNone   :
    default       :
      SET_ERROR(VSslError, qformat("client method error(%s)", qPrintable(methodType.str())), VERR_SSL_METHOD);
      return false;
  }
  m_ctx = SSL_CTX_new(m_meth);

  //
  // set sock and con
  //
  // sslSession->handle = tcpSession->handle; // gilgil temp 2014.04.03
  sslSession->ctx    = m_ctx;
  if (!sslSession->open())
  {
    error = sslSession->error;
    doClose();
    return false;
  }
  SSL_set_connect_state(sslSession->con);
  int res = SSL_connect(sslSession->con);
  if (res <= 0)
  {
    SET_ERROR(VSslError, qformat("SSL_connect return %d", res), SSL_get_error(sslSession->con, res));
    return false;
  }

  return true;
}

bool VSslClient::doClose()
{
  VLock lock(stateCloseCs); // gilgil temp 2014.03.14

  sslSession->close();

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

  VTcpClient::doClose();
  return true;
}

int VSslClient::doRead(char* buf, int size)
{
  // VLock lock(stateReadCs); // gilgil temp 2014.03.14

  return sslSession->read(buf, size);
}

int VSslClient::doWrite(char* buf, int size)
{
  // VLock lock(stateWriteCs); // gilgil temp 2014.03.14

  return sslSession->write(buf, size);
}

void VSslClient::load(VXml xml)
{
  VTcpClient::load(xml);

  methodType = xml.getInt("methodType", (int)methodType);
}

void VSslClient::save(VXml xml)
{
  VTcpClient::save(xml);

  xml.setInt("methodType", (int)methodType);
}

#ifdef QT_GUI_LIB
void VSslClient::optionAddWidget(QLayout* layout)
{
  VTcpClient::optionAddWidget(layout);

  QStringList methodTypes; methodTypes << "mtNone" << "mtSSLV2" << "mtSSLV3" << "mtSSLV23" << "mtTLSV1" << "mtDTLSV1";
  VOptionable::addComboBox(layout, "cbxMethodType", "Method Type", methodTypes, (int)methodType, methodType.str());
}

void VSslClient::optionSaveDlg(QDialog* dialog)
{
  VTcpClient::optionSaveDlg(dialog);

  methodType = (VSslMethodType)(dialog->findChild<QComboBox*>("cbxMethodType")->currentIndex());
}
#endif // QT_GUI_LIB
