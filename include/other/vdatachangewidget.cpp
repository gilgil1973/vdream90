#ifdef QT_GUI_LIB

#include "vdatachangewidget.h"
#include "ui_vdatachangewidget.h"

VDataChangeWidget::VDataChangeWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::VDataChangeWidget)
{
  ui->setupUi(this);

  layout()->setContentsMargins(0, 0, 0, 0);
  QStringList headerLables; headerLables << "Enabled" << "Log" << "Pattern" << "Syntax" << "Case" << "Minimal" << "Replace";

  ui->treeWidget->setHeaderLabels(headerLables);
  ui->treeWidget->setColumnWidth(ENABLED_IDX, 70);
  ui->treeWidget->setColumnWidth(LOG_IDX,     30);
  ui->treeWidget->setColumnWidth(SYNTAX_IDX,  100);
  ui->treeWidget->setColumnWidth(CS_IDX,      30);
  ui->treeWidget->setColumnWidth(MINIMAL_IDX, 70);


  ui->treeWidget->header()->setSectionResizeMode(ENABLED_IDX, QHeaderView::Fixed);
  ui->treeWidget->header()->setSectionResizeMode(LOG_IDX,     QHeaderView::Fixed);
  ui->treeWidget->header()->setSectionResizeMode(PATTERN_IDX, QHeaderView::Stretch);
  ui->treeWidget->header()->setSectionResizeMode(SYNTAX_IDX,  QHeaderView::Fixed);
  ui->treeWidget->header()->setSectionResizeMode(CS_IDX,      QHeaderView::Fixed);
  ui->treeWidget->header()->setSectionResizeMode(MINIMAL_IDX, QHeaderView::Fixed);
  ui->treeWidget->header()->setSectionResizeMode(REPLACE_IDX, QHeaderView::Stretch);
  ui->treeWidget->header()->setStretchLastSection(false);
  ui->treeWidget->setMinimumWidth(600);
}

VDataChangeWidget::~VDataChangeWidget()
{
  delete ui;
}

void VDataChangeWidget::on_pbAdd_clicked()
{
  QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(ui->treeWidget);
  VDataChangeItem newItem;
  *treeWidgetItem << newItem;
  int count = ui->treeWidget->topLevelItemCount();
  ui->treeWidget->insertTopLevelItem(count, treeWidgetItem);
}

void VDataChangeWidget::on_pbDel_clicked()
{
  QList<QTreeWidgetItem*> items = ui->treeWidget->selectedItems();
  foreach (QTreeWidgetItem* item, items)
  {
    delete item;
  }
}

void operator << (VDataChangeItem& item, QTreeWidgetItem& treeWidgetItem)
{
  item.enabled = treeWidgetItem.checkState(VDataChangeWidget::ENABLED_IDX) == Qt::Checked;
  item.log     = treeWidgetItem.checkState(VDataChangeWidget::LOG_IDX)     == Qt::Checked;
  item.pattern = treeWidgetItem.text(VDataChangeWidget::PATTERN_IDX);
  QComboBox* syntaxComboBox = dynamic_cast<QComboBox*>(treeWidgetItem.treeWidget()->itemWidget(&treeWidgetItem, VDataChangeWidget::SYNTAX_IDX));
  item.syntax  = (QRegExp::PatternSyntax)(syntaxComboBox->currentIndex());
  item.cs      = treeWidgetItem.checkState(VDataChangeWidget::CS_IDX) == Qt::Checked ? Qt::CaseSensitive : Qt::CaseInsensitive;
  item.minimal = treeWidgetItem.checkState(VDataChangeWidget::MINIMAL_IDX) == Qt::Checked;
  item.replace = qPrintable(treeWidgetItem.text(VDataChangeWidget::REPLACE_IDX));
  VError error; item.prepare(error);
}

void operator << (QTreeWidgetItem& treeWidgetItem, VDataChangeItem& item)
{
  treeWidgetItem.setCheckState(VDataChangeWidget::ENABLED_IDX, item.enabled ? Qt::Checked : Qt::Unchecked);
  treeWidgetItem.setCheckState(VDataChangeWidget::LOG_IDX,     item.log ? Qt::Checked : Qt::Unchecked);
  treeWidgetItem.setText(VDataChangeWidget::PATTERN_IDX, item.pattern);
  QComboBox* syntaxComboBox = new QComboBox(treeWidgetItem.treeWidget());
  QStringList sl; sl << "RegExp" << "Wildcard" << "FixedString" << "RegExp2" << "WildcardUnix" << "W3CXmlSchema11";
  syntaxComboBox->insertItems(0, sl);
  syntaxComboBox->setCurrentIndex((int)(item.syntax));
  treeWidgetItem.treeWidget()->setItemWidget(&treeWidgetItem, VDataChangeWidget::SYNTAX_IDX, syntaxComboBox);
  treeWidgetItem.setCheckState(VDataChangeWidget::CS_IDX, item.cs == Qt::CaseSensitive ? Qt::Checked : Qt::Unchecked);
  treeWidgetItem.setCheckState(VDataChangeWidget::MINIMAL_IDX, item.minimal ? Qt::Checked : Qt::Unchecked);
  treeWidgetItem.setText(VDataChangeWidget::REPLACE_IDX, QString(item.replace));

  treeWidgetItem.setFlags(treeWidgetItem.flags() | Qt::ItemIsEditable);
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

#endif // QT_GUI_LIB
