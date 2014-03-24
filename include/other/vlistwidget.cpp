#include "vlistwidget.h"
#include "ui_vlistwidget.h"

// ----------------------------------------------------------------------------
// VListWidget
// ----------------------------------------------------------------------------
VListWidget::VListWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::VListWidget)
{
  ui->setupUi(this);
}

VListWidget::~VListWidget()
{
  delete ui;
}

// ----------------------------------------------------------------------------
// VLisItemtWidget
// ----------------------------------------------------------------------------
VLisItemtWidget::VLisItemtWidget(QWidget *parent, VListItemtWidgetAccessible* accessible) : VListWidget(parent)
{
  ui->treeWidget->setIndentation(0);
  ui->treeWidget->setItemsExpandable(false);
  ui->treeWidget->setMinimumWidth(600);
  ui->treeWidget->header()->setStretchLastSection(false);

  this->accessible = accessible;
  VObject::connect(ui->pbAdd, SIGNAL(clicked()), this, SLOT(__on_pbAdd_clicked()));
  VObject::connect(ui->pbDel, SIGNAL(clicked()), this, SLOT(__on_pbDel_clicked()));
}

VLisItemtWidget::~VLisItemtWidget()
{
}

void VLisItemtWidget::itemsIntoTreeWidget()
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

void VLisItemtWidget::treeWidgetIntoItems()
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

void VLisItemtWidget::__on_pbAdd_clicked()
{
  QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(ui->treeWidget);
  void* item = accessible->widgetCeateItem();
  accessible->widgetItemIntoTreeWidgetItem(item, treeWidgetItem);
  int count = ui->treeWidget->topLevelItemCount();
  ui->treeWidget->insertTopLevelItem(count, treeWidgetItem);
}

void VLisItemtWidget::__on_pbDel_clicked()
{
  QList<QTreeWidgetItem*> items = ui->treeWidget->selectedItems();
  foreach (QTreeWidgetItem* treeWidgetItem, items)
  {
    delete treeWidgetItem;
  }
};
