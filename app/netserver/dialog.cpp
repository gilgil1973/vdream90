#include "dialog.h"
#include "ui_dialog.h"

// ----------------------------------------------------------------------------
// UdpServerThread
// ----------------------------------------------------------------------------
UdpServerThread::UdpServerThread(void* owner, VUdpServer* udpServer)
{
  this->dialog    = (Dialog*)owner;
  this->udpServer = udpServer;
}

UdpServerThread::~UdpServerThread()
{
  close();
}

void UdpServerThread::fireEvent(QEvent* event)
{
  QApplication::postEvent(dialog, event);
}

void UdpServerThread::run()
{
  LOG_ASSERT(udpServer != NULL);
  VSockAddrList& sockAddrList = udpServer->sockAddrList; // for abbreviate
  VUdpSession* udpSession = udpServer->udpSession; // for abbreviate

  // fireEvent(new MsgEvent("******** connected ********")); // gilgil temp 2014.03.01
  fireEvent(new StateEvent(VState::Opened));
  while (active())
  {
    QByteArray ba;
    int readLen = udpServer->read(ba);

    //
    // Delete session
    //
    if (readLen == VERR_FAIL)
    {
      QString msg = udpSession->error.msg;
      fireEvent(new MsgEvent(msg));

      if (udpSession->error.code == 10054/*WSAECONNRESET*/) // ICMP Destination Unreachable // gilgil temp 2014.04.18
      {
        while (true)
        {
          VSockAddrList::iterator it = sockAddrList.findByInAddr(udpSession->addr.sin_addr);
          if (it == sockAddrList.end()) break;
          sockAddrList.erase(it);
        }
        LOG_DEBUG("count=%d", sockAddrList.size()); // gilgi temp 2009.08.16
      } else // other error
      {
        VSockAddrList::iterator it = sockAddrList.findBySockAddr(udpSession->addr);
        if (it != sockAddrList.end())
          sockAddrList.erase(it);
        LOG_DEBUG("count=%d", sockAddrList.size()); // gilgi temp 2009.08.16
      }
      udpSession->error.clear();
      continue;
    }

    //
    // Add session
    //
    if (sockAddrList.findBySockAddr(udpSession->addr) == sockAddrList.end())
    {
      SOCKADDR_IN* newSockAddr = new SOCKADDR_IN;
      *newSockAddr = udpSession->addr;
      sockAddrList.insert(*newSockAddr);
      LOG_DEBUG("count=%d", sockAddrList.size()); // gilgi temp 2009.08.16
    }

    if (dialog->ui->chkShowHexa->checkState() == Qt::Checked)
      ba = ba.toHex();
    QString msg = ba;
    fireEvent(new MsgEvent(msg));

    if (dialog->ui->chkEcho->checkState() == Qt::Checked)
    {
      if (dialog->ui->chkEchoBroadcast->checkState() == Qt::Checked)
      {
        udpServer->write(ba); // do not check result
      } else
      {
        int writeLen = udpSession->write(ba);
        if (writeLen == VERR_FAIL) break;
      }
    }
  }
  // fireEvent(new MsgEvent("******** disconnected ********")); // gilgil temp 2014.03.01
  fireEvent(new CloseEvent);
}

// ----------------------------------------------------------------------------
// Dialog
// ----------------------------------------------------------------------------
Dialog::Dialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialog)
{
  ui->setupUi(this);
  initializeControl();
}

Dialog::~Dialog()
{
  saveControl();
  finalizeControl();
  delete ui;
}

void Dialog::showEvent(QShowEvent* showEvent)
{
  loadControl();
  setControl();
  QDialog::showEvent(showEvent);
}

void Dialog::initializeControl()
{
  netServer       = NULL;
  udpServerThread = NULL;

  move(0, 0); resize(640, 480);

  setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);
  setLayout(ui->mainLayout);
  ui->mainLayout->setSpacing(0);
  ui->pteRecv->setWordWrapMode(QTextOption::NoWrap);
  ui->pteSend->setWordWrapMode(QTextOption::NoWrap);

  QObject::connect(&tcpServer, SIGNAL(runned(VTcpSession*)), this, SLOT(tcpRun(VTcpSession*)), Qt::DirectConnection);
  QObject::connect(&sslServer, SIGNAL(runned(VSslSession*)), this, SLOT(sslRun(VSslSession*)), Qt::DirectConnection);
}

void Dialog::finalizeControl()
{
  on_pbClose_clicked();
}

void Dialog::loadControl()
{
  this->loadFromDefaultDoc("MainWindow");
}

void Dialog::saveControl()
{
  this->saveToDefaultDoc("MainWindow");
}

void Dialog::setControl(VState state)
{
  LOG_DEBUG(""); // gilgil temp 2014.02.28
  if (state == VState::None)
  {
    int currentIndex = ui->tabOption->currentIndex();
    switch (currentIndex)
    {
      case 0: netServer = &tcpServer; break;
      case 1: netServer = &udpServer; break;
      case 2: netServer = &sslServer; break;
    }
    state = netServer->state();
  }

  ui->pbOpen->setEnabled(state == VState::Closed);
  ui->pbClose->setEnabled(state != VState::Closed);
  ui->tabOption->setEnabled(state == VState::Closed);
  ui->pbSend->setEnabled(state == VState::Opened);
}

