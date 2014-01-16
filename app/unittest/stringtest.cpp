#include <QDebug>
#include "main.h"
#include <VString>
#include <VDebugNew>

TEST( String, ss_to_vs )
{
  std::string ss = "std::string";
  QString     qs = ss.c_str();
  qDebug() << qs;
}

TEST( String, vs_to_ss )
{
  QString qs = "test";
  std::string ss = qs.toLatin1().data();
  printf("%s\n", ss.c_str());
}
