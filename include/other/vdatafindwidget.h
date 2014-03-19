#ifndef VDATAFINDWIDGET_H
#define VDATAFINDWIDGET_H

#ifdef QT_GUI_LIB

#include <QWidget>
#include <QTreeWidget>

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
};

#endif // QT_GUI_LIB

#endif // VDATAFINDWIDGET_H
