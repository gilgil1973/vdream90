#include <VXml>
#include <VXmlDoc>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VXml
// ----------------------------------------------------------------------------
void VXml::clearChild()
{
  QDomElement* p = (QDomElement*)this;
  while (!QDomElement::childNodes().isEmpty())
  {
    p->removeChild(QDomElement::childNodes().at(0));
  }
}

VXml VXml::addChild(QString tagName)
{
  /*
  QDomElement element = this->ownerDocument().createElement(tagName);
  this->appendChild(element);
  VXml res = element; return res;
  */
  VXml res = QDomElement::ownerDocument().createElement(tagName);
  QDomElement::appendChild(res);
  return res;
}

VXml VXml::removeChild(QString tagName)
{
  VXml res = findChild(tagName);
  if (!res.isNull())
  {
    QDomElement::removeChild(res);
  }
  return res;
}

VXml VXml::gotoChild(QString tagName)
{
  VXml emt = findChild(tagName);
  if (emt.isNull())
    emt = addChild(tagName);
  return emt;
}

VXml VXml::gotoChilds(QString tagNames, QString delimiter)
{
  QStringList strList = tagNames.split(delimiter);
  VXml res = *this;
  foreach(const QString& s, strList)
  {
    res = res.gotoChild(s);
  }
  return res;
}

VXmlList VXml::childs()
{
  return childNodes();
  // QDomNodeList list = childNodes(); return *((VXmlList*)&list);
}

VXml VXml::findChild(QString tagName)
{
  int count = this->childNodes().count();
  for (int i = 0; i < count; i++)
  {
    QDomNode node = this->childNodes().at(i);
    if (!node.isElement()) continue;
    QDomElement element = node.toElement();
    if (tagName == element.tagName())
    {
      return element;
    }
  }
  return QDomElement(); // blank; return blank;
}

VXml VXml::findChilds(QString tagNames, QString delimiter)
{
  QStringList strList = tagNames.split(delimiter);
  VXml res = *this;
  foreach(const QString& s, strList)
  {
    res = res.findChild(s);
    if (res.isNull())
    {
      return QDomElement();
    }
  }
  return res;
}

int VXml::getInt(const QString name,  int def)
{
  if (!this->attributes().contains(name)) return def;
  QString s = this->attribute(name, s);
  if (s == "") return def;
  return this->attribute(name, s).toInt();
}

void VXml::setInt(const QString name, int val)
{
  this->setAttribute(name, val);
}

QChar VXml::getChr(const QString name, QChar def)
{
  if (!this->attributes().contains(name)) return def;
  QString s = this->attribute(name, s);
  if (s == "") return def;
  if (s.size() == 0) return def;
  return s.at(0);
}

void VXml::setChr(const QString name, QChar val)
{
  this->setAttribute(name, val);
}

bool VXml::getBool(const QString name, bool def)
{
  if (!this->attributes().contains(name)) return def;
  QString s = this->attribute(name, s);
  if (s == "") return def;
  return s.toLower() == "true";
}

void VXml::setBool(const QString name, bool val)
{
  this->setAttribute(name, val ? "true" : "false");
}

double VXml::getDouble(const QString name, double def)
{
  if (!this->attributes().contains(name)) return def;
  QString s = this->attribute(name, s);
  if (s == "") return def;
  return s.toDouble();
}

void VXml::setDouble(const QString name, double val)
{
  this->setAttribute(name, val);
}

QString VXml::getStr(const QString name, QString def)
{
  if (!this->attributes().contains(name)) return def;
  QString s = this->attribute(name, s);
  // if (s == "") return def; // gilgil temp 2012.08.13
  return s;
}

void VXml::setStr(const QString name, QString val)
{
  this->setAttribute(name, val);
}

QByteArray VXml::getArr(const QString name, QByteArray def)
{
  if (!this->attributes().contains(name)) return def;
  QString s = this->attribute(name, s);
  // if (s == "") return def; // gilgil temp 2012.08.13
  return QByteArray(qPrintable(s));
}

void VXml::setArr(const QString name, QByteArray val)
{
  this->setAttribute(name, QString(val));
}

void VXml::removeAttr(const QString name)
{
  this->removeAttribute(name);
}

// ----------------------------------------------------------------------------
// VXmlable
// ----------------------------------------------------------------------------
bool VXmlable::loadFromFile(const QString& fileName, const QString& path)
{
  VXmlDoc doc;
  if (!doc.loadFromFile(fileName)) return false;
  if (doc.root().isNull()) return false;
  VXml xml = (path == "") ? doc.root() : doc.root().findChilds(path);
  if (xml.isNull()) return false;
  this->load(xml);
  return true;
}

bool VXmlable::saveToFile(const QString& fileName, const QString& path)
{
  VXmlDoc doc;
  doc.loadFromFile(fileName);
  if (doc.root().isNull())
  {
    QString rootTagName = QFileInfo(fileName).completeBaseName();
    doc.createRoot(rootTagName);
  }
  VXml xml = (path == "") ? doc.root() : doc.root().gotoChild(path);
  this->save(xml);
  bool res = doc.saveToFile(fileName);
  return res;
}

bool VXmlable::loadFromDefaultDoc(const QString& path)
{
  VXmlDoc& doc = VXmlDoc::instance();
  VXml xml = doc.root().findChilds(path);
  if (xml.isNull()) return false;
  this->load(xml);
  return true;
}

bool VXmlable::saveToDefaultDoc(const QString& path)
{
  VXmlDoc& doc = VXmlDoc::instance();
  VXml xml = doc.root().gotoChilds(path);
  if (xml.isNull()) return false;
  this->save(xml);
  return true;
}

// ----------------------------------------------------------------------------
// VXmlList
// ----------------------------------------------------------------------------
VXml VXmlList::at(int index)
{
  // QDomNode node = QDomNodeList::at(index); // gilgi temp 2012.05.31
  // return *((VXml*)&node);
  return QDomNodeList::at(index).toElement();
}
