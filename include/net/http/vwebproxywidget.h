#ifndef VWEBPROXYWIDGET_H
#define VWEBPROXYWIDGET_H

#ifdef QT_GUI_LIB

#include <QWidget>

namespace Ui {
  class VWebProxyWidget;
}

class VWebProxyWidget : public QWidget
{
  Q_OBJECT

public:
  explicit VWebProxyWidget(QWidget *parent = 0);
  ~VWebProxyWidget();

public:
  Ui::VWebProxyWidget *ui;
};

#endif // QT_GUI_LIB

#endif // VWEBPROXYWIDGET_H
