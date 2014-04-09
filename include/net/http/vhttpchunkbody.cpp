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

  QByteArray _buffer = buffer;
  int res = 0;

  while (true)
  {
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

    int chunkSize = oneLine.toInt(NULL, 16);
    if (chunkSize == 0 && oneLine != "0") break;
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
    _buffer.remove(0, 2);

    items.push_back(Item(chunkSize, chunkData));

    LOG_DEBUG("chunkSize=%d(%x)", chunkSize, chunkSize); // gilgil temp 2014.04.09

    res++;

    if (chunkSize == 0) break; // may be last chunk
  }

  if (res > 0)
  {
    buffer = _buffer;
  }
  return res;
}

QByteArray VHttpChunkBody::toByteArray()
{
  QByteArray res;
  for (Items::iterator it = items.begin(); it != items.end(); it++)
  {
    res += QString::number(it->first, 16) + "\r\n" + it->second + "\r\n";
  }
  return res;
}
