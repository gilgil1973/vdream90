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
  LOG_INFO("webproxy started %s", VDREAM_VERSION);
  int res = run(argc, argv);
  VApp::finalize();
  LOG_INFO("webproxy terminated");
  return res;
}
