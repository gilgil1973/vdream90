#ifndef VHTTPPROXYWIDGET_H
#define VHTTPPROXYWIDGET_H

#ifdef QT_GUI_LIB

#include <QWidget>

namespace Ui {
  class VHttpProxyWidget;
}

class VHttpProxyWidget : public QWidget
{
  Q_OBJECT

public:
  explicit VHttpProxyWidget(QWidget *parent = 0);
  ~VHttpProxyWidget();

public:
  Ui::VHttpProxyWidget *ui;
};

#endif // QT_GUI_LIB

#endif // VHTTPPROXYWIDGET_H
