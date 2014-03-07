#ifdef QT_GUI_LIB

#include "vhttpproxywidget.h"
#include "ui_vhttpproxywidget.h"

VHttpProxyWidget::VHttpProxyWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::VHttpProxyWidget)
{
  ui->setupUi(this);
  ui->tabInbound->setLayout(ui->glInbound);
  ui->tabOutbound->setLayout(ui->glOutbound);
  ui->tabTcpServer->setLayout(ui->glTcpServer);
  ui->tabSslServer->setLayout(ui->glSslServer);
}

VHttpProxyWidget::~VHttpProxyWidget()
{
  delete ui;
}
#endif // QT_GUI_LIB
