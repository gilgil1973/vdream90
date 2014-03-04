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
  QString fileName         = VApp::filePath();
  QString path             = QFileInfo(fileName).path();
  QString completeBaseName = QFileInfo(fileName).completeBaseName();
  QString res              = path + "/" + completeBaseName + ".xml";
  return res;
}

VXmlDoc& VXmlDoc::instance()
{
  return VXmlDocInstance::instance();
}
