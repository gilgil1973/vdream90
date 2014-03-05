// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_GRAPH_H__
#define __V_GRAPH_H__

#include <VObject>
#include <VFactory>

// ----------------------------------------------------------------------------
// VGraphObjectList
// ----------------------------------------------------------------------------
class VGraph;
class VGraphObjectList : public QList<VObject*>, public VXmlable
{
protected:
  VGraph* m_graph;

public:
  VGraphObjectList(VGraph* graph);
  virtual ~VGraphObjectList();

public:
  void clear();

public:
  bool addObject(VObject* object);
  bool delObject(VObject* object);

public:
  VObject* findByName(QString name);
  VObject* findByClassName(QString className);

  QList<VObject*> findObjectsByClassName(QString className);
  QList<VObject*> findObjectsByCategoryName(QString categoryName);

  QStringList findNamesByClassName(QString className);
  QStringList findNamesByCategoryName(QString categoryName);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

// ----------------------------------------------------------------------------
// VGraphConnect
// ----------------------------------------------------------------------------
class VGraphConnect : public VXmlable
{
public:
  VGraphConnect();
  virtual ~VGraphConnect();

public:
  QString sender;
  QString signal;
  QString receiver;
  QString slot;

public:
  bool operator == (const VGraphConnect& rhs);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

// ----------------------------------------------------------------------------
// VGraphConnectList
// ----------------------------------------------------------------------------
class VGraphConnectList : public QList<VGraphConnect>, public VXmlable
{
protected:
  VGraph* m_graph;

public:
  VGraphConnectList(VGraph* graph);
  virtual ~VGraphConnectList();

public:
  void clear();

public:
  bool addConnect(VGraphConnect connect);
  bool delConnect(VGraphConnect connect);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

// ----------------------------------------------------------------------------
// VGraph
// ----------------------------------------------------------------------------
class VGraph : public VObject
{
  Q_OBJECT

public:
  VGraph(void* owner = NULL);
  virtual ~VGraph();

public:
  void clear();

public:
  virtual bool doOpen();
  virtual bool doClose();

public:
  VGraphObjectList  objectList;
  VGraphConnectList connectList;

protected:
  static QStringList methodList(VObject* object, QMetaMethod::MethodType methodType);
public:
  static QStringList signalList(VObject* object);
  static QStringList slotList(VObject* object);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

#endif // __V_GRAPH_H__
