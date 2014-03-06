#include "widget.h"
#include <QApplication>

#include <VApp>

int run(int argc, char* argv[])
{
  QApplication a(argc, argv);
  Widget w;
  w.show();
  return a.exec();
}

int main(int argc, char* argv[])
{
  VApp::initialize();
  int res = run(argc, argv);
  VApp::finalize();
  return res;
}
