#include "vlistwidget.h"
#include "ui_vlistwidget.h"

// ----------------------------------------------------------------------------
// VListWidget
// ----------------------------------------------------------------------------
VListWidget::VListWidget(QWidget *parent, VListWidgetAccessible* accessible) :
  QWidget(parent),
  ui(new Ui::VListWidget)
{
  ui->setupUi(this);

  ui->treeWidget->setIndentation(0);
  ui->treeWidget->setItemsExpandable(false);
  ui->treeWidget->setMinimumWidth(600);
  ui->treeWidget->header()->setStretchLastSection(false);

  this->accessible = accessible;
  VObject::connect(ui->pbAdd, SIGNAL(clicked()), this, SLOT(__on_pbAdd_clicked()));
  VObject::connect(ui->pbDel, SIGNAL(clicked()), this, SLOT(__on_pbDel_clicked()));
}

VListWidget::~VListWidget()
{
  delete ui;
}

void VListWidget::itemsIntoTreeWidget()
{
  ui->treeWidget->clear();
  QList<QTreeWidgetItem*> treeWidgetItems;
  for (int i = 0; i < accessible->widgetCount(); i++)
  {
    void* item = accessible->widgetAt(i);
    QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(ui->treeWidget);
    accessible->widgetItemIntoTreeWidgetItem(item, treeWidgetItem);
    treeWidgetItems.push_back(treeWidgetItem);
  }
  ui->treeWidget->insertTopLevelItems(0, treeWidgetItems);
}

void VListWidget::treeWidgetIntoItems()
{
  accessible->widgetClear();
  int count = ui->treeWidget->topLevelItemCount();
  for (int i = 0; i < count; i++)
  {
    QTreeWidgetItem* treeWidgetItem = ui->treeWidget->topLevelItem(i);
    void* newItem = accessible->widgetCeateItem();
    accessible->widgetTreeWidgetItemIntoItem(treeWidgetItem, newItem);
    accessible->widgetPushBack(newItem);
  }
}

void VListWidget::__on_pbAdd_clicked()
{
  QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(ui->treeWidget);
  void* item = accessible->widgetCeateItem();
  accessible->widgetItemIntoTreeWidgetItem(item, treeWidgetItem);
  int count = ui->treeWidget->topLevelItemCount();
  ui->treeWidget->insertTopLevelItem(count, treeWidgetItem);
}

void VListWidget::__on_pbDel_clicked()
{
  QList<QTreeWidgetItem*> items = ui->treeWidget->selectedItems();
  foreach (QTreeWidgetItem* treeWidgetItem, items)
  {
    delete treeWidgetItem;
  }
};
