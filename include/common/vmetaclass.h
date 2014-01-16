// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_META_CLASS_H__
#define __V_META_CLASS_H__

#include <list>
#include <map>
#include <typeinfo>
#include <string.h> // for strcmp
#include <stdio.h>  // for printf

// ----------------------------------------------------------------------------
// IVMetaClass
// ----------------------------------------------------------------------------
class IVMetaClass
{
public:
  virtual char* className()      = 0;
  virtual char* categoryName()   = 0;
  virtual void* createInstance() = 0;
};

// ----------------------------------------------------------------------------
// VMetaClass
// ----------------------------------------------------------------------------
class VMetaClass : public IVMetaClass
{
public:
  std::type_info* typeInfo;

public:
  virtual void* createInstance()
  {
    printf("***************************************************\n");
    printf("can not create object for class \"%s\"\n", className());
    printf("***************************************************\n");
    return NULL;
  }
};

// ----------------------------------------------------------------------------
// VMetaClassList
// ----------------------------------------------------------------------------
class VMetaClassList : public std::list<VMetaClass*>
{
public:
  void* createByClassName(char* className)
  {
    for (VMetaClassList::iterator it = begin(); it != end(); it++)
    {
      VMetaClass* metaClass = *it;
      char* metaClassName = metaClass->className();
      if (strcmp(metaClassName, className) == 0)
      {
        return metaClass->createInstance();
      }
    }
    return NULL;
  }
};

// ----------------------------------------------------------------------------
// __VLessClassName
// ----------------------------------------------------------------------------
class __VLessClassName
{
public:
  bool operator()(char* left, char* right) const
  {
    return strcmp(left, right) < 0;
  }
};

// ----------------------------------------------------------------------------
// VMetaClassMap
// ----------------------------------------------------------------------------
class VMetaClassMap : public std::map<char* /* categoryName */, VMetaClassList, __VLessClassName>
{
private:
  VMetaClassMap() {}                                // singleton

private:
  VMetaClassMap(const VMetaClassMap&);              // noncopyable
  VMetaClassMap& operator = (const VMetaClassMap&); // noncopyable

public:
  static VMetaClassMap& instance()
  {
    static VMetaClassMap g_instance;
    return g_instance;
  }

  static VMetaClassList& getList(char* categoryName)
  {
    VMetaClassMap& map = instance();
    return map[categoryName];
  }
};

// ----------------------------------------------------------------------------
// VMetaClassMgr
// ----------------------------------------------------------------------------
class VMetaClassMgr
{
public:
  static void* createByClassName(char* className)
  {
    VMetaClassMap& map = VMetaClassMap::instance();
    for (VMetaClassMap::iterator it = map.begin(); it != map.end(); it++)
    {
      VMetaClassList& list = it->second;
      void* res = list.createByClassName(className);
      if (res != NULL) return res;
    }
    return NULL;
  }
};

// ----------------------------------------------------------------------------
// REGISTER_ABSTRACT_METACLASS
// ----------------------------------------------------------------------------
#ifndef REGISTER_ABSTRACT_METACLASS
#define REGISTER_ABSTRACT_METACLASS(ClassName, CategoryName)             \
class VMetaClass##ClassName : public VMetaClass                          \
{                                                                        \
public:                                                                  \
  VMetaClass##ClassName()                                                \
  {                                                                      \
    this->typeInfo         = (std::type_info*)&typeid(ClassName);        \
    VMetaClassList& list   = VMetaClassMap::getList(#CategoryName);      \
    list.push_back(this);                                                \
  }                                                                      \
  virtual ~VMetaClass##ClassName()                                       \
  {                                                                      \
    VMetaClassList& list = VMetaClassMap::getList(this->categoryName()); \
    list.remove(this);                                                   \
  }                                                                      \
  virtual char* className()                                              \
  {                                                                      \
    return (char*)#ClassName;                                            \
  }                                                                      \
  virtual char* categoryName()                                           \
  {                                                                      \
    return (char*)#CategoryName;                                         \
  }                                                                      \
} g_abstractMetaClass_##ClassName;
#endif // REGISTER_ABSTRACT_METACLASS

// ----------------------------------------------------------------------------
// REGISTER_METACLASS
// ----------------------------------------------------------------------------
#ifndef REGISTER_METACLASS
#define REGISTER_METACLASS(ClassName, CategoryName)                      \
class VMetaClass##ClassName : public VMetaClass                          \
{                                                                        \
public:                                                                  \
  VMetaClass##ClassName()                                                \
  {                                                                      \
    this->typeInfo         = (std::type_info*)&typeid(ClassName);        \
    VMetaClassList& list   = VMetaClassMap::getList(#CategoryName);      \
    list.push_back(this);                                                \
  }                                                                      \
  virtual ~VMetaClass##ClassName()                                       \
  {                                                                      \
    VMetaClassList& list = VMetaClassMap::getList(this->categoryName()); \
    list.remove(this);                                                   \
  }                                                                      \
  virtual char* className()                                              \
  {                                                                      \
    return (char*)#ClassName;                                            \
  }                                                                      \
  virtual char* categoryName()                                           \
  {                                                                      \
    return (char*)#CategoryName;                                         \
  }                                                                      \
  virtual void* createInstance()                                         \
  {                                                                      \
    return (void*) new ClassName;                                        \
  }                                                                      \
} g_metaClass_##ClassName;
#endif // REGISTER_METACLASS

#endif // __V_META_CLASS_H__
