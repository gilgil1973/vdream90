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
    QByteArray oneLine;
    int pos = data.indexOf("\r\n");
    if (pos == -1) return false;
    oneLine = data.left(pos);
    data.remove(0, pos + 2);

    int chunkSize = oneLine.toInt(NULL, 16);
    if (chunkSize == 0) break;
    if (data.length() < chunkSize) return false;
    QByteArray chunkData = data.left(chunkSize);
    items.push_back(Item(chunkSize, chunkData));
    LOG_DEBUG("chunkSize=%d", chunkSize); // gilgil temp 2014.04.09

    data.remove(0, data.length());
    if (data == "") break;
  }
  return true;
}

QByteArray VHttpBody::toByteArray()
{
  QByteArray res;
  for (Items::iterator it = items.begin(); it != items.end(); it++)
  {
    res += QString::number(it->first) + "\r\n" + it->second;
  }
  return res;
}
