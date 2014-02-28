#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "optiondlg.h"
#include "aboutdlg.h"
#include "mylog.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow, public VXmlable
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

protected:
  void showEvent(QShowEvent* showEvent);

public:
  void initializeControl();
  void finalizeControl();
  void loadControl();
  void saveControl();
  void setControl();

public:
  MyLog* myLog;

public:
  OptionDlg* optionDlg;
  AboutDlg*  aboutDlg;

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

private slots:
  void _onMessage(QString qs);

private slots:
  void on_actionOpen_triggered();

  void on_actionClose_triggered();

  void on_actionEdit_triggered();

  void on_actionClear_triggered();

  void on_actionExit_triggered();

  void on_actionOption_triggered();

  void on_actionAbout_triggered();

  void on_actionAlwaysOnTop_triggered();

  void on_actionWordWrap_triggered();

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
