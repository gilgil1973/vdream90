#include <QApplication>
#include "mainwindow.h"
#include <VApp>

int run(int argc, char* argv[])
{
  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}

int main(int argc, char *argv[])
{
  VApp::initialize(true, true, "");
  int res = run(argc, argv);
  VApp::finalize();
  return res;
}
