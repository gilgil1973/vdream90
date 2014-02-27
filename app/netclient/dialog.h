#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

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
  QString msg;

public:
  MsgEvent(QString msg) : QEvent(QEvent::None) { this->msg = msg; }
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
class ClientThread : public VThread
{
protected:
  QDialog*    dialog;
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
// Dialog
// ----------------------------------------------------------------------------
namespace Ui {
  class Dialog;
}

class Dialog : public QDialog, public VXmlable
{
  Q_OBJECT

public:
  explicit Dialog(QWidget *parent = 0);
  ~Dialog();

public:
  VTcpClient  tcpClient;
  VUdpClient  udpClient;
  VSslClient  sslClient;
  VNetClient* netClient;

  ClientThread* clientThread;

public:
  void setControl(VState state = VState::None);

public:
  virtual bool event(QEvent* event);

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
  Ui::Dialog *ui;
};

#endif // DIALOG_H
