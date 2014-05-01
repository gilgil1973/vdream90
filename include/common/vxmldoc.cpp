#include <VXmlDoc>
#include <VApp>    // for filePath
#include <VLog>       // for LOG_ERROR
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VXmlDocInstance
// ----------------------------------------------------------------------------
class VXmlDocInstance : public VXmlDoc
{
public:
  VXmlDocInstance()
  {
    QString fileName    = defaultFileName();
    QString rootTagName = QFileInfo(fileName).completeBaseName();
    createRoot(rootTagName);
  }

  virtual ~VXmlDocInstance()
  {
  }

public:
  static VXmlDocInstance& instance()
  {
    static VXmlDocInstance doc;
    return doc;
  }
};

// ----------------------------------------------------------------------------
// VXmlDoc
// ----------------------------------------------------------------------------
VXmlDoc::VXmlDoc(const VXml xml)
{
  QDomNode* _this = (QDomNode*)this;
  QDomNode* _xml  = (QDomNode*)&xml;
  *_this = *_xml;
}

VXml VXmlDoc::root()
{
  //QDomElement element = this->documentElement(); // gilgil temp 2012.05.31
  //VXml res = *((VXml*)&element);
  VXml res = QDomDocument::documentElement();
#ifdef _DEBUG
  if (res.isNull())
  {
    LOG_WARN("root is NULL");
  }
#endif // _DEBUG
  return res;
}

// ----- gilgil temp 2012.07.06 -----
/*
void VXmlDoc::setRoot(VXml xml)
{
  clear();
  createRoot(xml.tagName());
  VXml& _root = root();
  _root = xml.cloneNode();
  // root().appendChild(xml); // gilgil temp 2012.06.10
}
*/
// ----------------------------------

void VXmlDoc::clear()
{
  QDomDocument::clear();
}

VXml VXmlDoc::createRoot(QString value)
{
  if (!this->documentElement().isNull()) // if (!root().isNull())
  {
    LOG_ERROR("root is not null. call clear before createRoot");
    QDomElement element; return *((VXml*)(&element));
  }
  QDomElement res = QDomDocument::createElement(value);
  QDomDocument::appendChild(res).toElement();
  // VXml res = *((VXml*)&element); return res; // gilgil temp 2012.05.31
  return res;
}

bool VXmlDoc::loadFromFile(QString fileName)
{
  if (!QFile::exists(fileName))
  {
    LOG_ERROR("file(%s) not exists", qPrintable(fileName));
    return false;
  }

  bool res;
  QFile file(fileName);

  res = file.open(QIODevice::ReadOnly);
  if (!res)
  {
    QString error = file.errorString();
    LOG_ERROR("file open(%s) return false %s", qPrintable(fileName), qPrintable(error));
    return false;
  }

  VXmlDoc tempDoc;
  QString errorMsg;
  int errorLine;
  int errorColumn;
  res = tempDoc.setContent(&file, &errorMsg, &errorLine, &errorColumn);
  file.close();
  if (!res)
  {
    LOG_ERROR("setContent(%s) return false %s(%d:%d)", qPrintable(fileName), qPrintable(errorMsg), errorLine, errorColumn);
    return false;
  }
  *this = tempDoc.cloneNode().toDocument();
  return true;
}

bool VXmlDoc::saveToFile(QString fileName)
{
  bool res;
  QFile file(fileName);

  res = file.open(QIODevice::WriteOnly | QIODevice::Text);
  if (!res)
  {
    QString error = file.errorString();
    LOG_ERROR("file open(%s) return false", qPrintable(fileName), qPrintable(error));
    return false;
  }

  QTextStream stream(&file);
  this->save(stream, 2, QDomNode::EncodingFromDocument);
  file.close();

  return true;
}

QString VXmlDoc::defaultFileName()
{
  QString fileName         = VApp::fileName();
  QString path             = QFileInfo(fileName).path();
  QString completeBaseName = QFileInfo(fileName).completeBaseName();
  QString res              = path + QDir::separator() + completeBaseName + ".xml";
  return res;
}

VXmlDoc& VXmlDoc::instance()
{
  return VXmlDocInstance::instance();
}

#ifdef GTEST
#include <gtest/gtest.h>

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

#endif // GTEST
