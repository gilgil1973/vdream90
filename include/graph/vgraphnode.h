// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_GRAPH_NODE_H__
#define __V_GRAPH_NODE_H__

#include <VObject>
#include <VGraphStream>

// ----------------------------------------------------------------------------
// VGraphNode
// ----------------------------------------------------------------------------
class VGraphLink;
class VGraphNode : public VObject
{
  Q_OBJECT

public:
  VGraphNode(void* owner);
  virtual ~VGraphNode();

public:
  enum Mode { None, SendOnly, RecvOnly, SendRecv };
public:
  virtual Mode mode() { return None; }

public:
  QList<VGraphNode*> outNodes;
  QList<VGraphNode*> inNodes;

  virtual bool addOutLink(VGraphNode* inNode,  VGraphStreamType* type);
  virtual bool addInLink (VGraphNode* outNode, VGraphStreamType* type);

public:
  virtual VGraphStreamTypeList outStreamTypeList();
  virtual bool acceptInStreamType(VGraphStreamType* type);

protected:
  virtual bool doOpen();
  virtual bool doClose();

protected:
  virtual bool send(VGraphStream* stream);
  virtual bool recv(VGraphStream* stream);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

#endif // __V_GRAPH_NODE_H__
