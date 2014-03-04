// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_OBJECT_H__
#define __V_OBJECT_H__

#include <QObject>

#include <VError>
#include <VString>
#include <VXml>
#include <VLog>

// ----------------------------------------------------------------------------
// VObject
// ----------------------------------------------------------------------------
class VObject :
  public QObject,
  // public VGetLoggable, // gilgil temp 2012.09.18
  public VXmlable
{
  Q_OBJECT

public:
  //
  // constructor and destructor
  //
public:
  void* owner;

public:
  VObject(void* owner = NULL);
  virtual ~VObject();

protected:
  VState m_state;

public:
  int     tag; // used for debugging
  QString name;
  VState  state()     { return m_state;                   }
  QString className() { return CLASS_NAME(*this);         }
  bool    active()    { return m_state == VState::Opened; }

  //
  // error
  //
public:
  VError error;

  //
  // open and close
  //
public slots:
  virtual bool open();
  virtual bool close();
  virtual bool close(bool wait, VTimeout timeout = vd::DEFAULT_TIMEOUT);
  virtual bool wait(VTimeout timeout = vd::DEFAULT_TIMEOUT);

protected:
  virtual bool doOpen();
  virtual bool doClose();

signals:
  void opened();
  void closed();

  //
  // load and save
  //
public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

#endif // __V_OBJECT_H__
