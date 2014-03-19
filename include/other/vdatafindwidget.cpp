#ifdef QT_GUI_LIB

#include "vdatafindwidget.h"
#include "ui_vdatafindwidget.h"

VDataFindWidget::VDataFindWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::VDataFindWidget)
{
  ui->setupUi(this);

  layout()->setContentsMargins(0, 0, 0, 0);
}

VDataFindWidget::~VDataFindWidget()
{
  delete ui;
}

#endif // QT_GUI_LIB