void Dialog::tcpRun(VTcpSession* tcpSession)
{
  myRun(tcpSession);
}

void Dialog::sslRun(VSslSession* sslSession)
{
  myRun(sslSession);
}

void Dialog::fireEvent(QEvent* event)
{
  QApplication::postEvent(this, event);
}

void Dialog::myRun(VNetSession* netSession)
{
  LOG_ASSERT(netSession != NULL);

  fireEvent(new MsgEvent("******** connected ********"));

  while (true)
  {
    QByteArray ba;

    int readLen = netSession->read(ba);
    if (readLen == VERR_FAIL) break;

    if (ui->chkShowHexa->checkState() == Qt::Checked)
      ba = ba.toHex();
    QString msg = ba;
    fireEvent(new MsgEvent(msg));

    if (ui->chkEcho->checkState() == Qt::Checked)
    {
      if (ui->chkEchoBroadcast->checkState() == Qt::Checked)
      {
        netServer->write(ba); // do not check result
      } else
      {
        int writeLen = netSession->write(ba);
        if (writeLen == VERR_FAIL) break;
      }
    }
  }

  fireEvent(new MsgEvent("******** disconnected ********")); // gilgil temp 2014.03.01
  // fireEvent(new CloseEvent); // gilgil temp 2014.03.01
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
  ui->chkEcho->setCheckState((Qt::CheckState)xml.getInt("echo", (int)ui->chkEcho->checkState()));
  ui->chkEchoBroadcast->setCheckState((Qt::CheckState)xml.getInt("echoBroadcast", (int)ui->chkEchoBroadcast->checkState()));
  ui->tabOption->setCurrentIndex(xml.getInt("currentIndex", 0));
  ui->leTcpPort->setText(xml.getStr("tcpPort", ui->leTcpPort->text()));
  ui->leUdpPort->setText(xml.getStr("udpPort", ui->leUdpPort->text()));
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

  tcpServer.load(xml.gotoChilds("netClient/tcpServer"));
  udpServer.load(xml.gotoChilds("netClient/udpServer"));
  sslServer.load(xml.gotoChilds("netClient/sslServer"));
}

void Dialog::save(VXml xml)
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
  xml.setInt("echo", (int)ui->chkEcho->checkState());
  xml.setInt("echoBroadcast", (int)ui->chkEchoBroadcast->checkState());
  xml.setInt("currentIndex", ui->tabOption->currentIndex());
  xml.setStr("tcpPort", ui->leTcpPort->text());
  xml.setStr("udpPort", ui->leUdpPort->text());
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

  tcpServer.save(xml.gotoChilds("netClient/tcpServer"));
  udpServer.save(xml.gotoChilds("netClient/udpServer"));
  sslServer.save(xml.gotoChilds("netClient/sslServer"));
}

void Dialog::on_pbOpen_clicked()
{
  int currentIndex = ui->tabOption->currentIndex();
  switch (currentIndex)
  {
    case 0:
      tcpServer.port = ui->leTcpPort->text().toInt();
      netServer = &tcpServer;
      break;
    case 1:
      udpServer.port = ui->leUdpPort->text().toInt();
      netServer = &udpServer;
      break;
    case 2:
      sslServer.port = ui->leSslPort->text().toInt();
      netServer = &sslServer;
      break;
  }

  bool res = netServer->open();
  if (!res)
  {
    QString msg = netServer->error.msg;
    ui->pteRecv->insertPlainText(msg + "\n");
    ui->pteRecv->ensureCursorVisible();
    return;
  }
  if (netServer == &udpServer)
  {
    SAFE_DELETE(udpServerThread);
    udpServerThread = new UdpServerThread(this, &udpServer);
    udpServerThread->open();
  }

  setControl();
}

void Dialog::on_pbClose_clicked()
{
  LOG_ASSERT(netServer != NULL);
  netServer->close();
  SAFE_DELETE(udpServerThread);
  setControl();
}

void Dialog::on_pbClear_clicked()
{
  ui->pteRecv->clear();
}

void Dialog::on_tbTcpAdvance_clicked()
{
  if (tcpServer.optionDoAll())
  {
    ui->leTcpPort->setText(QString::number(tcpServer.port));
  }
}

void Dialog::on_tbUdpAdvence_clicked()
{
  if (udpServer.optionDoAll())
  {
    ui->leTcpPort->setText(QString::number(udpServer.port));
  }
}

void Dialog::on_tbSslAdvanced_clicked()
{
  if (sslServer.optionDoAll())
  {
    ui->leTcpPort->setText(QString::number(sslServer.port));
  }
}

void Dialog::on_pbSend_clicked()
{
  if (netServer == NULL) return;
  QByteArray ba = qPrintable(ui->pteSend->toPlainText());
  if (ui->chkSendHexa->checkState() == Qt::Checked) ba = ba.fromHex(ba);
  netServer->write(ba);
}
