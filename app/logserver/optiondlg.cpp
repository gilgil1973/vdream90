#include "optiondlg.h"
#include "ui_optiondlg.h"
#include "mylog.h"

OptionDlg::OptionDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionDlg)
{
    ui->setupUi(this);
}

void OptionDlg::showEvent(QShowEvent *)
{
  myLog->loadFromDefaultDoc("myLog");
  ui->lePort->setText(QString::number(myLog->udpServer->port));
  ui->chkAutoOpen->setChecked(myLog->autoOpen);
  ui->chkTimeShow->setChecked(myLog->timeShow);
}

void OptionDlg::closeEvent(QCloseEvent *)
{
}

OptionDlg::~OptionDlg()
{
    delete ui;
}

void OptionDlg::on_buttonBox_accepted()
{
  myLog->udpServer->port = ui->lePort->text().toInt();
  myLog->autoOpen        = ui->chkAutoOpen->checkState();
  myLog->timeShow        = ui->chkTimeShow->checkState();
  myLog->saveToDefaultDoc("myLog");
}

void OptionDlg::on_buttonBox_rejected()
{
  this->close();
}
