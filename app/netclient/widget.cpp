#include "widget.h"
#include "ui_widget.h"
#include <VDebugNew>

// ----------------------------------------------------------------------------
// ClientThread
// ----------------------------------------------------------------------------
ClientThread::ClientThread(void* owner, VNetClient* netClient) : VThread(owner)
{
  this->widget    = (Widget*)owner;
  this->netClient = netClient;
}

ClientThread::~ClientThread()
{
  close();
}

void ClientThread::fireEvent(QEvent* event)
{
  QApplication::postEvent(widget, event);
}

void ClientThread::run()
{
  LOG_ASSERT(netClient != NULL);
  fireEvent(new StateEvent(VState::Opening));

  bool res = netClient->open();
  if (!res)
  {
    fireEvent(new MsgEvent(netClient->error.msg, QThread::currentThreadId()));
    fireEvent(new CloseEvent);
    return;
  }

  if (dynamic_cast<VUdpClient*>(netClient) == NULL)
  {
    fireEvent(new MsgEvent("******** connected ********", QThread::currentThreadId()));
  }
  fireEvent(new StateEvent(VState::Opened));

  while (true)
  {
    QByteArray ba;

    int readLen = netClient->read(ba);
    if (readLen == VERR_FAIL) break;

    if (widget->ui->chkShowHexa->checkState() == Qt::Checked)
      ba = ba.toHex();
    QString msg = ba;
    fireEvent(new MsgEvent(msg, QThread::currentThreadId()));
  }

  if (dynamic_cast<VUdpClient*>(netClient) == NULL)
  {
    fireEvent(new MsgEvent("******** disconnected ********", QThread::currentThreadId()));
  }
  fireEvent(new CloseEvent);
}

// ----------------------------------------------------------------------------
// Widget
// ----------------------------------------------------------------------------
Widget::Widget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::Widget)
{
  ui->setupUi(this);
  initializeControl();
}

