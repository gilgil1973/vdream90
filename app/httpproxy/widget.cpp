#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Widget)
{
  ui->setupUi(this);
}

Widget::~Widget()
{
  delete ui;
}

void Widget::initializeControl()
{
  netClient    = NULL;
  clientThread = NULL;

  move(0, 0); resize(640, 480);

  setLayout(ui->mainLayout);
  ui->mainLayout->setSpacing(0);
  ui->pteRecv->setWordWrapMode(QTextOption::NoWrap);
  ui->pteSend->setWordWrapMode(QTextOption::NoWrap);
}

void Widget::finalizeControl()
{
  on_pbClose_clicked();
}

void Widget::loadControl()
{
  this->loadFromDefaultDoc("MainWindow");
}

void Widget::saveControl()
{
  this->saveToDefaultDoc("MainWindow");
}

void Widget::setControl(VState state)
{
  LOG_DEBUG(""); // gilgil temp 2014.02.28
  if (state == VState::None)
  {
    int currentIndex = ui->tabOption->currentIndex();
    switch (currentIndex)
    {
      case 0: netClient = &tcpClient; break;
      case 1: netClient = &udpClient; break;
      case 2: netClient = &sslClient; break;
    }
    state = netClient->state();
  }

  ui->pbOpen->setEnabled(state == VState::Closed);
  ui->pbClose->setEnabled(state != VState::Closed);
  ui->tabOption->setEnabled(state == VState::Closed);
  ui->pbSend->setEnabled(state == VState::Opened);
}
