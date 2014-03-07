#include "vhttpproxywidget.h"
#include "ui_vhttpproxywidget.h"

VHttpProxyWidget::VHttpProxyWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::VHttpProxyWidget)
{
  ui->setupUi(this);
  ui->tabCommon->setLayout(ui->glCommon);
  ui->tabOutPolicy->setLayout(ui->glOutPolicy);
  ui->tabTcpServer->setLayout(ui->glTcpServer);
  ui->tabSslServer->setLayout(ui->glSslServer);
}

VHttpProxyWidget::~VHttpProxyWidget()
{
  delete ui;
}
