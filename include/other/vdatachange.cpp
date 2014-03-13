#include "vdatachange.h"

// ----------------------------------------------------------------------------
// VDataChangeItem
// ----------------------------------------------------------------------------
VDataChangeItem::VDataChangeItem()
{
  enabled = true;
  re      = false;
  log     = true;
  from    = "";
  to      = "";
}

void VDataChangeItem::load(VXml xml)
{
  enabled = xml.getBool("enabled", enabled);
  re      = xml.getBool("re", re);
  log     = xml.getBool("log", log);
  from    = qPrintable(xml.getStr("from", from));
  to      = qPrintable(xml.getStr("to",   to));
}

void VDataChangeItem::save(VXml xml)
{
  xml.setBool("enabled", enabled);
  xml.setBool("re", re);
  xml.setBool("log", log);
  xml.setStr("from", qPrintable(from));
  xml.setStr("to",   qPrintable(to));
}

// ----------------------------------------------------------------------------
// VDataChangeItems
// ----------------------------------------------------------------------------
VDataChangeItems::VDataChangeItems()
{
}

VDataChangeItems::~VDataChangeItems()
{
}

bool VDataChangeItems::change(QByteArray& ba)
{
  bool res = false;
  QByteArray _old = ba;
  for (int i = 0; i < count(); i++)
  {
    VDataChangeItem& item = (VDataChangeItem&)at(i);
    if (!item.enabled) continue;
    ba.replace(item.from, item.to);
    if (ba != _old)
    {
      if (item.log)
      {
        LOG_INFO("changed %s > %s", qPrintable(item.from), qPrintable(item.to));
      }
      _old = ba;
      res = true;
    }
  }
  return res;
}

void VDataChangeItems::load(VXml xml)
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

void VDataChangeItems::save(VXml xml)
{
  xml.clearChild();
  for (VDataChangeItems::iterator it = begin(); it != end(); it++)
  {
    VDataChangeItem& item = *it;
    VXml childXml = xml.addChild("item");
    item.save(childXml);
  }
}

#ifdef QT_GUI_LIB
#include "vdatachangewidget.h"
#include "ui_vdatachangewidget.h"
void VDataChangeItems::optionAddWidget(QLayout* layout)
{
  VDataChangeWidget* widget = new VDataChangeWidget(layout->parentWidget());
  widget->setObjectName("dataChangeWidget");
  *(widget->ui->treeWidget) << *this;
  layout->addWidget(widget);
}

void VDataChangeItems::optionSaveDlg(QDialog* dialog)
{
  VDataChangeWidget* widget = dialog->findChild<VDataChangeWidget*>("dataChangeWidget");
  LOG_ASSERT(widget != NULL);
  *this << *(widget->ui->treeWidget);
}

#endif // QT_GUI_LIB
