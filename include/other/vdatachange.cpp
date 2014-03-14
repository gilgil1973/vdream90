#include "vdatachange.h"

// ----------------------------------------------------------------------------
// VDataChangeItem
// ----------------------------------------------------------------------------
VDataChangeItem::VDataChangeItem()
{
  enabled = true;
  pattern = "";
  syntax  = QRegExp::RegExp;
  cs      = Qt::CaseSensitive;
  minimal = false;
  replace = "";
}

bool VDataChangeItem::prepare(VError& error)
{
  // gilgil temp 2014.03.15 order ???
  rx.setPattern(pattern);
  rx.setCaseSensitivity(cs);
  rx.setMinimal(minimal);
  rx.setPatternSyntax(syntax);

  if (!rx.isValid())
  {
    SET_ERROR(VError, qformat("rx is not valid(%s)", qPrintable(pattern)), VERR_UNKNOWN);
    return false;
  }

  return true;
}

bool VDataChangeItem::change(QByteArray& ba)
{
  QString text(ba);

  int index = rx.indexIn(text);
  if (index == -1) return false;
  LOG_DEBUG("index=%d", index); // gilgil temp 2014.03.15

  QString from = rx.cap(0);
  ba.replace(index, from.length(), replace);
  if (log)
  {
    LOG_INFO("changed %s > %s", qPrintable(from), qPrintable(replace));
  }
  return true;
}

void VDataChangeItem::load(VXml xml)
{
  enabled = xml.getBool("enabled", enabled);
  log     = xml.getBool("log", log);
  pattern = xml.getStr("pattern", pattern);
  syntax  = (QRegExp::PatternSyntax)xml.getInt("syntax", (int)syntax);
  cs      = (Qt::CaseSensitivity)xml.getInt("cs", (int)cs);
  minimal = xml.getBool("minimal", minimal);
  replace = qPrintable(xml.getStr("replace", QString(replace)));
}

void VDataChangeItem::save(VXml xml)
{
  xml.setBool("enabled", enabled);
  xml.setBool("log", log);
  xml.setStr("pattern", pattern);
  xml.setInt("syntax", (int)syntax);
  xml.setInt("cs", (int)cs);
  xml.setBool("minimal", minimal);
  xml.setStr("replace", QString(replace));
}

// ----------------------------------------------------------------------------
// VDataChange
// ----------------------------------------------------------------------------
VDataChange::VDataChange()
{
}

VDataChange::~VDataChange()
{
}

bool VDataChange::prepare(VError& error)
{
  for (int i = 0; i < count(); i++)
  {
    VDataChangeItem& item = (VDataChangeItem&)at(i);
    if (!item.prepare(error)) return false;
  }
  return true;
}

bool VDataChange::change(QByteArray& ba)
{
  bool res = false;
  QByteArray _old = ba;
  for (int i = 0; i < count(); i++)
  {
    VDataChangeItem& item = (VDataChangeItem&)at(i);
    if (!item.enabled) continue;
    item.change(ba);
    if (ba != _old)
    {
      res = true;
      _old = ba;
      res = true;
    }
  }
  return res;
}

void VDataChange::load(VXml xml)
{
  clear();
  {
    xml_foreach (childXml, xml.childs())
    {
      VDataChangeItem item;
      item.load(childXml);
      push_back(item);
    }
  }
}

void VDataChange::save(VXml xml)
{
  xml.clearChild();
  for (VDataChange::iterator it = begin(); it != end(); it++)
  {
    VDataChangeItem& item = *it;
    VXml childXml = xml.addChild("item");
    item.save(childXml);
  }
}

#ifdef QT_GUI_LIB
#include "vdatachangewidget.h"
#include "ui_vdatachangewidget.h"
void VDataChange::optionAddWidget(QLayout* layout)
{
  VDataChangeWidget* widget = new VDataChangeWidget(layout->parentWidget());
  widget->setObjectName("dataChangeWidget");
  *(widget->ui->treeWidget) << *this;
  layout->addWidget(widget);
}

void VDataChange::optionSaveDlg(QDialog* dialog)
{
  VDataChangeWidget* widget = dialog->findChild<VDataChangeWidget*>("dataChangeWidget");
  LOG_ASSERT(widget != NULL);
  *this << *(widget->ui->treeWidget);
}

#endif // QT_GUI_LIB
