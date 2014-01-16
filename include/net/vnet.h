// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_NET_H__
#define __V_NET_H__

#include <VRwObject>

// ----------------------------------------------------------------------------
// Link Library
// ----------------------------------------------------------------------------
#ifdef _MSC_VER
  #ifdef WINCE
    #pragma comment(lib, "ws2.lib")
  #else
    #pragma comment(lib, "ws2_32.lib")
  #endif // WINCE
#endif // _MSC_VER

#ifdef WIN32
  #define NOMINMAX
  #include <winSock2.h>
  #include <windows.h>
#endif // WIN32
#ifdef linux
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
#endif // linux

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
#ifndef SD_BOTH
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02
#endif // SD_BOTH

// ----------------------------------------------------------------------------
// Type Definition
// ----------------------------------------------------------------------------
#ifdef WIN32
typedef int socklen_t;
#endif // WIN32

// ----------------------------------------------------------------------------
// Ip
// ----------------------------------------------------------------------------
class Ip
{
protected:
  quint32 value;

public:
  Ip()                                 {                        } // default ctor
  Ip(const quint32 value)              { this->value = value;   } // conversion ctor
  operator quint32() const             { return value;          } // cast operator
  // const    quint32* operator &() const { return &value;         } // pointer operator // gilgil temp 2012.06.04

public:
  Ip(const QString s);

public:
  void clear();

public:
  QString str();
  QString qformat(QString format);

public:
  bool isLocalHost() { quint8 prefix = (value & 0xFF000000) >> 24; return prefix == 0x7F;                  } // 127.*.*.*
  bool isBroadcast() { return value == 0xFFFFFF;                                                           } // 255.255.255.255
  bool isMulticast() { quint8 prefix = (value & 0xFF000000) >> 24; return prefix >= 0xE0 && prefix < 0xF0; } // 224.0.0.0 ~ 239.255.255.255
};

// ----------------------------------------------------------------------------
// VNetInstance
// ----------------------------------------------------------------------------
class VNetInstance : private VNonCopyable
{
private: // singleton
  VNetInstance();
  virtual ~VNetInstance();

public:
  static VNetInstance& instance();
};

// ----------------------------------------------------------------------------
// VNet
// ----------------------------------------------------------------------------
class VNet : public VRwObject
{
public:
  VNet(void* owner = NULL);
  virtual ~VNet();

public:
  static bool isIPV4Address(const QString host);
  static Ip   resolve(QString host);
};

// ----------------------------------------------------------------------------
// VNetError, VSocketError
// ----------------------------------------------------------------------------
VDECLARE_ERROR_CLASS(VNetError)
VDECLARE_ERROR_CLASS(VSocketError)

// ----------------------------------------------------------------------------
// VNetError code
// ----------------------------------------------------------------------------
#define VERR_ERROR_IN_RECV        (VERR_CATEGORY_NET + 0)
#define VERR_PORT_IS_ZERO         (VERR_CATEGORY_NET + 1)
#define VERR_INVALID_HOST         (VERR_CATEGORY_NET + 2)
#define VERR_CAN_NOT_RESOLVE_HOST (VERR_CATEGORY_NET + 3)

#endif //__V_NET_H__
