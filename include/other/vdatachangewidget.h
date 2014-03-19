#ifndef VDATACHANGEWIDGET_H
#define VDATACHANGEWIDGET_H

#ifdef QT_GUI_LIB

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
  Ui::VDataChangeWidget *ui;
private slots:
  void on_pbAdd_clicked();
  void on_pbDel_clicked();
};

#endif // VDATACHANGEWIDGET_H

#endif // QT_GUI_LIB
