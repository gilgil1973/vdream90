#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qfile.h>
#include <qtextstream.h>
#include <VApp>
#include <VLog>
#include <Shellapi.h> // for ShellExecuteA
#include <VDebugNew>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  initializeControl();
}

MainWindow::~MainWindow()
{
  saveControl();
  finalizeControl();
  delete ui;
}

void MainWindow::initializeControl()
{
  optionDlg = NULL;
  aboutDlg  = NULL;

  move(0, 0); resize(640, 480);
  setCentralWidget(ui->plainTextEdit);

  myLog = new MyLog;
  bool res = connect(&myLog->sync, SIGNAL(onMessage(QString)), this, SLOT(_onMessage(QString)));
  if (!res)
  {
    LOG_ERROR("connect return false");
  }
}

void MainWindow::finalizeControl()
{
  SAFE_DELETE(optionDlg);
  SAFE_DELETE(aboutDlg);
  if (myLog != NULL)
  {
    myLog->close();
    setControl();
    SAFE_DELETE(myLog);
  }
}

void MainWindow::loadControl()
{
  this->loadFromDefaultDoc("MainWindow");
  myLog->loadFromDefaultDoc("myLog");
}

void MainWindow::saveControl()
{
  this->saveToDefaultDoc("MainWindow");
  myLog->saveToDefaultDoc("myLog");
}

void MainWindow::setControl()
{
  bool active = myLog->active();
  ui->actionOpen->setEnabled(!active);
  ui->actionClose->setEnabled(active);
  ui->actionOption->setEnabled(!active);

  Qt::WindowFlags flags = this->windowFlags();
  if (ui->actionAlwaysOnTop->isChecked())
  {
    if ((flags & Qt::WindowStaysOnTopHint) == 0)
    {
      flags |= Qt::WindowStaysOnTopHint;
      this->setWindowFlags(flags);
      this->show();
    }
  }
  else
  {
    if ((flags & Qt::WindowStaysOnTopHint) != 0)
    {
      flags &= ~Qt::WindowStaysOnTopHint;
      this->setWindowFlags(flags);
      this->show();
    }
  }

  if (ui->actionWordWrap->isChecked())
  {
    if (ui->plainTextEdit->lineWrapMode() == QPlainTextEdit::NoWrap)
    {
      ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
      this->show();
    }
  } else
  {
    if (ui->plainTextEdit->lineWrapMode() == QPlainTextEdit::WidgetWidth)
    {
      ui->plainTextEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
      this->show();
    }
  }
}

void MainWindow::showEvent(QShowEvent* showEvent)
{
  if (isVisible()) return;
  loadControl();
  setControl();
  if (myLog->autoOpen) on_actionOpen_triggered();
  QMainWindow::showEvent(showEvent);
}

void MainWindow::load(VXml xml)
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

  ui->actionShowLog->setChecked(xml.getBool("showLog", ui->actionShowLog->isChecked()));
  ui->actionAlwaysOnTop->setChecked(xml.getBool("alwaysOnTop", ui->actionAlwaysOnTop->isChecked()));
  ui->actionWordWrap->setChecked(xml.getBool("wordWrap", ui->actionWordWrap->isChecked()));
}

void MainWindow::save(VXml xml)
{
  {
    VXml coordXml = xml.gotoChild("coord");
    QRect rect = geometry();
    coordXml.setInt("left",   rect.left());
    coordXml.setInt("top",    rect.top());
    coordXml.setInt("width",  rect.width());
    coordXml.setInt("height", rect.height());
  }

  xml.setBool("showLog", ui->actionShowLog->isChecked());
  xml.setBool("alwaysOnTop", ui->actionAlwaysOnTop->isChecked());
  xml.setBool("wordWrap", ui->actionWordWrap->isChecked());
}

#include <qmessagebox.h>
void MainWindow::_onMessage(QString qs)
{
  if (!ui->actionShowLog->isChecked()) return;
  if (myLog->timeShow)
  {
    static const int BUF_SIZE = 256;
    char timeBuf[BUF_SIZE];
    QDateTime now = QDateTime::currentDateTime();
    int i = sprintf_s(timeBuf, BUF_SIZE - 1, "%02d:%02d:%02d %03d : ",
      now.time().hour(), now.time().minute(), now.time().second(), now.time().msec());
    timeBuf[i] = '\0';
    QString ts = timeBuf;
    qs = ts + qs;
  }
  ui->plainTextEdit->appendPlainText(qs);
}

void MainWindow::on_actionOpen_triggered()
{
  if (!myLog->open())
  {
    QMessageBox msgBox;
    msgBox.setText(QString(QString::fromLocal8Bit(myLog->error.msg)));
    msgBox.exec();
  }
  setControl();
}

void MainWindow::on_actionClose_triggered()
{
  myLog->close();
  setControl();
}

void MainWindow::on_actionEdit_triggered()
{
  QString qs = ui->plainTextEdit->toPlainText();
  QDateTime now = QDateTime::currentDateTime();
  QString completeBaseName = qformat("%04d%02d%02d%02d%02d%02d",
    now.date().year(), now.date().month(), now.time().hour(), now.time().minute(), now.time().second());
  QString fileName;
  int index = 0;
  while (true)
  {
    fileName = VApp::currentPath() + QDir::separator() + completeBaseName;
    if (index > 0) 
      fileName += "_" + QString::number(index) + ".log";
    else
      fileName += ".log";
    if (!QFile::exists(fileName)) break;
  }
  {
    QFile f(fileName);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&f);
    out << qs;
  }
#ifdef WIN32
  ShellExecuteA(0, "open", qPrintable(fileName), NULL, NULL, SW_SHOW);
#endif // WIN32
#ifdef linux
  {
    QString cmd = "gedit " + fileName + " &";
    int res = system(qPrintable(cmd));
    if (res < 0)
    {
      LOG_ERROR("system(%s) return %d errno=%d", qPrintable(cmd), res, errno);
    }
  }
#endif // linux
}

void MainWindow::on_actionClear_triggered()
{
  ui->plainTextEdit->clear();
}

void MainWindow::on_actionExit_triggered()
{
  close();
}

void MainWindow::on_actionOption_triggered()
{
  if (optionDlg == NULL)
  {
    optionDlg = new OptionDlg;
    optionDlg->myLog = myLog;
  }
  optionDlg->exec();
}

void MainWindow::on_actionAbout_triggered()
{
  if (aboutDlg == NULL)
  {
    aboutDlg = new AboutDlg;
  }
  aboutDlg->exec();
}

void MainWindow::on_actionAlwaysOnTop_triggered()
{
  setControl();
}

void MainWindow::on_actionWordWrap_triggered()
{
  setControl();
}
