#ifdef QT_GUI_LIB

#include "vdatachangewidget.h"
#include "ui_vdatachangewidget.h"

VDataChangeWidget::VDataChangeWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::VDataChangeWidget)
{
  ui->setupUi(this);

  layout()->setContentsMargins(0, 0, 0, 0);
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

#endif // QT_GUI_LIB
