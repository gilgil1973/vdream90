#include <VHttpBody>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VHttpBody
// ----------------------------------------------------------------------------
VHttpBody::VHttpBody()
{
  clear();
}

void VHttpBody::clear()
{
  items.clear();
}

bool VHttpBody::parse(QByteArray& data)
{
  clear();
  while (true)
  {
    QByteArray _data = data;

    QByteArray oneLine;
    int pos = _data.indexOf("\r\n");
    if (pos == -1) break;

    oneLine = _data.left(pos);
    _data.remove(0, pos + 2); // "\r\n"

    int chunkSize = oneLine.toInt(NULL, 16);
    if (_data.length() < chunkSize + 2) break;

    QByteArray chunkData = _data.left(chunkSize);
    _data.remove(0, chunkSize);
    if (_data.length() < 2 || (_data[0] != '\r' && _data[1] != '\n'))
    {
      LOG_WARN("invalid chunk format");
      break;
    }
    _data.remove(0, 2);

    items.push_back(Item(chunkSize, chunkData));

    LOG_DEBUG("chunkSize=%d(%x)", chunkSize, chunkSize); // gilgil temp 2014.04.09

    data = _data;
  }
  return items.count() > 0;
}

QByteArray VHttpBody::toByteArray()
{
  QByteArray res;
  for (Items::iterator it = items.begin(); it != items.end(); it++)
  {
    res += QString::number(it->first, 16) + "\r\n" + it->second + "\r\n";
  }
  return res;
}
