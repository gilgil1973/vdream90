// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_HTTP_REQUEST_H__
#define __V_HTTP_REQUEST_H__

#include <VHttpHeader>

// ----------------------------------------------------------------------------
// VHttpRequestLine
// ----------------------------------------------------------------------------
class VHttpRequestLine
{
public:
  VHttpRequestLine();

public:
  QByteArray method;
  QByteArray path;
  QByteArray version;

public:
  void clear();
  bool parse(QByteArray data);
  QByteArray toByteArray();
};

// ----------------------------------------------------------------------------
// VHttpRequest
// ----------------------------------------------------------------------------
class VHttpRequest
{
public:
  VHttpRequest();

public:
  VHttpRequestLine requestLine;
  VHttpHeader      header;
  QByteArray      body;

public:
  void clear();
  bool parse(QByteArray data);
  QByteArray toByteArray();

public:
  bool findHost(QString& host, int& port);
};

#endif // __V_HTTP_REQUEST_H__
