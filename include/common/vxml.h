// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//------------------------------------------------------------------------------

#ifndef __V_XML_H__
#define __V_XML_H__

#define WIN32_LEAN_AND_MEAN
#include <QtXml>
#include <VString>

// ----------------------------------------------------------------------------
// Link Library
// ----------------------------------------------------------------------------
#ifdef _MSC_VER
  #ifdef _DEBUG
    // #pragma comment(lib, "QtXmld4.lib") // gilgil temp 2013.02.12
  #else
    // #pragma comment(lib, "QtXml4.lib") // gilgil temp 2013.02.12
  #endif // _DEBUG
#endif // _MSC_VER


// ----------------------------------------------------------------------------
// VXml
// ----------------------------------------------------------------------------
class VXmlList;
class VXml : protected QDomElement
{
  friend class VXmlDoc;

public:
  VXml()                                   {}
  VXml(const QDomElement& rhs) : QDomElement(rhs) {}
  // VXml& operator = (const VXml& rhs)       { *((QDomElement*)this) = *((QDomElement*)&rhs);  return *this; } // gilgil temp 2012.05.31
  operator QDomElement() const { return *this; }

public:
  //
  // Edit Child
  //
  void clearChild();
  VXml addChild(QString tagName);
  VXml removeChild(QString tagName);
  VXml gotoChild(QString tagName);
  VXml gotoChilds(QString tagNames, QString delimiter = "/");

  //
  // Retrieve Child
  //
  VXmlList childs();
  VXml findChild (QString tagName);
  VXml findChilds(QString tagNames, QString delimiter = "/");

  //
  // From QDomNode
  //
  void    clear()                           { QDomElement::clear();          }
  bool    isNull() const                    { return QDomElement::isNull();  }
  QString tagName() const                   { return QDomElement::tagName(); }
  void    setTagName(const QString& name)   { QDomElement::setTagName(name); }
  QString text() const                      { return QDomElement::text();    }
  VXml    cloneNode(bool deep = true) const { QDomNode node = QDomNode::cloneNode(deep); return *(VXml*)&node; }

  //
  // Attributes
  //
  int        getInt    (const QString name,  int           def = 0);
  void       setInt    (const QString name,  int           val);
  int        getLong   (const QString name,  long          def = 0);
  void       setLong   (const QString name,  long          val);
  int        getULong  (const QString name,  unsigned long def = 0);
  void       setULong  (const QString name,  unsigned long val);
  int        getInt64  (const QString name,  qint64        def = 0);
  void       setInt64  (const QString name,  qint64        val);
  QChar      getChr    (const QString name,  QChar         def = '\0');
  void       setChr    (const QString name,  QChar         val);
  bool       getBool   (const QString name,  bool          def = false);
  void       setBool   (const QString name,  bool          val);
  double     getDouble (const QString name,  double        def = 0.);
  void       setDouble (const QString name,  double        val);
  QString    getStr    (const QString name,  QString       def = "");
  void       setStr    (const QString name,  QString       val);
  QByteArray getArr    (const QString name,  QByteArray    def = "");
  void       setArr    (const QString name,  QByteArray    val);

  void    removeAttr(const QString name);
};

// ----------------------------------------------------------------------------
// IVXmlable
// ----------------------------------------------------------------------------
class IVXmlable
{
public:
  virtual void load(VXml xml) = 0;
  virtual void save(VXml xml) = 0;
};

// ----------------------------------------------------------------------------
// VXmlable
// ----------------------------------------------------------------------------
class VXmlable : public IVXmlable
{
public:
  bool loadFromFile      (const QString& fileName, const QString& path);
  bool saveToFile        (const QString& fileName, const QString& path);
  bool loadFromDefaultDoc(const QString& path);
  bool saveToDefaultDoc  (const QString& path);
};

// ----------------------------------------------------------------------------
// VXmlList
// ----------------------------------------------------------------------------
class VXmlList : public QDomNodeList
{
public:
  VXmlList()                                        {}
  VXmlList(const QDomNodeList& rhs) : QDomNodeList(rhs) {}
  // VXmlList& operator = (const VXmlList& rhs)        { *((QDomNodeList*)this) = *((QDomNodeList*)&rhs);  return *this; } // gilgil temp 2012.05.31

public:
  VXml at(int index);
};

#define xml_foreach(xml, list)                                              \
  int __i = 0;                                                              \
  int __count = (list).count();                                             \
  if (__count > 0)                                                          \
    for (VXml xml = (list).at(0); __i < __count; __i++, xml = (list).at(__i))

#endif // __V_EMT_H__
