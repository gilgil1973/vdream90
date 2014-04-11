#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

#include <VThread>
#include <VTcpClient>
#include <VUdpClient>
#include <VSslClient>

// ----------------------------------------------------------------------------
// StateEvent
// ----------------------------------------------------------------------------
class StateEvent : public QEvent
{
public:
  VState state;

public:
  StateEvent(VState state) : QEvent(QEvent::None) { this->state = state; }
};

// ----------------------------------------------------------------------------
// MsgEvent
// ----------------------------------------------------------------------------
class MsgEvent : public QEvent
{
public:
  QString    msg;
  Qt::HANDLE threadId;
public:
  MsgEvent(QString msg, Qt::HANDLE threadId) : QEvent(None)
  {
    this->msg      = msg;
    this->threadId = threadId;
  }
};

// ----------------------------------------------------------------------------
// CloseEvent
// ----------------------------------------------------------------------------
class CloseEvent: public QEvent
{
public:
  CloseEvent() : QEvent(QEvent::None) {}
};

// ----------------------------------------------------------------------------
// ClientThread
// ----------------------------------------------------------------------------
class Widget;
class ClientThread : public VThread
{
protected:
  Widget*     widget;
  VNetClient* netClient;

public:
  ClientThread(void* owner, VNetClient* netClient);
  virtual ~ClientThread();

protected:
  void fireEvent(QEvent* event);

protected:
  virtual void run();
};

// ----------------------------------------------------------------------------
// Widget
// ----------------------------------------------------------------------------
namespace Ui {
  class Widget;
}

class Widget : public QWidget, public VXmlable
{
  Q_OBJECT

  friend class ClientThread;

public:
  explicit Widget(QWidget *parent = 0);
  ~Widget();

public:
  void initializeControl();
  void finalizeControl();
  void loadControl();
  void saveControl();
  void setControl(VState state = VState::None);

public:
  VTcpClient  tcpClient;
  VUdpClient  udpClient;
  VSslClient  sslClient;
  VNetClient* netClient;

  ClientThread* clientThread;

public:
  virtual bool event(QEvent* event);
  virtual void showEvent(QShowEvent* event);
  void         showMessage(MsgEvent* event);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

private slots:
  void on_pbOpen_clicked();

  void on_pbClose_clicked();

  void on_pbClear_clicked();

  void on_tbTcpAdvance_clicked();

  void on_tbUdpAdvence_clicked();

  void on_tbSslAdvanced_clicked();

  void on_pbSend_clicked();

private:
  Ui::Widget *ui;
};

#endif // WIDGET_H
