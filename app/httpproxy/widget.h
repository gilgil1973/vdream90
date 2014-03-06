#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <VHttpProxy>

namespace Ui {
  class Widget;
}

class Widget : public QWidget
{
  Q_OBJECT

public:
  explicit Widget(QWidget *parent = 0);
  ~Widget();

public:
  void initializeControl();
  void finalizeControl();
  void loadControl();
  void saveControl();
  void setControl(VState state = VState::None);

protected:
  virtual void showEvent(QShowEvent* showEvent);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

private:
  Ui::Widget *ui;
};

#endif // WIDGET_H
