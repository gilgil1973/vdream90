#include "vdatachange.h"

// ----------------------------------------------------------------------------
// VRegExp
// ----------------------------------------------------------------------------
VRegExp::VRegExp()
{
  pattern  = "";
  syntax   = QRegExp::FixedString;
  cs       = Qt::CaseSensitive;
  minimal  = false;
}

bool VRegExp::prepare(VError& error)
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

void VRegExp::load(VXml xml)
{
  pattern  = xml.getStr("pattern", pattern);
  syntax   = (QRegExp::PatternSyntax)xml.getInt("syntax", (int)syntax);
  cs       = (Qt::CaseSensitivity)xml.getInt("cs", (int)cs);
  minimal  = xml.getBool("minimal", minimal);
}

void VRegExp::save(VXml xml)
{
  xml.setStr("pattern", pattern);
  xml.setInt("syntax", (int)syntax);
  xml.setInt("cs", (int)cs);
  xml.setBool("minimal", minimal);
}

#ifdef QT_GUI_LIB
void VRegExp::initialize(QTreeWidget* treeWidget)
{
  treeWidget->setIndentation(0);
  treeWidget->setItemsExpandable(false);
  treeWidget->setMinimumWidth(600);
  treeWidget->header()->setStretchLastSection(false);

  QStringList headerLables;
  headerLables << "Pattern" << "Syntax" << "Case" << "Minimal";
  treeWidget->setHeaderLabels(headerLables);

  // treeWidget->setColumnWidth(PATTERN_IDX,   0);
  treeWidget->setColumnWidth(SYNTAX_IDX,    100);
  treeWidget->setColumnWidth(CS_IDX,        30);
  treeWidget->setColumnWidth(MINIMAL_IDX,   30);

  treeWidget->header()->setSectionResizeMode(PATTERN_IDX,   QHeaderView::Stretch);
  treeWidget->header()->setSectionResizeMode(SYNTAX_IDX,    QHeaderView::Interactive);
  treeWidget->header()->setSectionResizeMode(CS_IDX,        QHeaderView::Interactive);
  treeWidget->header()->setSectionResizeMode(MINIMAL_IDX,   QHeaderView::Interactive);
}

void operator << (VRegExp& regexp, QTreeWidgetItem& treeWidgetItem)
{
  regexp.pattern  = treeWidgetItem.text(VRegExp::PATTERN_IDX);
  QComboBox* syntaxComboBox = dynamic_cast<QComboBox*>(treeWidgetItem.treeWidget()->itemWidget(&treeWidgetItem, VRegExp::SYNTAX_IDX));
  regexp.syntax   = (QRegExp::PatternSyntax)(syntaxComboBox->currentIndex());
  regexp.cs       = treeWidgetItem.checkState(VRegExp::CS_IDX) == Qt::Checked ? Qt::CaseSensitive : Qt::CaseInsensitive;
  regexp.minimal  = treeWidgetItem.checkState(VRegExp::MINIMAL_IDX) == Qt::Checked;
}

void operator << (QTreeWidgetItem& treeWidgetItem, VRegExp& regexp)
{
  treeWidgetItem.setText(VRegExp::PATTERN_IDX, regexp.pattern);
  QComboBox* syntaxComboBox = new QComboBox(treeWidgetItem.treeWidget());
  QStringList sl; sl << "RegExp" << "Wildcard" << "FixedString" << "RegExp2" << "WildcardUnix" << "W3CXmlSchema11";
  syntaxComboBox->insertItems(0, sl);
  syntaxComboBox->setCurrentIndex((int)(regexp.syntax));
  treeWidgetItem.treeWidget()->setItemWidget(&treeWidgetItem, VRegExp::SYNTAX_IDX, syntaxComboBox);
  treeWidgetItem.setCheckState(VRegExp::CS_IDX, regexp.cs == Qt::CaseSensitive ? Qt::Checked : Qt::Unchecked);
  treeWidgetItem.setCheckState(VRegExp::MINIMAL_IDX, regexp.minimal ? Qt::Checked : Qt::Unchecked);

  treeWidgetItem.setFlags(treeWidgetItem.flags() | Qt::ItemIsEditable);
}
#endif // QT_GUI_LIB

// ----------------------------------------------------------------------------
// VDataFindItem
// ----------------------------------------------------------------------------
VDataFindItem::VDataFindItem()
{
  enabled = true;
}

int VDataFindItem::find(QByteArray& ba, int offset)
{
  QString text = QString::fromLatin1(ba);

  int index = rx.indexIn(text, offset);
  if (index == -1) return -1;

  QString found = rx.cap(0);
  LOG_INFO("found   \"%s\"", qPrintable(found));

  return index;
}

