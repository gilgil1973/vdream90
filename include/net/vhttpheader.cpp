#include <VHttpHeader>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VHttpHeader
// ----------------------------------------------------------------------------
VHttpHeader::VHttpHeader()
{
  clear();
}

QByteArray VHttpHeader::value(QByteArray key)
{
  for (Items::iterator it = items.begin(); it != items.end(); it++)
  {
    // LOG_DEBUG("it = %s, %s", it->first.data(), it->second.data()); // gilgil temp
    if (it->first == key)
      return it->second;
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

void VHttpHeader::clear()
{
  items.clear();
}

bool VHttpHeader::parse(QByteArray data)
{
  clear();
  while (true)
  {
    QByteArray oneData;
    int pos = data.indexOf("\r\n");
    if (pos == -1)
    {
      oneData = data;
      data = "";
    } else
    {
      oneData = data.left(pos);
      data.remove(0, pos + 2);
    }
    // LOG_DEBUG("oneData=%s", oneData.data()); // gilgil temp 2013.10.20

    pos = oneData.indexOf(':');
    if (pos == -1)
    {
      LOG_ERROR("oneData.indexOf(';') return -1 (%s)", oneData.data());
      return false;
    }
    QByteArray key   = oneData.left(pos);
    QByteArray value = oneData.mid(pos + 2).trimmed();
    // LOG_DEBUG("setValue(%s, %s)", key.data(), value.data()); // gilgil temp 2013.10.20
    setValue(key, value);

    if (data == "") break;
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
