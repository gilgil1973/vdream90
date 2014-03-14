#ifdef QT_GUI_LIB

#include "vhttpproxywidget.h"
#include "ui_vhttpproxywidget.h"

VHttpProxyWidget::VHttpProxyWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::VHttpProxyWidget)
{
  ui->setupUi(this);

  ui->glInternal->setMargin(8);
  ui->glExternal->setMargin(8);
  ui->glTcpServer->setMargin(8);
  ui->glSslServer->setMargin(8);
  ui->glInbound->setMargin(8);
  ui->glOutbound->setMargin(8);

  ui->tabInternal->setLayout(ui->glInternal);
  ui->tabExternal->setLayout(ui->glExternal);
  ui->tabTcpServer->setLayout(ui->glTcpServer);
  ui->tabSslServer->setLayout(ui->glSslServer);
  ui->tabDataChange->setLayout(ui->glDataChange);
  ui->tabInbound->setLayout(ui->glInbound);
  ui->tabOutbound->setLayout(ui->glOutbound);
}

VHttpProxyWidget::~VHttpProxyWidget()
{
  delete ui;
}
#endif // QT_GUI_LIB
