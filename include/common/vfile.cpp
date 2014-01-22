#include <VFile>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VFile
// ----------------------------------------------------------------------------
VFile::VFile(void* owner) : VRwObject(owner)
{
  fileName = "";
  mode     = QIODevice::NotOpen;
}

VFile::VFile(const QString& fileName) : VRwObject(NULL)
{
  this->fileName = fileName;
  mode           = QIODevice::NotOpen;
}

VFile::VFile(const QString& fileName, QIODevice::OpenMode mode)
{
  this->fileName = fileName;
  this->mode     = mode;
}

VFile::~VFile()
{
  close();
}

bool VFile::doOpen()
{
  if (fileName == "")
  {
    SET_ERROR(VFileError, qformat("file name is null"), VERR_FILENAME_NOT_SPECIFIED);
    return false;
  }
  base.setFileName(fileName);
  if (!base.open(mode))
  {
    if (mode | QIODevice::ReadOnly)
    {
      if (!QFile::exists(fileName))
      {
        SET_ERROR(VFileError, qformat("file not exist(%s)", qPrintable(fileName)), VERR_FILE_NOT_EXIST);
        return false;
      }
    }
    SET_ERROR(VFileError, base.errorString(), base.error());
    return false;
  }
  return true;
}

bool VFile::doClose()
{
  base.close();
  return true;
}

int VFile::doRead(char* buf,  int size)
{
  int res = (int)base.read(buf, (qint64)size);
  if (res == -1)
  {
    SET_ERROR(VFileError, qformat("error in read file(%s)", qPrintable(fileName)), VERR_ERROR_IN_READ_FILE);
    return VERR_FAIL;
  }
  if (res == 0)
  {
    if (base.atEnd())
    {
      SET_DEBUG_ERROR(VFileError, qformat("end of file(%s)", qPrintable(fileName)), VERR_END_OF_FILE);
    } else
    {
      SET_ERROR(VFileError, qformat("read return 0(%s) %s", qPrintable(fileName), qPrintable(base.errorString())), base.error());
    }
    return VERR_FAIL;
  }
  return res;
}

int VFile::doWrite(char* buf, int size)
{
  int res = (int)base.write(buf, (qint64)size);
  if (res != size)
  {
    SET_ERROR(VFileError, qformat("error in write file(%s) %s", qPrintable(fileName), qPrintable(base.errorString())), base.error());
    return VERR_FAIL;
  }
  return res;
}

bool VFile::createFolder(QString folder)
{
#ifdef WIN32
  while (true)
  {
    int pos = folder.indexOf("/");
    if (pos == -1) break;
    folder = folder.replace(pos, 1, "\\");
  }
#endif // WIN32
  if (folder != "")
  {
    // ----- gilgil temp 2012.05.26 -----
    if (folder.right(1) != QDir::separator())
      folder += QDir::separator();
    // ----------------------------------
    QDir dir(folder);
    if (!dir.exists())
    {
      bool res;
      if (dir.isAbsolute())
        res = dir.mkdir(folder);
      else
      {
        QString dirName = QDir::currentPath() + QDir::separator() + folder;
        res = dir.mkdir(dirName);
      }
      if (!res)
      {
        LOG_ERROR("can not create folder(%s)", qPrintable(folder));
      }
    }
  }
  return true;
}

void VFile::load(VXml xml)
{
  VRwObject::load(xml);

  fileName = xml.getStr("fileName", fileName);
  mode = (QIODevice::OpenMode)xml.getInt("mode", (int)mode);
}

void VFile::save(VXml xml)
{
  VRwObject::save(xml);

  xml.setStr("fileName", fileName);
  xml.setInt("mode", (int)mode);
}
