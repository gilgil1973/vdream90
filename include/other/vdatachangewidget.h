#ifndef VDATACHANGEWIDGET_H
#define VDATACHANGEWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <VDataChange>

namespace Ui {
  class VDataChangeWidget;
}

class VDataChangeWidget : public QWidget
{
  Q_OBJECT

public:
  explicit VDataChangeWidget(QWidget *parent = 0);
  ~VDataChangeWidget();

public:
  static const int ENABLED_IDX = 0;
  static const int RE_IDX      = 1;
  static const int LOG_IDX     = 2;
  static const int FROM_IDX    = 3;
  static const int TO_IDX      = 4;

public:
  Ui::VDataChangeWidget *ui;
private slots:
  void on_pbAdd_clicked();
  void on_pbDel_clicked();
};

void operator << (VDataChangeItem& item, QTreeWidgetItem& treeWidgetItem);
void operator << (QTreeWidgetItem& treeWidgetItem, VDataChangeItem& item);
void operator << (VDataChange& dataChange, QTreeWidget& treeWidget);
void operator << (QTreeWidget& treeWidget, VDataChange& dataChange);

#endif // VDATACHANGEWIDGET_H
