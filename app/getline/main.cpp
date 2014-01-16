#include <QDir>

// ----------------------------------------------------------------------------
// usage
// ----------------------------------------------------------------------------
void usage()
{
  printf("getline version 8.0\n");
  printf("Copyright (c) Gilbert Lee All rights reserved\n");
  printf("\n");
  printf("getline [-f <root folder>] <list of files>\n");
  printf("\n");
  printf("example\n");
  printf("\n");
  printf("  getline *.h *.cpp\n");
  printf("  getline *.h* *.c*\n");
  printf("  getline -f /usr/include *.h\n");
}

// ----------------------------------------------------------------------------
// GetLine
// ---------------------------------------------------------------------------
class GetLine
{
public:
  static int getLineOfFileName(QString fileName);
  static int scan(QString folder, QStringList& nameFilters);
};

int GetLine::getLineOfFileName(QString fileName)
{
  static const int MAX_STR_SIZE = 65536;
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) return 0;

  int res = 0;
  while (true)
  {
    char buf[MAX_STR_SIZE];
    qint64 readLen = file.readLine(buf, MAX_STR_SIZE);
    if (readLen == -1) break;
    res++;
  }
  file.close();
  printf("%s\t%d\n", QFileInfo(fileName).fileName().toLatin1().data(), res);
  return res;
}

int GetLine::scan(QString folder, QStringList& nameFilters)
{
  int res = 0;
  QDir dir(folder);
  bool isFolderWritten = false;
  QFileInfoList fileInfoList;

  //
  // file
  //
  fileInfoList = dir.entryInfoList(nameFilters);
  foreach(QFileInfo fileInfo, fileInfoList)
  {
    if (fileInfo.isDir()) continue;
    if (!isFolderWritten)
    {
      printf("--------------------------------------------------\n");
      printf("%s\n\n", folder.toLatin1().data());
      isFolderWritten = true;
    }
    res += getLineOfFileName(fileInfo.absoluteFilePath());
  }

  //
  // dir
  //
  fileInfoList = dir.entryInfoList(QDir::AllDirs);
  foreach(QFileInfo fileInfo, fileInfoList)
  {
    QString fileName = fileInfo.fileName();
    if (fileName == "." || fileName == "..") continue;
    QString subFolder = folder + fileInfo.fileName() + "/";
    res += scan(subFolder, nameFilters);
  }
  return res;
}

// ----------------------------------------------------------------------------
// Param
// ----------------------------------------------------------------------------
class Param
{
public:
  QString     folder;
  QStringList nameFilters;

public:
  Param() { folder = "./"; nameFilters.clear(); }

public:
  bool parse(int argc, char* argv[])
  {
    int index = 1;
    while (index < argc)
    {
      if (strcmp(argv[index], "-f") == 0)
      {
        index++;
        if (index >= argc)
        {
          printf("invalid option\n");
          return false;
        }
        folder = argv[index];
        if (folder[folder.length() - 1] != '\\' && folder[folder.length() - 1] != '/')
          folder += '/';
      } else
      {
        QString nameFilter = argv[index];
        nameFilters.push_back(nameFilter);
      }
      index++;
    }
    return nameFilters.size() > 0;
  }
};

// ----------------------------------------------------------------------------
// main
// ----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  Param param;
  if (!param.parse(argc, argv))
  {
    usage();
    return 0;
  }

  int res = GetLine::scan(param.folder, param.nameFilters);

  printf("\n");
  printf("==================================================\n");
  printf("Total File Line = %d\n", res);
  printf("==================================================\n");
}
