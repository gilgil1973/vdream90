#include <VHttpHeader>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VHttpHeader
// ----------------------------------------------------------------------------
VHttpHeader::VHttpHeader()
{
  clear();
}

QByteArray VHttpHeader::value(QByteArray key, bool caseSensitive)
{
  if (caseSensitive)
  {
    for (Items::iterator it = items.begin(); it != items.end(); it++)
    {
      if (it->first == key)
        return it->second;
    }
  } else // case insensitive
  {
    QByteArray lowerKey = key.toLower();
    for (Items::iterator it = items.begin(); it != items.end(); it++)
    {
      if (it->first.toLower() == lowerKey)
        return it->second;
    }
  }
  return "";
}

void VHttpHeader::setValue(QByteArray key, QByteArray value)
{
  for (Items::iterator it = items.begin(); it != items.end(); it++)
  {
    if (it->first == key)
    {
      it->second = value;
      return;
    }
  }
  Item newItem(key, value);
  items.push_back(newItem);
}

void VHttpHeader::addValue(QByteArray key, QByteArray value)
{
  Item newItem(key, value);
  items.push_back(newItem);
}

void VHttpHeader::clear()
{
  items.clear();
}

bool VHttpHeader::parse(QByteArray& buffer)
{
  clear();
  while (true)
  {
    QByteArray oneLine;
    int pos = buffer.indexOf("\r\n");
    if (pos == -1)
    {
      oneLine = buffer;
      buffer = "";
    } else
    {
      oneLine = buffer.left(pos);
      buffer.remove(0, pos + 2);
    }

    pos = oneLine.indexOf(':');
    if (pos == -1)
    {
      LOG_ERROR("oneLine.indexOf(';') return -1 (%s)", oneLine.data());
      return false;
    }
    QByteArray key   = oneLine.left(pos);
    QByteArray value = oneLine.mid(pos + 2).trimmed();
    // LOG_DEBUG("setValue(%s, %s)", key.data(), value.data()); // gilgil temp 2013.10.20
    addValue(key, value);

    if (buffer == "") break;
  }
  return true;
}

QByteArray VHttpHeader::toByteArray()
{
  QByteArray res;
  for (Items::iterator it = items.begin(); it != items.end(); it++)
  {
    res += it->first + ": " + it->second + "\r\n";
  }
  return res;
}
