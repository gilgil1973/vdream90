#include "dialog.h"
#include "ui_dialog.h"
#include <VDebugNew>

// ----------------------------------------------------------------------------
// ClientThread
// ----------------------------------------------------------------------------
ClientThread::ClientThread(void* owner, VNetClient* netClient) : VThread(owner)
{
  this->dialog = (QDialog*)owner;
  this->netClient = netClient;
}

ClientThread::~ClientThread()
{
  close();
}

void ClientThread::fireEvent(QEvent* event)
{
  QApplication::postEvent(dialog, event);
}

void ClientThread::run()
{
  LOG_ASSERT(netClient != NULL);
  fireEvent(new StateEvent(VState::Opening));
  bool res = netClient->open();
  if (!res)
  {
    fireEvent(new MsgEvent(netClient->error.msg));
    fireEvent(new CloseEvent);
    return;
  }

  fireEvent(new MsgEvent("******** connected ********"));
  fireEvent(new StateEvent(VState::Opened));
  while (true)
  {
    QByteArray ba;
    int readLen = netClient->read(ba);
    if (readLen == VERR_FAIL) break;
    QString msg = ba;
    fireEvent(new MsgEvent(msg));
  }
  fireEvent(new MsgEvent("******** disconnected ********"));
  fireEvent(new CloseEvent);
}

// ----------------------------------------------------------------------------
// Dialog
// ----------------------------------------------------------------------------
Dialog::Dialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialog)
{
  netClient = NULL;
  clientThread = NULL;

  ui->setupUi(this);

  move(0, 0);
  resize(640, 480);
  setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
  setLayout(ui->mainLayout);
  ui->mainLayout->setSpacing(0);
  ui->pteRecv->setWordWrapMode(QTextOption::NoWrap);
  ui->pteSend->setWordWrapMode(QTextOption::NoWrap);

  loadFromDefaultDoc("MainWindow");
  setControl();
}

Dialog::~Dialog()
{
  on_pbClose_clicked();
  this->saveToDefaultDoc("MainWindow");
  delete ui;
}

void Dialog::setControl(VState state)
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

bool Dialog::event(QEvent* event)
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

  return QDialog::event(event);
}

void Dialog::load(VXml xml)
{
  QRect rect = geometry();
  rect.setLeft  ((xml.getInt("left",   0)));
  rect.setTop   ((xml.getInt("top",    0)));
  rect.setWidth ((xml.getInt("width",  640)));
  rect.setHeight((xml.getInt("height", 480)));
  setGeometry(rect);

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

  QList<int> sizes;
  for (int i = 0; i < ui->splitter->count(); i++)
    sizes << xml.gotoChild("spilitter").getInt(qformat("height%d", i), 0);
  ui->splitter->setSizes(sizes);

  tcpClient.load(xml.gotoChilds("netClient/tcpClient"));
  udpClient.load(xml.gotoChilds("netClient/udpClient"));
  sslClient.load(xml.gotoChilds("netClient/sslClient"));
}

void Dialog::save(VXml xml)
{
  QRect rect = geometry();
  xml.setInt("left",   rect.left());
  xml.setInt("top",    rect.top());
  xml.setInt("width",  rect.width());
  xml.setInt("height", rect.height());

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

  for (int i = 0; i < ui->splitter->count(); i++)
    xml.gotoChild("spilitter").setInt(qformat("height%d", i), ui->splitter->sizes().at(i));

  tcpClient.save(xml.gotoChilds("netClient/tcpClient"));
  udpClient.save(xml.gotoChilds("netClient/udpClient"));
  sslClient.save(xml.gotoChilds("netClient/sslClient"));
}

void Dialog::on_pbOpen_clicked()
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
      netClient = &udpClient;
      udpClient.host = ui->leUdpHost->text();
      udpClient.port = ui->leUdpPort->text().toInt();
      break;
    case 2:
      netClient = &sslClient;
      sslClient.host = ui->leSslHost->text();
      sslClient.port = ui->leSslPort->text().toInt();
      break;
  }

  SAFE_DELETE(clientThread);
  clientThread = new ClientThread(this, netClient);
  clientThread->open();
}

void Dialog::on_pbClose_clicked()
{
  LOG_ASSERT(netClient != NULL);
  netClient->close();

  SAFE_DELETE(clientThread);

  setControl();
}

void Dialog::on_pbClear_clicked()
{
  ui->pteRecv->clear();
}

void Dialog::on_tbTcpAdvance_clicked()
{
  tcpClient.optionDoAll();
}

void Dialog::on_tbUdpAdvence_clicked()
{
  udpClient.optionDoAll();
}

void Dialog::on_tbSslAdvanced_clicked()
{
  sslClient.optionDoAll();
}

void Dialog::on_pbSend_clicked()
{
  if (netClient == NULL) return;
  QByteArray ba = qPrintable(ui->pteSend->toPlainText());
  netClient->write(ba);
}