Widget::~Widget()
{
  saveControl();
  finalizeControl();
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

bool Widget::event(QEvent* event)
{
  StateEvent* stateEvent = dynamic_cast<StateEvent*>(event);
  if (stateEvent != NULL)
  {
    setControl();
    return true;
  }

  MsgEvent* msgEvent = dynamic_cast<MsgEvent*>(event);
  if (msgEvent != NULL)
  {
    ui->pteRecv->insertPlainText(msgEvent->msg + "\n");
    ui->pteRecv->ensureCursorVisible();
    return true;
  }

  CloseEvent* closeEvent = dynamic_cast<CloseEvent*>(event);
  if (closeEvent != NULL)
  {
    ui->pbClose->click();
    return true;
  }

  return QWidget::event(event);
}

void Widget::showEvent(QShowEvent* showEvent)
{
  loadControl();
  setControl();
  QWidget::showEvent(showEvent);
}

void Widget::showMessage(MsgEvent* event)
{
  static Qt::HANDLE lastThreadId = 0;
  if (lastThreadId != 0 && lastThreadId != event->threadId) event->msg = QString("\r\n") + event->msg;
  lastThreadId = event->threadId;
  ui->pteRecv->insertPlainText(event->msg);
  ui->pteRecv->ensureCursorVisible();
}

void Widget::load(VXml xml)
{
  {
    VXml coordXml = xml.findChild("coord");
    if (!coordXml.isNull())
    {
      QRect rect = geometry();
      rect.setLeft  ((coordXml.getInt("left",   0)));
      rect.setTop   ((coordXml.getInt("top",    0)));
      rect.setWidth ((coordXml.getInt("width",  640)));
      rect.setHeight((coordXml.getInt("height", 480)));
      setGeometry(rect);
    }
  }

  ui->chkShowHexa->setCheckState((Qt::CheckState)xml.getInt("showHexa", (int)ui->chkShowHexa->checkState()));
  ui->chkSendHexa->setCheckState((Qt::CheckState)xml.getInt("sendHexa", (int)ui->chkSendHexa->checkState()));
  ui->tabOption->setCurrentIndex(xml.getInt("currentIndex", 0));
  ui->leTcpHost->setText(xml.getStr("tcpHost", ui->leTcpHost->text()));
  ui->leTcpPort->setText(xml.getStr("tcpPort", ui->leTcpPort->text()));
  ui->leUdpHost->setText(xml.getStr("udpHost", ui->leUdpHost->text()));
  ui->leUdpPort->setText(xml.getStr("udpPort", ui->leUdpPort->text()));
  ui->leSslHost->setText(xml.getStr("sslHost", ui->leSslHost->text()));
  ui->leSslPort->setText(xml.getStr("sslPort", ui->leSslPort->text()));
  ui->pteSend->insertPlainText(xml.getStr("sendText", ui->pteSend->toPlainText()));

  {
    VXml sizesXml = xml.findChild("sizes");
    QList<int> sizes;
    if (!sizesXml.isNull())
    {
      QStringList strList = sizesXml.getStr("splitter").split(",");
      foreach (QString s, strList) sizes << s.toInt();
      ui->splitter->setSizes(sizes);
    }
  }

  tcpClient.load(xml.gotoChilds("netClient/tcpClient"));
  udpClient.load(xml.gotoChilds("netClient/udpClient"));
  sslClient.load(xml.gotoChilds("netClient/sslClient"));
}

void Widget::save(VXml xml)
{
  {
    VXml coordXml = xml.gotoChild("coord");
    QRect rect = geometry();
    coordXml.setInt("left",   rect.left());
    coordXml.setInt("top",    rect.top());
    coordXml.setInt("width",  rect.width());
    coordXml.setInt("height", rect.height());
  }

  xml.setInt("showHexa", (int)ui->chkShowHexa->checkState());
  xml.setInt("sendHexa", (int)ui->chkSendHexa->checkState());
  xml.setInt("currentIndex", ui->tabOption->currentIndex());
  xml.setStr("tcpHost", ui->leTcpHost->text());
  xml.setStr("tcpPort", ui->leTcpPort->text());
  xml.setStr("udpHost", ui->leUdpHost->text());
  xml.setStr("udpPort", ui->leUdpPort->text());
  xml.setStr("sslHost", ui->leSslHost->text());
  xml.setStr("sslPort", ui->leSslPort->text());
  xml.setStr("sendText", ui->pteSend->toPlainText());

  {
    VXml sizesXml = xml.gotoChild("sizes");
    QList<int> sizes = ui->splitter->sizes();
    QString strList;
    strList.clear(); foreach (int size, sizes) strList += QString::number(size) + ",";
    strList = strList.left(strList.count() - 1);
    sizesXml.setStr("splitter", strList);
  }

  tcpClient.save(xml.gotoChilds("netClient/tcpClient"));
  udpClient.save(xml.gotoChilds("netClient/udpClient"));
  sslClient.save(xml.gotoChilds("netClient/sslClient"));
}

void Widget::on_pbOpen_clicked()
{
  int currentIndex = ui->tabOption->currentIndex();
  switch (currentIndex)
  {
    case 0:
      tcpClient.host = ui->leTcpHost->text();
      tcpClient.port = ui->leTcpPort->text().toInt();
      netClient = &tcpClient;
      break;
    case 1:
      udpClient.host = ui->leUdpHost->text();
      udpClient.port = ui->leUdpPort->text().toInt();
      netClient = &udpClient;
      break;
    case 2:
      sslClient.host = ui->leSslHost->text();
      sslClient.port = ui->leSslPort->text().toInt();
      netClient = &sslClient;
      break;
  }

  SAFE_DELETE(clientThread);
  clientThread = new ClientThread(this, netClient);
  clientThread->open();

  setControl();
}

void Widget::on_pbClose_clicked()
{
  LOG_ASSERT(netClient != NULL);
  netClient->close();
  SAFE_DELETE(clientThread);
  setControl();
}

void Widget::on_pbClear_clicked()
{
  ui->pteRecv->clear();
}

void Widget::on_tbTcpAdvance_clicked()
{
  if (tcpClient.optionDoAll(this))
  {
    ui->leTcpHost->setText(tcpClient.host);
    ui->leTcpPort->setText(QString::number(tcpClient.port));
  }
}

void Widget::on_tbUdpAdvence_clicked()
{
  if (udpClient.optionDoAll(this))
  {
    ui->leUdpHost->setText(udpClient.host);
    ui->leUdpPort->setText(QString::number(udpClient.port));
  }
}

void Widget::on_tbSslAdvanced_clicked()
{
  if (sslClient.optionDoAll(this))
  {
    ui->leSslHost->setText(sslClient.host);
    ui->leSslPort->setText(QString::number(sslClient.port));
  }
}

void Widget::on_pbSend_clicked()
{
  if (netClient == NULL) return;
  QByteArray ba = qPrintable(ui->pteSend->toPlainText());
  ba = ba.replace("\n", "\r\n");
  if (ui->chkSendHexa->checkState() == Qt::Checked) ba = ba.fromHex(ba);
  netClient->write(ba);
}