void VDataFindItem::load(VXml xml)
{
  VRegExp::load(xml);

  enabled = xml.getBool("enabled", enabled);
}

void VDataFindItem::save(VXml xml)
{
  VRegExp::save(xml);

  xml.setBool("enabled", enabled);
}

#ifdef QT_GUI_LIB
void VDataFindItem::initialize(QTreeWidget* treeWidget)
{
  VRegExp::initialize(treeWidget);

  LOG_DEBUG("column count=%d", treeWidget->header()->count()); // gilgil temp 2014.03.19
  QStringList headerLables;
  for (int i = 0; i < treeWidget->columnCount(); i++) headerLables << treeWidget->headerItem()->text(i);
  headerLables << "Enabled";
  treeWidget->setHeaderLabels(headerLables);
  LOG_DEBUG("column count=%d", treeWidget->header()->count()); // gilgil temp 2014.03.19

  treeWidget->setColumnWidth(ENABLED_IDX, 30);

  treeWidget->header()->setSectionResizeMode(ENABLED_IDX,   QHeaderView::Interactive);
}

void operator << (QTreeWidgetItem& treeWidgetItem, VDataFindItem& item)
{
  treeWidgetItem << (VRegExp&)item;

  treeWidgetItem.setCheckState(VDataFindItem::ENABLED_IDX, item.enabled ? Qt::Checked : Qt::Unchecked);
}

void operator << (VDataFindItem& item, QTreeWidgetItem& treeWidgetItem)
{
  (VRegExp&)item << treeWidgetItem;

  item.enabled  = treeWidgetItem.checkState(VDataFindItem::ENABLED_IDX) == Qt::Checked;

  VError error; item.prepare(error);
}
#endif // QT_GUI_LIB

// ----------------------------------------------------------------------------
// VDataFind
// ----------------------------------------------------------------------------
VDataFind::VDataFind()
{
}

VDataFind::~VDataFind()
{
}

bool VDataFind::prepare(VError& error)
{
  for (int i = 0; i < count(); i++)
  {
    VDataFindItem& item = (VDataFindItem&)at(i);
    if (!item.prepare(error)) return false;
  }
  return true;
}

bool VDataFind::find(QByteArray& ba)
{
  bool res = false;
  for (int i = 0; i < count(); i++)
  {
    VDataFindItem& item = (VDataFindItem&)at(i);
    if (!item.enabled) continue;
    if (item.find(ba))
    {
      res = true;
    }
  }
  return res;
}

void VDataFind::load(VXml xml)
{
  clear();
  {
    xml_foreach (childXml, xml.childs())
    {
      VDataFindItem item;
      item.load(childXml);
      push_back(item);
    }
  }
}

void VDataFind::save(VXml xml)
{
  xml.clearChild();
  for (VDataFind::iterator it = begin(); it != end(); it++)
  {
    VDataFindItem& item = *it;
    VXml childXml = xml.addChild("item");
    item.save(childXml);
  }
}

#ifdef QT_GUI_LIB
#include "vdatachangewidget.h"
#include "ui_vdatachangewidget.h"
void VDataFind::optionAddWidget(QLayout* layout)
{
  VDataChangeWidget* widget = new VDataChangeWidget(layout->parentWidget());
  widget->setObjectName("dataFindWidget");
  *(widget->ui->treeWidget) << *this;
  layout->addWidget(widget);
}

void VDataFind::optionSaveDlg(QDialog* dialog)
{
  VDataChangeWidget* widget = dialog->findChild<VDataChangeWidget*>("dataFindWidget");
  LOG_ASSERT(widget != NULL);
  *this << *(widget->ui->treeWidget);
}

void operator << (QTreeWidget& treeWidget, VDataFind& dataFind)
{
  treeWidget.clear();
  QList<QTreeWidgetItem*> treeWidgetItems;
  for (int i = 0; i < dataFind.count(); i++)
  {
    VDataFindItem& item = (VDataFindItem&)dataFind.at(i);
    QTreeWidgetItem* newWidgetItem = new QTreeWidgetItem(&treeWidget);
    *newWidgetItem << item;
    treeWidgetItems.push_back(newWidgetItem);
  }
  treeWidget.insertTopLevelItems(0, treeWidgetItems);
}

void operator << (VDataFind& dataFind, QTreeWidget& treeWidget)
{
  dataFind.clear();
  int count = treeWidget.topLevelItemCount();
  for (int i = 0; i < count; i++)
  {
    QTreeWidgetItem* treeWidgetItem = treeWidget.topLevelItem(i);
    VDataFindItem newItem;
    newItem << *treeWidgetItem;
    dataFind.push_back(newItem);
  }
}
#endif // QT_GUI_LIB
