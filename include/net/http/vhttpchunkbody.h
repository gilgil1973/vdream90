// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_HTTP_CHUNK_BODY_H__
#define __V_HTTP_CHUNK_BODY_H__

#include <VHttpCommon>

// ----------------------------------------------------------------------------
// VHttpChunkBody
// ----------------------------------------------------------------------------
class VHttpChunkBody
{
public:
  VHttpChunkBody();

public:
  typedef QPair<int /*chunkSize*/, QByteArray /*chunkData*/> Item;
  typedef QList<Item> Items;
  Items items;

public:
  void clear();
  int parse(QByteArray& buffer);
  QByteArray toByteArray();
};

#endif // __V_HTTP_CHUNK_BODY_H__
