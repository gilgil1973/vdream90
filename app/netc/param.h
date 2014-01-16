// ----------------------------------------------------------------------------
//
// vdream Component Suite version 8.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __PARAM_H__
#define __PARAM_H__

#include <VCommon>

#define DEFAULT_BUF_SIZE 8192
// ----------------------------------------------------------------------------
// NetType
// ----------------------------------------------------------------------------
typedef enum
{
  ntTCP,
  ntUDP,
  ntSSL
} NetType;

// ----------------------------------------------------------------------------
// Param
// ----------------------------------------------------------------------------
class Param
{
public:
  //
  // Network
  //
  NetType netType;
  QString host;
  int     port;
  QString localHost;
  int     localPort;

  // Other
  int     bufSize;
  QString log;

protected:
  int     argc;
  char**  argv;
  QString get(int idx);

public:
  Param();
  virtual ~Param();
  bool parse(int argc, char* argv[]);
};

#endif // __PARAM_H__
