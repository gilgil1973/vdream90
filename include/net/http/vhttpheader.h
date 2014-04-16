// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_HTTP_HEADER_H__
#define __V_HTTP_HEADER_H__

#include <VHttpCommon>

// ----------------------------------------------------------------------------
// VHttpHeader
// ----------------------------------------------------------------------------
class VHttpHeader
{
public:
  VHttpHeader();

public:
  typedef QPair<QByteArray /*key*/, QByteArray /*value*/> Item;
  typedef QList<Item> Items;
  Items items;

public:
  QByteArray value(QByteArray key);
  void       setValue(QByteArray key, QByteArray value);
  void       addValue(QByteArray key, QByteArray value);

public:
  void clear();
  bool parse(QByteArray& buffer);
  QByteArray toByteArray();
};

#endif // __V_HTTP_HEADER_H__
