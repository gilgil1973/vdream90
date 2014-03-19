#include "vdatachange.h"

// ----------------------------------------------------------------------------
// VDataChangeItem
// ----------------------------------------------------------------------------
VDataChangeItem::VDataChangeItem()
{
  enabled  = true;
  log      = true;
  replace  = "";
}

int VDataChangeItem::change(QByteArray& ba, int offset)
{
  QString text = QString::fromLatin1(ba);

  int index = rx.indexIn(text, offset);
  if (index == -1) return -1;

  QString found = rx.cap(0);
  if (found == replace) return -1;
  text.replace(index, found.length(), replace);
  ba = text.toLatin1();

  if (log)
  {
    LOG_INFO("changed \"%s\" > \"%s\"", qPrintable(found), qPrintable(replace));
  }

  return index;
}

void VDataChangeItem::load(VXml xml)
{
  VRegExp::load(xml);

  enabled  = xml.getBool("enabled", enabled);
  log      = xml.getBool("log", log);
  replace  = qPrintable(xml.getStr("replace", QString(replace)));
}

void VDataChangeItem::save(VXml xml)
{
  VRegExp::save(xml);

  xml.setBool("enabled", enabled);
  xml.setBool("log",     log);
  xml.setStr("replace",  QString(replace));
}

#ifdef QT_GUI_LIB
void VDataChangeItem::initialize(QTreeWidget* treeWidget)
{
  VRegExp::initialize(treeWidget);

  QStringList headerLables;
  for (int i = 0; i < treeWidget->columnCount(); i++) headerLables << treeWidget->headerItem()->text(i);
  headerLables << "Enabled" << "Log" << "Replace";
  treeWidget->setHeaderLabels(headerLables);

  treeWidget->setColumnWidth(ENABLED_IDX, 30);
  treeWidget->setColumnWidth(LOG_IDX,     30);
  // treeWidget->setColumnWidth(REPLACE_IDX, 0);

  treeWidget->header()->setSectionResizeMode(ENABLED_IDX,   QHeaderView::Interactive);
  treeWidget->header()->setSectionResizeMode(LOG_IDX,       QHeaderView::Interactive);
  treeWidget->header()->setSectionResizeMode(REPLACE_IDX,   QHeaderView::Stretch);
}

void operator << (QTreeWidgetItem& treeWidgetItem, VDataChangeItem& item)
{
  treeWidgetItem << (VRegExp&)item;

  treeWidgetItem.setCheckState(VDataChangeItem::ENABLED_IDX, item.enabled ? Qt::Checked : Qt::Unchecked);
  treeWidgetItem.setCheckState(VDataChangeItem::LOG_IDX,     item.log ? Qt::Checked : Qt::Unchecked);
  treeWidgetItem.setText(VDataChangeItem::REPLACE_IDX, QString(item.replace));
}

void operator << (VDataChangeItem& item, QTreeWidgetItem& treeWidgetItem)
{
  (VRegExp&)item << treeWidgetItem;

  item.enabled  = treeWidgetItem.checkState(VDataChangeItem::ENABLED_IDX) == Qt::Checked;
  item.log      = treeWidgetItem.checkState(VDataChangeItem::LOG_IDX)     == Qt::Checked;
  item.replace  = qPrintable(treeWidgetItem.text(VDataChangeItem::REPLACE_IDX));

  VError error; item.prepare(error);
}
#endif // QT_GUI_LIB

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
  for (int i = 0; i < count(); i++)
  {
    VDataChangeItem& item = (VDataChangeItem&)at(i);
    if (!item.enabled) continue;
    if (item.change(ba))
    {
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
#include "vdatafindwidget.h"
#include "ui_vdatafindwidget.h"
void VDataChange::__on_pbAdd_clicked()
{
  QPushButton* pbAdd = dynamic_cast<QPushButton*>(this->sender());
  LOG_ASSERT(pbAdd != NULL);
  VDataFindWidget* widget = dynamic_cast<VDataFindWidget*>(pbAdd->parent());
  LOG_ASSERT(widget != NULL);
  QTreeWidget* treeWidget = widget->ui->treeWidget;

  QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(treeWidget);
  VDataChangeItem newItem;
  *treeWidgetItem << newItem;
  int count = treeWidget->topLevelItemCount();
  treeWidget->insertTopLevelItem(count, treeWidgetItem);
}

void VDataChange::__on_pbDel_clicked()
{
  QPushButton* pbDel = dynamic_cast<QPushButton*>(this->sender());
  LOG_ASSERT(pbDel != NULL);
  VDataFindWidget* widget = dynamic_cast<VDataFindWidget*>(pbDel->parent());
  LOG_ASSERT(widget != NULL);
  QTreeWidget* treeWidget = widget->ui->treeWidget;

  QList<QTreeWidgetItem*> items = treeWidget->selectedItems();
  foreach (QTreeWidgetItem* item, items)
  {
    delete item;
  }
}

void VDataChange::optionAddWidget(QLayout* layout)
{
  VDataFindWidget* widget = new VDataFindWidget(layout->parentWidget());
  VDataChangeItem::initialize(widget->ui->treeWidget);
  widget->setObjectName("dataChangeWidget");
  *(widget->ui->treeWidget) << *this;
  layout->addWidget(widget);
  VObject::connect(widget->ui->pbAdd, SIGNAL(clicked()), this, SLOT(__on_pbAdd_clicked()));
  VObject::connect(widget->ui->pbDel, SIGNAL(clicked()), this, SLOT(__on_pbDel_clicked()));
}

void VDataChange::optionSaveDlg(QDialog* dialog)
{
  VDataFindWidget* widget = dialog->findChild<VDataFindWidget*>("dataChangeWidget");
  LOG_ASSERT(widget != NULL);
  *this << *(widget->ui->treeWidget);
}

void operator << (QTreeWidget& treeWidget, VDataChange& dataChange)
{
  treeWidget.clear();
  QList<QTreeWidgetItem*> treeWidgetItems;
  for (int i = 0; i < dataChange.count(); i++)
  {
    VDataChangeItem& item = (VDataChangeItem&)dataChange.at(i);
    QTreeWidgetItem* newWidgetItem = new QTreeWidgetItem(&treeWidget);
    *newWidgetItem << item;
    treeWidgetItems.push_back(newWidgetItem);
  }
  treeWidget.insertTopLevelItems(0, treeWidgetItems);
}

void operator << (VDataChange& dataChange, QTreeWidget& treeWidget)
{
  dataChange.clear();
  int count = treeWidget.topLevelItemCount();
  for (int i = 0; i < count; i++)
  {
    QTreeWidgetItem* treeWidgetItem = treeWidget.topLevelItem(i);
    VDataChangeItem newItem;
    newItem << *treeWidgetItem;
    dataChange.push_back(newItem);
  }
}
#endif // QT_GUI_LIB
