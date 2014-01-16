#include <VHttpResponse>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VHttpStatusLine
// ----------------------------------------------------------------------------
VHttpStatusLine::VHttpStatusLine()
{
  clear();
}

void VHttpStatusLine::clear()
{
  version = "";
  code    = 0;
  text    = "";
}

bool VHttpStatusLine::parse(QByteArray data)
{
  QList<QByteArray> bal = data.split(' ');
  if (bal.size() < 3)
  {
    LOG_ERROR("bal.size is %d", bal.size());
    return false;
  }
  version = bal.at(0);
  code    = bal.at(1).toInt();

  text = "";
  for (int i = 2; i < bal.size(); i++)
  {
    text += bal.at(i);
    if (i < bal.size() - 1) text += " ";
  }
  return true;
}

QByteArray VHttpStatusLine::toByteArray()
{
  return version + " " + QByteArray::number(code) + " " + text + "\r\n";
}

// ----------------------------------------------------------------------------
// VHttpResponse
// ----------------------------------------------------------------------------
VHttpResponse::VHttpResponse()
{
  clear();
}

void VHttpResponse::clear()
{
  statusLine.clear();
  header.clear();
  body.clear();
}

bool VHttpResponse::parse(QByteArray data)
{
  if (!data.startsWith("HTTP/1")) return false;

  int pos = data.indexOf("\r\n\r\n");
  if (pos == -1) return false;

  QByteArray baHeader = data.left(pos + 2);
  body = data.mid(pos + 4);

  pos = baHeader.indexOf("\r\n");
  QByteArray baStatusLine = baHeader.left(pos);
  baHeader.remove(0, pos + 2);

  if (!statusLine.parse(baStatusLine)) return false;
  if (!header.parse(baHeader)) return false;

  return true;
}

QByteArray VHttpResponse::toByteArray()
{
  return statusLine.toByteArray() + header.toByteArray() + "\r\n" + body;
}
