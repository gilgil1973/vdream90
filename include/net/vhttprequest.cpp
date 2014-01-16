#include <VHttpRequest>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VHttpRequestLine
// ----------------------------------------------------------------------------
VHttpRequestLine::VHttpRequestLine()
{
  clear();
}

void VHttpRequestLine::clear()
{
  method  = "";
  path    = "";
  version = "";
}

bool VHttpRequestLine::parse(QByteArray data)
{
  QList<QByteArray> bal = data.split(' ');
  if (bal.size() != 3)
  {
    LOG_ERROR("bal.size is %d", bal.size());
    return false;
  }
  method  = bal.at(0);
  path    = bal.at(1);
  version = bal.at(2);
  return true;
}

QByteArray VHttpRequestLine::toByteArray()
{
  return method + " " + path + " " + version + "\r\n";
}

// ----------------------------------------------------------------------------
// VHttpRequest
// ----------------------------------------------------------------------------
VHttpRequest::VHttpRequest()
{
  clear();
}

void VHttpRequest::clear()
{
  requestLine.clear();
  header.clear();
  body.clear();
}

bool VHttpRequest::parse(QByteArray data)
{
  if (!data.startsWith("GET ") && !data.startsWith("POST ")) return false;

  int pos = data.indexOf("\r\n\r\n");
  if (pos == -1) return false;

  QByteArray baHeader = data.left(pos + 2);
  body = data.mid(pos + 4);

  pos = baHeader.indexOf("\r\n");
  QByteArray baRequestLine = baHeader.left(pos);
  baHeader.remove(0, pos + 2);

  if (!requestLine.parse(baRequestLine)) return false;
  if (!header.parse(baHeader)) return false;

  return true;
}

QByteArray VHttpRequest::toByteArray()
{
  return requestLine.toByteArray() + header.toByteArray() + "\r\n" + body;
}

bool VHttpRequest::findHost(QString& host, int& port)
{
  QByteArray hostValue = header.value("Host");
  if (hostValue == "") return false;
  int pos = hostValue.indexOf(':');
  if (pos == -1)
  {
    host = hostValue;
    port = 80;
  } else
  {
    host = hostValue.left(pos + 1);
    port = hostValue.right(hostValue.length() - pos - 1).toInt();
  }
  return true;
}
