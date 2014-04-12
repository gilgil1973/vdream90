#ifdef QT_GUI_LIB

#include "vwebproxywidget.h"
#include "ui_vwebproxywidget.h"

VWebProxyWidget::VWebProxyWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::VWebProxyWidget)
{
  ui->setupUi(this);

  ui->glInternal->setMargin(8);
  ui->glExternal->setMargin(8);
  ui->glHttpServer->setMargin(8);
  ui->glHttpsServer->setMargin(8);
  ui->glInbound->setMargin(8);
  ui->glOutbound->setMargin(8);
  ui->glOther->setMargin(8);

  ui->tabInternal->setLayout(ui->glInternal);
  ui->tabExternal->setLayout(ui->glExternal);
  ui->tabHttpServer->setLayout(ui->glHttpServer);
  ui->tabHttpsServer->setLayout(ui->glHttpsServer);
  ui->tabDataChange->setLayout(ui->glDataChange);
  ui->tabInbound->setLayout(ui->glInbound);
  ui->tabOutbound->setLayout(ui->glOutbound);
  ui->tabOther->setLayout(ui->glOther);
}

VWebProxyWidget::~VWebProxyWidget()
{
  delete ui;
}
#endif // QT_GUI_LIB
