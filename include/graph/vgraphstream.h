// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_GRAPH_STREAM_H__
#define __V_GRAPH_STREAM_H__

#include <typeinfo>
#include <QList>

// ----------------------------------------------------------------------------
// VGraphStream
// ----------------------------------------------------------------------------
class VGraphStream
{
public:
  VGraphStream()          {}
  virtual ~VGraphStream() {}
};

// ----------------------------------------------------------------------------
// VGraphStreamType
// ----------------------------------------------------------------------------
class VGraphStreamType : public std::type_info
{
};

// ----------------------------------------------------------------------------
// VGraphStreamType
// ----------------------------------------------------------------------------
class VGraphStreamTypeList : public QList<std::type_info*>
{
};

#endif // __V_GRAPH_STREAM_H__
