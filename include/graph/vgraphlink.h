// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_GRAPH_LINK_H__
#define __V_GRAPH_LINK_H__

#include <VObject>

// ----------------------------------------------------------------------------
// VGraphLink
// ----------------------------------------------------------------------------
class VGraphNode;
class VGraphLink : public VObject
{
  Q_OBJECT

public:
  VGraphLink(void* owner);
  virtual ~VGraphLink();

protected:
  virtual bool doOpen();
  virtual bool doClose();

public:
  virtual bool link(VGraphNode* sender, VGraphNode* receiver);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

#endif // __V_GRAPH_NODE_H__
