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
    &proxy, SIGNAL(onHttpRequestHeader(VHttpRequest*,VHttpProxyConnection*)),
    this,       SLOT(httpRequestHeader(VHttpRequest*,VHttpProxyConnection*)),
    Qt::DirectConnection);
  VObject::connect(
    &proxy, SIGNAL(onHttpResponseHeader(VHttpResponse*,VHttpProxyConnection*)),
    this, SLOT(httpResponseHeader(VHttpResponse*,VHttpProxyConnection*)),
    Qt::DirectConnection);
  VObject::connect(
    &proxy, SIGNAL(onHttpRequestBody(QByteArray*,VHttpProxyConnection*)),
    this, SLOT(httpRequestBody(QByteArray*,VHttpProxyConnection*)),
    Qt::DirectConnection);
  VObject::connect(&proxy, SIGNAL(onHttpResponseBody(QByteArray*,VHttpProxyConnection*)),
    this, SLOT(httpResponseBody(QByteArray*,VHttpProxyConnection*)),
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
    showMessage(msgEvent->msg, msgEvent->crlf);
    return true;
  }
  return QWidget::event(event);
}

void Widget::showMessage(QString msg, bool crlf)
{
  if (crlf) msg += "\r\n";
  ui->pteMsg->insertPlainText(msg);
  ui->pteMsg->ensureCursorVisible();
}

void Widget::httpRequestHeader(VHttpRequest* request, VHttpProxyConnection* connection)
{
  Q_UNUSED(connection)

  if (!showMsg) return;
  QApplication::postEvent(this, new MsgEvent(request->toByteArray(), false));
}


void Widget::httpResponseHeader(VHttpResponse* response, VHttpProxyConnection* connection)
{
  Q_UNUSED(connection)

  if (!showMsg) return;
  QApplication::postEvent(this, new MsgEvent(response->toByteArray(), false));
}

void Widget::httpRequestBody(QByteArray* body, VHttpProxyConnection* connection)
{
  Q_UNUSED(connection)

  if (!showMsg) return;
  QApplication::postEvent(this, new MsgEvent(*body, false));
}

void Widget::httpResponseBody(QByteArray* body, VHttpProxyConnection* connection)
{
  Q_UNUSED(connection)

  if (!showMsg) return;
  QApplication::postEvent(this, new MsgEvent(*body, false));
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
    showMessage(msg, true);
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
