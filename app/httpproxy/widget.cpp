#include "widget.h"
#include "ui_widget.h"

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
  showMsg = true;
  move(0, 0); resize(640, 480);
  VObject::connect(
    &proxy, SIGNAL(onHttpRequestHeader(VHttpRequest*,VNetSession*,VNetClient*)),
    this, SLOT(httpRequestHeader(VHttpRequest*,VNetSession*,VNetClient*)),
    Qt::DirectConnection);
  VObject::connect(
    &proxy, SIGNAL(onHttpRequestBody(VHttpRequest*,QByteArray*,VNetSession*,VNetClient*)),
    this, SLOT(httpRequestBody(VHttpRequest*,QByteArray*,VNetSession*,VNetClient*)),
    Qt::DirectConnection);
  VObject::connect(
    &proxy, SIGNAL(onHttpResponseHeader(VHttpResponse*,VNetClient*,VNetSession*)),
    this, SLOT(httpResponseHeader(VHttpResponse*,VNetClient*,VNetSession*)),
    Qt::DirectConnection);
  VObject::connect(&proxy, SIGNAL(onHttpResponseBody(VHttpResponse*,QByteArray*,VNetClient*,VNetSession*)),
    this, SLOT(httpResponseBody(VHttpResponse*,QByteArray*,VNetClient*,VNetSession*)),
    Qt::DirectConnection);
}

void Widget::finalizeControl()
{
  ui->pbClose->click();
}

void Widget::loadControl()
{
  this->loadFromDefaultDoc("MainWindow");
  setControl();
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
    state = proxy.state();
  }

  ui->pbOpen->setEnabled(state == VState::Closed);
  ui->pbClose->setEnabled(state != VState::Closed);
  ui->pbOption->setEnabled(state == VState::Closed);
}

void Widget::showEvent(QShowEvent* showEvent)
{
  loadControl();
  QWidget::showEvent(showEvent);
}

bool Widget::event(QEvent* event)
{
  MsgEvent* msgEvent = dynamic_cast<MsgEvent*>(event);
  if (msgEvent != NULL)
  {
    showMessage(msgEvent->msg);
    return true;
  }
  return QWidget::event(event);
}

void Widget::showMessage(QString msg)
{
  ui->pteMsg->insertPlainText(msg);
  ui->pteMsg->ensureCursorVisible();
}

void Widget::httpRequestHeader(VHttpRequest* header, VNetSession* inSession, VNetClient*  outClient)
{
  Q_UNUSED(inSession)
  Q_UNUSED(outClient)

  if (!showMsg) return;
  QApplication::postEvent(this, new MsgEvent(header->toByteArray()));
}

void Widget::httpRequestBody(VHttpRequest* header, QByteArray* body, VNetSession* inSession, VNetClient*  outClient)
{
  Q_UNUSED(header)
  Q_UNUSED(outClient)
  Q_UNUSED(inSession)

  if (!showMsg) return;
  QApplication::postEvent(this, new MsgEvent(*body));
}

void Widget::httpResponseHeader(VHttpResponse* header, VNetClient*  outClient, VNetSession* inSession)
{
  Q_UNUSED(inSession)
  Q_UNUSED(outClient)

  if (!showMsg) return;
  QApplication::postEvent(this, new MsgEvent(header->toByteArray()));
}

void Widget::httpResponseBody(VHttpResponse* header, QByteArray* body, VNetClient* outClient, VNetSession* inSession)
{
  Q_UNUSED(header)
  Q_UNUSED(outClient)
  Q_UNUSED(inSession)

  if (!showMsg) return;
  QApplication::postEvent(this, new MsgEvent(*body));
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

  showMsg = xml.getBool("showMsg", showMsg);
  ui->chkShowMsg->setCheckState(showMsg ? Qt::Checked : Qt::Unchecked);

  proxy.load(xml.gotoChild("HttpProxy"));
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

  xml.setBool("showMsg", showMsg);

  proxy.save(xml.gotoChild("HttpProxy"));
}

void Widget::on_pbOpen_clicked()
{
  if (!proxy.open())
  {
    QString msg = proxy.error.msg;
    showMessage(msg);
  }
  setControl();
}

void Widget::on_pbClose_clicked()
{
  proxy.close();
  setControl();
}

void Widget::on_pbClear_clicked()
{
  ui->pteMsg->clear();
}

void Widget::on_pbOption_clicked()
{
  proxy.optionDoAll(this);
}

void Widget::on_chkShowMsg_clicked()
{
  showMsg = ui->chkShowMsg->checkState() == Qt::Checked;
}
