#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

#include <VNetServer>

namespace Ui {
  class Dialog;
}

class Dialog : public QDialog, public VXmlable
{
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = 0);
  ~Dialog();

public:
  void setControl();

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

private:
  Ui::Dialog *ui;
};

#endif // DIALOG_H
