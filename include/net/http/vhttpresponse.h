// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_HTTP_RESPONSE_H__
#define __V_HTTP_RESPONSE_H__

#include <VHttpHeader>

// ----------------------------------------------------------------------------
// VHttpStatusLine
// ----------------------------------------------------------------------------
class VHttpStatusLine
{
public:
  VHttpStatusLine();

public:
  QByteArray version;
  int        code;
  QByteArray text;

public:
  void clear();
  bool parse(QByteArray& data);
  QByteArray toByteArray();
};

// ----------------------------------------------------------------------------
// VHttpResponse
// ----------------------------------------------------------------------------
class VHttpResponse
{
public:
  VHttpResponse();

public:
  VHttpStatusLine statusLine;
  VHttpHeader     header;

public:
  void clear();
  bool parse(QByteArray data, QByteArray* body);
  QByteArray toByteArray();
};

#endif // __V_HTTP_RESPONSE_H__
