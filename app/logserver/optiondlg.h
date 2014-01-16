#ifndef OPTIONDLG_H
#define OPTIONDLG_H

#include <QDialog>
#include "mylog.h"

namespace Ui {
  class OptionDlg;
}

class OptionDlg : public QDialog
{
  Q_OBJECT

public:
  explicit OptionDlg(QWidget *parent = 0);
  ~OptionDlg();

protected:
  virtual void showEvent(QShowEvent *);
  virtual void closeEvent(QCloseEvent *);

public:
  MyLog* myLog; // reference

private slots:
  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

private:
  Ui::OptionDlg *ui;
};

#endif // OPTIONDLG_H
