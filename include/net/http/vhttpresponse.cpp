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

bool VHttpStatusLine::parse(QByteArray& buffer)
{
  QList<QByteArray> bal = buffer.split(' ');
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
}

bool VHttpResponse::parse(QByteArray& buffer)
{
  if (!buffer.startsWith("HTTP/1")) return false;

  int pos = buffer.indexOf("\r\n\r\n");
  if (pos == -1) return false;

  QByteArray baHeader = buffer.left(pos + 2);

  int firstLinePos = baHeader.indexOf("\r\n");
  QByteArray baStatusLine = baHeader.left(firstLinePos);
  baHeader.remove(0, firstLinePos + 2);

  if (!statusLine.parse(baStatusLine)) return false;
  if (!header.parse(baHeader)) return false;

  buffer = buffer.mid(pos + 4);

  return true;
}

QByteArray VHttpResponse::toByteArray()
{
  return statusLine.toByteArray() + header.toByteArray() + "\r\n";
}
