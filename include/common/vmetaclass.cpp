#ifdef GTEST
#include <gtest/gtest.h>
#include <VMetaClass>
#include <QDebug>
#include <VDebugNew>

class Base
{
public:
  Base()             { printf("Base::Base\n");  }
  virtual ~Base()    { printf("Base::~Base\n"); }
};

class Derive1 : public Base
{
public:
  Derive1()          { printf("  Derive1::Derive1\n");  }
  virtual ~Derive1() { printf("  Derive1::~Derive1\n"); }
};

class Derive2 : public Base
{
public:
  Derive2()          { printf("  Derive2::Derive2\n");  }
  virtual ~Derive2() { printf("  Derive2::~Derive2\n"); }
};

class Derive3 : public Base
{
public:
  Derive3()          { printf("  Derive3::Derive3\n");  }
  virtual ~Derive3() { printf("  Derive3::~Derive3\n"); }
};

REGISTER_METACLASS(Derive1, Base)
REGISTER_METACLASS(Derive2, Base)
REGISTER_METACLASS(Derive3, Base)

TEST ( MetaClass, showListTest )
{
  VMetaClassMap& map = VMetaClassMap::instance();
  for (VMetaClassMap::iterator it = map.begin(); it != map.end(); it++)
  {
    VMetaClassList& list = it->second;
    foreach(VMetaClass* metaClass, list)
    {
      qDebug() << "className=" << metaClass->className() << " categoryName=" << metaClass->categoryName();
    }
    qDebug() << "";
  }
}

TEST ( MetaClass, createTest )
{
  VMetaClassList& list = VMetaClassMap::getList("Base");
  foreach(VMetaClass* metaClass, list)
  {
    Base* base = (Base*)metaClass->createInstance();
    delete base;
  }
}

#endif // GTEST
