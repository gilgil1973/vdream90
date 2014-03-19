#ifndef VDATAFINDWIDGET_H
#define VDATAFINDWIDGET_H

#ifdef QT_GUI_LIB

#include <QWidget>
#include <QTreeWidget>
#include <VDataChange>

namespace Ui {
  class VDataFindWidget;
}

class VDataFindWidget : public QWidget
{
  Q_OBJECT

public:
  explicit VDataFindWidget(QWidget *parent = 0);
  ~VDataFindWidget();

public:
  Ui::VDataFindWidget *ui;
private slots:
  void on_pbAdd_clicked();
  void on_pbDel_clicked();
};

#endif // QT_GUI_LIB

#endif // VDATAFINDWIDGET_H
