#include "param.h"

Param::Param()
{
  // Network
  netType   = ntTCP;
  port      = 0;
  localHost = "";
  localPort = 0;

  bufSize = DEFAULT_BUF_SIZE;
  echo    = true;
  echoBroadcast = false;
  log = "";

  argc = 0;
  argv = NULL;
}

Param::~Param()
{
}

QString Param::get(int idx)
{
  if (idx >= 0 && idx < argc) return (QString)argv[idx];
  return "";
}

bool Param::parse(int argc, char* argv[])
{
  this->argc = argc;
  this->argv = argv;

  int i = 1;
  QString s, para;

  //
  // tcp or udp
  //
  if (get(i) == "tcp") this->netType = ntTCP, i++;
  if (get(i) == "udp") this->netType = ntUDP, i++;

  //
  // port
  //
  s = get(i);
  if (s == "")
  {
    printf("port not specified\n");
    return false;
  }
  this->port = s.toInt(); i++;

  //
  // option
  //
  while (i < argc)
  {
    s = get(i);
    if (s == "-lh")
    {
      para = get(i + 1); if (para == "")
      {
        printf("local host not specified\n");
        return false;
      }
      this->localHost = para;
      i += 2; continue;
    }
    if (s == "-lp")
    {
      para = get(i + 1); if (para == "")
      {
        printf("local port not specified\n");
        return false;
      }
      this->localPort = para.toInt();
      i += 2; continue;
    }
    if (s == "-bs")
    {
      para = get(i + 1); if (para == "")
      {
        printf("buffer size not specified\n");
        return false;
      }
      this->bufSize = para.toInt();
      i += 2; continue;
    }
    if (s == "-ne")
    {
      echo = false;
      i++; continue;
    }
    if (s == "-eb")
    {
      echoBroadcast = true;
      i++; continue;
    }
    if (s == "-log")
    {
      para = get(i + 1); if (para == "")
      {
        printf("log not specified\n");
        return false;
      }
      this->log = para;
      i += 2; continue;
    }

    printf("syntax error(%s)\n", qPrintable(s));
    return false;
  }
  return true;
}
