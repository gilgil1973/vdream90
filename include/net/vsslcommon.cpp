#include <VSslCommon>

// ----------------------------------------------------------------------------
// VSslCommon
// ----------------------------------------------------------------------------
bool VSslCommon::initialize()
{
  static bool initialized = false;
  if (initialized) return true;
  initialized = true;
  int res = SSL_library_init();
  if (res != 1)
  {
   LOG_FATAL("SSL_library_init return %d", res);
  }
  return res == 1;
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
