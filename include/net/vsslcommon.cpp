#include <VSslCommon>
#include <VThread>

// ----------------------------------------------------------------------------
// VSslCommon
// ----------------------------------------------------------------------------
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

  VSslCommon& sc = VSslCommon::instance();
  LOG_ASSERT(type < sc.numLock);

  if (mode & CRYPTO_LOCK)
  {
    sc.lockCs[type].lock();
  }
  else
  {
    sc.lockCs[type].unlock();
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
  if (s == "mtNone")   value = mtNone;
  if (s == "mtSSLV2")  value = mtSSLV2;
  if (s == "mtSSLV3")  value = mtSSLV3;
  if (s == "mtSSLV23") value = mtSSLV23;
  if (s == "mtTLSV1")  value = mtTLSV1;
  if (s == "mtDTLSV1") value = mtDTLSV1;
  else value = mtNone;
}

QString VSslMethodType::str() const
{
  QString res;
  switch (value)
  {
    case mtNone   : res = "mtNone";   break;
    case mtSSLV2  : res = "mtSSLV2";  break;
    case mtSSLV3  : res = "mtSSLV3";  break;
    case mtSSLV23 : res = "mtSSLV23"; break;
    case mtTLSV1  : res = "mtTLSV1";  break;
    case mtDTLSV1 : res = "mtDTLSV1"; break;
    default       : res = "mtNone";   break;
  }
  return res;
}
