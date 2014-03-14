#include "vdatachangewidget.h"
#include "ui_vdatachangewidget.h"

VDataChangeWidget::VDataChangeWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::VDataChangeWidget)
{
  ui->setupUi(this);

  layout()->setContentsMargins(0, 0, 0, 0);
  QStringList headerLables; headerLables << "Enabled" << "RegExp" << "Log" << "From" << "To";
  ui->treeWidget->setHeaderLabels(headerLables);
  ui->treeWidget->setColumnWidth(ENABLED_IDX, 70);
  ui->treeWidget->setColumnWidth(RE_IDX,      70);
  ui->treeWidget->setColumnWidth(LOG_IDX,     70);
  ui->treeWidget->header()->setSectionResizeMode(ENABLED_IDX, QHeaderView::Fixed);
  ui->treeWidget->header()->setSectionResizeMode(RE_IDX,      QHeaderView::Fixed);
  ui->treeWidget->header()->setSectionResizeMode(LOG_IDX,     QHeaderView::Fixed);
  ui->treeWidget->header()->setSectionResizeMode(FROM_IDX,    QHeaderView::Stretch);
  ui->treeWidget->header()->setSectionResizeMode(TO_IDX,      QHeaderView::Stretch);
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
  item.re      = treeWidgetItem.checkState(VDataChangeWidget::RE_IDX)      == Qt::Checked;
  item.log     = treeWidgetItem.checkState(VDataChangeWidget::LOG_IDX)     == Qt::Checked;
  item.from    = qPrintable(treeWidgetItem.text(VDataChangeWidget::FROM_IDX));
  item.to      = qPrintable(treeWidgetItem.text(VDataChangeWidget::TO_IDX));
}

void operator << (QTreeWidgetItem& treeWidgetItem, VDataChangeItem& item)
{
  treeWidgetItem.setCheckState(VDataChangeWidget::ENABLED_IDX, item.enabled ? Qt::Checked : Qt::Unchecked);
  treeWidgetItem.setCheckState(VDataChangeWidget::RE_IDX,      item.re      ? Qt::Checked : Qt::Unchecked);
  treeWidgetItem.setCheckState(VDataChangeWidget::LOG_IDX,     item.log     ? Qt::Checked : Qt::Unchecked);
  treeWidgetItem.setText(VDataChangeWidget::FROM_IDX, item.from);
  treeWidgetItem.setText(VDataChangeWidget::TO_IDX,   item.to);
  treeWidgetItem.setFlags(treeWidgetItem.flags() | Qt::ItemFlag::ItemIsEditable);
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
