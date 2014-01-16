#include "main.h"
#include <VXmlDoc>
#include <VDebugNew>

TEST ( XML, testSizeTest )
{
  qDebug() << "sizeof(QDomDocument) is " << sizeof(QDomDocument);
  qDebug() << "sizeof(DomElement) is " << sizeof(QDomElement);
  qDebug() << "sizeof(VXmlDoc) is " << sizeof(VXmlDoc);
  qDebug() << "sizeof(VXml) is " << sizeof(VXml);
}

TEST ( XML, testDefaultValueTest )
{
  VXml xml;

  int i = xml.getInt("int", 999);
  EXPECT_TRUE( i == 999 );

  QChar ch = xml.getChr("char", 'A');
  EXPECT_TRUE( ch == 'A' );

  bool b = xml.getBool("bool", true);
  EXPECT_TRUE( b == true );

  QString s = xml.getStr("string", "gilgil");
  EXPECT_TRUE( s == "gilgil" );
}

//
// Parent -+- Child
//         |
//         +- Child
//
void makeDefaultXML(VXml xml)
{
  xml.setStr("name", "john");
  xml.setInt("age", 40);

  VXml child1 = xml.addChild("child1");
  child1.setStr("name", "harry");
  child1.setInt("age", 16);

  VXml child2 = xml.addChild("child2");
  child2.setStr("name", "sally");
  child2.setInt("age", 15);
}

TEST ( XML, testChildTest )
{
  VXmlDoc doc;

  VXml xml = doc.createRoot("parent");
  makeDefaultXML(xml); // gilgil temp 2012.04.25
  doc.saveToFile("test.xml");

  QString johnName = xml.getStr("name");
  EXPECT_TRUE( johnName == "john" );

  int johnAge = xml.getInt("age");
  EXPECT_TRUE( johnAge == 40 );

  VXmlList xmlList = xml.childs();
  int s = xmlList.size();
  EXPECT_TRUE( s == 2 );

  xml_foreach(child, xml.childs())
  {
    qDebug() << "child : name is " << child.getStr("name") << " and " << child.getInt("age") << " years old.";
  }
}
