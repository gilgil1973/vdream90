#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <VHttpProxy>

namespace Ui {
  class Widget;
}

class Widget : public QWidget, public VXmlable
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

public:
  VHttpProxy proxy;

protected:
  virtual void showEvent(QShowEvent* showEvent);

public slots:
  void showMessage(QString msg);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

private slots:
  void on_pbOpen_clicked();

  void on_pbClose_clicked();

  void on_pbOption_clicked();

private:
  Ui::Widget *ui;
};

#endif // WIDGET_H
