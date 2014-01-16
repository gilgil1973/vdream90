// ----------------------------------------------------------------------------
//
// VDream Component Suite version 5.0
//
// http://www.vdream.co.kr
//
// Copyright (c) NetworkLab Corporation All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef paramh
#define paramh

#include <VCommon>
// #include "VDDelphi.h" // gilgil temp 2012.04.15

#define DEFAULT_BUF_SIZE 8192
// ----------------------------------------------------------------------------
// NetType
// ----------------------------------------------------------------------------
typedef enum
{
  ntTCP,
  ntUDP
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
  int     port;
  QString localHost;
  int     localPort;

  // Other
  int     bufSize;
  bool    echo;
  bool    echoBroadcast;
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

#endif paramh
