#include <VHttpChunkBody>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VHttpChunkBody
// ----------------------------------------------------------------------------
VHttpChunkBody::VHttpChunkBody()
{
  clear();
}

void VHttpChunkBody::clear()
{
  items.clear();
}

int VHttpChunkBody::parse(QByteArray& buffer)
{
  clear();
  int res = 0;
  while (true)
  {
    QByteArray _buffer = buffer;

    //
    // chunkSize
    //
    int pos = _buffer.indexOf("\r\n");
    if (pos == -1)
    {
      break;
    }

    QByteArray oneLine = _buffer.left(pos);
    _buffer.remove(0, pos + 2); // "\r\n"

    bool ok;
    int chunkSize = oneLine.toInt(&ok, 16);
    if (!ok) break;
    if (_buffer.length() < chunkSize + 2) break;

    //
    // chunkData
    //
    QByteArray chunkData = _buffer.left(chunkSize);
    _buffer.remove(0, chunkSize);
    if (_buffer.length() < 2 || (_buffer[0] != '\r' || _buffer[1] != '\n'))
    {
      LOG_WARN("invalid chunk format");
      break;
    }
    _buffer.remove(0, 2); // "\r\n"

    items.push_back(Item(chunkSize, chunkData));
    // LOG_DEBUG("chunkSize=%d(0x%x) %s %s", chunkSize, chunkSize, qPrintable(chunkData.left(10)), qPrintable(chunkData.right(10))); // gilgil temp 2014.04.11

    buffer = _buffer;
    res++;
    if (chunkSize == 0)
    {
      break; // may be last chunk
    }
  }
  return res;
}

QByteArray VHttpChunkBody::toByteArray()
{
  QByteArray res;
  for (Items::iterator it = items.begin(); it != items.end(); it++)
  {
    Item&       item      = *it;
    int         chunkSize = item.first;
    QByteArray& chunkData = item.second;
    res += QByteArray::number(chunkSize, 16) + "\r\n" + chunkData + "\r\n";
  }
  return res;
}
