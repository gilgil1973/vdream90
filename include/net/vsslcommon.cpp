#include <VSslCommon>
#include <VThread>

// ----------------------------------------------------------------------------
// VSslCommon
// ----------------------------------------------------------------------------
int VSslCommon::numLock = 0;
VCS *VSslCommon::lockCs = NULL;

VSslCommon::VSslCommon()
{
  numLock = 0;
  lockCs  = NULL;

  int res = SSL_library_init();
  if (res != 1)
  {
   LOG_FATAL("SSL_library_init return %d", res);
   return;
  }

  numLock = CRYPTO_num_locks();
  LOG_DEBUG("CRYPTO_num_locks()=%d", numLock);
  lockCs = new VCS[numLock];

  CRYPTO_set_id_callback(threadIdCallback);
  CRYPTO_set_locking_callback(lockingCallback);
}

VSslCommon::~VSslCommon()
{
  if (numLock != 0)
  {
    numLock = 0;
  }
  if (lockCs != NULL)
  {
    delete[] lockCs;
    lockCs = NULL;
  }
}

unsigned long VSslCommon::threadIdCallback(void)
{
  unsigned long res;
  res = (unsigned long)QThread::currentThreadId();
  return res;
}

void VSslCommon::lockingCallback(int mode, int type, const char* file, int line)
{
  Q_UNUSED(file)
  Q_UNUSED(line)

  // LOG_DEBUG("type=%d", type); // gilgil temp 2014.03.24

  LOG_ASSERT(type < numLock);

  if (mode & CRYPTO_LOCK)
  {
    lockCs[type].lock();
  }
  else
  {
    lockCs[type].unlock();
  }
}

VSslCommon& VSslCommon::instance()
{
  static VSslCommon _instance;
  return _instance;
}

// ----------------------------------------------------------------------------
// VSslMethodType
// ----------------------------------------------------------------------------
VSslMethodType::VSslMethodType(const QString s)
{
  if (s == "mtNone")         value = mtNone;
  else if (s == "mtSSLv2")   value = mtSSLv2;
  else if (s == "mtSSLv3")   value = mtSSLv3;
  else if (s == "mtSSLv23")  value = mtSSLv23;
  else if (s == "mtTLSv1")   value = mtTLSv1;
  else if (s == "mtTLSv1_1") value = mtTLSv1_1;
  else if (s == "mtTLSv1_2") value = mtTLSv1_2;
  else if (s == "mtDTLSv1")  value = mtDTLSv1;
  else value = mtNone;
}

QString VSslMethodType::str() const
{
  QString res;
  switch (value)
  {
    case mtNone    : res = "mtNone";    break;
    case mtSSLv2   : res = "mtSSLv2";   break;
    case mtSSLv3   : res = "mtSSLv3";   break;
    case mtSSLv23  : res = "mtSSLv23";  break;
    case mtTLSv1   : res = "mtTLSv1";   break;
    case mtTLSv1_1 : res = "mtTLSv1_1"; break;
    case mtTLSv1_2 : res = "mtTLSv1_2"; break;
    case mtDTLSv1  : res = "mtDTLSV1";  break;
    default        : res = "mtNone";    break;
  }
  return res;
}
