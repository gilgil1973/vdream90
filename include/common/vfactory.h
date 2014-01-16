// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_FACTORY_H__
#define __V_FACTORY_H__

#include <VLog>
#include <VXmlDoc>

// ----------------------------------------------------------------------------
// VFactory
// ----------------------------------------------------------------------------
template <class T> // T muse be decendent class of IVXmlable.
class VFactory
{
public:
  //
  // createByClassName
  //
  static T* createByClassName(const QString& className)
  {
    QString parentClassName = CLASS_NAME(T);
    VMetaClassList& list = VMetaClassMap::getList((char*)qPrintable(parentClassName));
    T* res = (T*)list.createByClassName((char*)qPrintable(className));
    if (res == NULL)
    {
      LOG_FATAL("can not create object for %s", qPrintable(className));
    }
    return res;
  }

  //
  // createByXML
  //
  static T* createByXML(VXml xml)
  {
    QString className = xml.getStr("_class", "");
    if (className == "")
    {
      return NULL;
    }
    T* object = createByClassName(className);
    if (object == NULL)
    {
      return NULL;
    }
    IVXmlable* xmlObject = dynamic_cast<IVXmlable*>(object);
    if (xmlObject == NULL)
    {
      LOG_ERROR("xmlObject is NULL");
      return NULL;
    }
    try {
      xmlObject->load(xml);
    }
    catch(...) {
      LOG_FATAL("xmlObject->load exception");
      xmlObject = NULL;
    }
    return (T*)xmlObject;
  }

  //
  // createByDefaultDoc
  //
  static T* createByDefaultDoc(const QString& path)
  {
    VXmlDoc& doc = VXmlDoc::instance();
    VXml xml = doc.root().findChilds(path);
    if (xml.isNull()) return NULL;
    return createByXML(xml);
  }
};

#endif // __V_FACTORY_H__
