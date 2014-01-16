// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_XML_DOC_H__
#define __V_XML_DOC_H__

#include <VXml>

// ----------------------------------------------------------------------------
// VXmlDoc
// ----------------------------------------------------------------------------
class VXmlDoc : protected QDomDocument
{
public:
  VXmlDoc()                                            {}
  VXmlDoc(const QDomDocument& rhs) : QDomDocument(rhs) {}
  // VXmlDoc& operator = (const VXmlDoc& rhs)        { *((QDomDocument*)this) = *((QDomDocument*)&rhs);  return *this; } // gilgil temp 2012.05.31
  // operator QDomDocument&() { return *this; }
  VXmlDoc(const QString rootTagName)                    { createRoot(rootTagName); }
  VXmlDoc(const VXml xml);

public:
  VXml root();
  // void setRoot(VXml xml); // gilgil temp 2012.07.06

public:
  void clear();
  VXml createRoot(QString value);
  bool loadFromFile(QString fileName);
  bool saveToFile(QString fileName);

public:
  //
  // From QDomDocument
  //
  QString toString() const { return QDomDocument::toString(); }

public:
  static QString defaultFileName();

public:
  static VXmlDoc& instance();
};

#endif // __V_XML_DOC_H__
