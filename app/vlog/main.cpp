#include <iostream>
#include <string>

//#include <QFile> // gilgil temp 2013.02.19

#include <VApp>
//#include <VFile> // gilgil temp 2013.02.19

#include <VLogUdp>
#include <VLogStdout>
#include <VLogFile>
#include <VLogList>

using namespace std;

void changeLog()
{
  VLogUdp* logUDP = new VLogUdp;
  logUDP->showThreadID = false;
  logUDP->showDateTime = VShowDateTime::None;

  VLogStdout* logStdout = new VLogStdout;
  logStdout->showThreadID = false;
  logStdout->showDateTime = VShowDateTime::None;

  VLogFile* logFile = new VLogFile("log/");
  logFile->showThreadID = false;
  logFile->showDateTime = VShowDateTime::DateTime;

  VLogList* logList = new VLogList;
  logList->showThreadID = false;
  logList->showDateTime = VShowDateTime::None;

  logList->items.append(logUDP);
  logList->items.append(logStdout);
  logList->items.append(logFile);

  logList->saveToDefaultDoc("log");
  VLog::changeLog(logList);
}

int main()
{
  bool fileExist = QFile::exists(VXmlDoc::defaultFileName());
  VApp::initialize();
  if (!fileExist) changeLog();

  string s;
  while (true)
  {
    getline(cin, s);
    if (cin.bad())
    {
      LOG_DEBUG("cin.bad");
      break;
    }
    VLog::getLog()->trace("%s", s.c_str());
    if (cin.eof())
    {
      LOG_DEBUG("cin.eof");
      break;
    }
  }
  VApp::finalize();
  return 0;
}
