#include "aboutdlg.h"
#include "ui_aboutdlg.h"
#include <VCommon>

AboutDlg::AboutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDlg)
{
    ui->setupUi(this);
}

AboutDlg::~AboutDlg()
{
    delete ui;
}

void AboutDlg::showEvent(QShowEvent *)
{
  ui->lblVDreamVersion->setText(VDREAM_VERSION);
}

void AboutDlg::on_btnOK_clicked()
{
  this->close();
}
