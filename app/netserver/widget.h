#ifndef DIALOG_H
#define DIALOG_H

#include <QWidget>

#include <VThread>
#include <VTcpServer>
#include <VUdpServer>
#include <VSslServer>

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
// UdpServerThread
// ----------------------------------------------------------------------------
class Dialog;
class UdpServerThread : public VThread
{
protected:
  Dialog*     dialog;
  VUdpServer* udpServer;

public:
  UdpServerThread(void* owner, VUdpServer* udpServer);
  virtual ~UdpServerThread();

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

  friend class UdpServerThread; // gilgil temp 2012.02.28

public:
  explicit Dialog(QWidget *parent = 0);
  ~Dialog();

public:
  void initializeControl();
  void finalizeControl();
  void loadControl();
  void saveControl();
  void setControl(VState state = VState::None);

public:
  VTcpServer  tcpServer;
  VUdpServer  udpServer;
  VSslServer  sslServer;
  VNetServer* netServer;

  UdpServerThread* udpServerThread;

protected slots:
  void tcpRun(VTcpSession* tcpSession);
  void sslRun(VSslSession* sslSession);

protected:
  void fireEvent(QEvent* event);
  void myRun(VNetSession* netSession);

protected:
  virtual bool event(QEvent* event);
  void showEvent(QShowEvent* showEvent);

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
