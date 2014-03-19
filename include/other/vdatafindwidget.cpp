#ifdef QT_GUI_LIB

#include "vdatafindwidget.h"
#include "ui_vdatafindwidget.h"

VDataFindWidget::VDataFindWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::VDataFindWidget)
{
  ui->setupUi(this);

  layout()->setContentsMargins(0, 0, 0, 0);
}

VDataFindWidget::~VDataFindWidget()
{
  delete ui;
}

void VDataFindWidget::on_pbAdd_clicked()
{
  QTreeWidgetItem* treeWidgetItem = new QTreeWidgetItem(ui->treeWidget);
  VDataChangeItem newItem;
  *treeWidgetItem << newItem;
  int count = ui->treeWidget->topLevelItemCount();
  ui->treeWidget->insertTopLevelItem(count, treeWidgetItem);
}

void VDataFindWidget::on_pbDel_clicked()
{
  QList<QTreeWidgetItem*> items = ui->treeWidget->selectedItems();
  foreach (QTreeWidgetItem* item, items)
  {
    delete item;
  }
}

#endif // QT_GUI_LIB
