// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_SSL_COMMON_H__
#define __V_SSL_COMMON_H__

#include <VError>
#include <VLog>
#include <openssl/ssl.h>

// ----------------------------------------------------------------------------
// Link Library
// ----------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma comment(lib, "libeay32.lib")
  #pragma comment(lib, "ssleay32.lib")
#endif // _MSC_VER

// ----------------------------------------------------------------------------
// VSslCommon
// ----------------------------------------------------------------------------
class VSslCommon
{
public:
  static const int DEFAULT_SSL_PORT        = 443;
  static const int DEFAULT_SSL_PROXY_PORT  = 4433;

public:
  static bool initialize();
};

// ----------------------------------------------------------------------------
// VSslMethodType
// ----------------------------------------------------------------------------
class VSslMethodType
{
public:
  enum _VSslMethodType
  {
    mtNone,   // 0
    mtSSLV2,  // 1
    mtSSLV3,  // 2
    mtSSLV23, // 3
    mtTLSV1,  // 4
    mtDTLSV1  // 5
  };

protected:
  _VSslMethodType value;

public:
  VSslMethodType()                            {                      } // default ctor
  VSslMethodType(const _VSslMethodType value) { this->value = value; } // conversion ctor
  operator _VSslMethodType() const            { return value;        } // cast operator

public:
  VSslMethodType(const int i)                 { value = (_VSslMethodType)i; }
  VSslMethodType(const QString s);
  QString str() const;
};

// ----------------------------------------------------------------------------
// VSslError
// ----------------------------------------------------------------------------
VDECLARE_ERROR_CLASS(VSslError)

// ----------------------------------------------------------------------------
// VDSSLError code
// ----------------------------------------------------------------------------
const static int VERR_SSL_METHOD                 = VERR_CATEGORY_SSL + 0;
const static int VERR_IN_BIO_S_FILE              = VERR_CATEGORY_SSL + 1;
const static int VERR_IN_BIO_READ_FILENAME       = VERR_CATEGORY_SSL + 2;
const static int VERR_IN_PEM_READ_BIO_PRIVATEKEY = VERR_CATEGORY_SSL + 3;
const static int VERR_IN_PEM_READ_BIO_X509_AUX   = VERR_CATEGORY_SSL + 4;
const static int VERR_IN_SSL_CTX_USE_CERTIFICATE = VERR_CATEGORY_SSL + 5;
const static int VERR_SSL_CTX_USER_PRIVATEKEY    = VERR_CATEGORY_SSL + 6;
const static int VERR_SSL_CTX_CHECK_PRIVATEKEY   = VERR_CATEGORY_SSL + 7;

#endif // __V_SSL_COMMON_H__
