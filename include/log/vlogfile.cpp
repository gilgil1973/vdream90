#include <VLogFile>
#include <VFile>
#include <QUrl>
#include <VApp>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// VMyLogFile
// ----------------------------------------------------------------------------
class VMyLogFile : public VFile
{
public:
  VLog* getLog() { return NULL; }
};

REGISTER_METACLASS(VLogFile, VLog)

// ----------------------------------------------------------------------------
// VLogFile
// ----------------------------------------------------------------------------
const char* VLogFile::DEFAULT_LOG_FILE_NAME = "%04d%02d%02d.log";

VLogFile::VLogFile()
{
  showDateTime = VShowDateTime::DateTime;
  setFolder("");
  m_nowFileName = "";
  m_file = new VMyLogFile;
  // m_file.setLog(NULL); // gilgil temp 2012.09.18
  fileName = DEFAULT_LOG_FILE_NAME;
  closeOnWrite = true;
}

VLogFile::VLogFile(QString filePath)
{
  showDateTime = VShowDateTime::DateTime;
  QString _path     = QFileInfo(filePath).path();
  QString _fileName = QFileInfo(filePath).fileName();
  setFolder(_path);
  m_nowFileName = "";
  m_file = new VMyLogFile;
  // m_file.setLog(NULL); // gilgil temp 2012.09.18
  fileName = _fileName;
  if (fileName == "") fileName = DEFAULT_LOG_FILE_NAME;
  closeOnWrite = true;
}

VLogFile::~VLogFile()
{
  SAFE_DELETE(m_file);
}

void VLogFile::write(const char* buf, int len)
{
  VLock lock(*this);

  QDateTime now = QDateTime::currentDateTime();
  QString newFileName = qformat(qPrintable(fileName), now.date().year(), now.date().month(), now.date().day());
  if (newFileName != m_nowFileName)
  {
    m_file->close();
    m_nowFileName    = newFileName;
    m_file->fileName = m_folder + newFileName;
    m_file->mode     = QIODevice::Append | QIODevice::Text;
  }
  if (!m_file->active())
  {
    if (!m_file->open()) return;
  }
  m_file->write((char*)buf, (size_t)len);
  m_file->write((char*)"\n", 1);
  if (closeOnWrite)
  {
    m_file->close();
  }
}

VLog* VLogFile::createByURI(const QString& uri)
{
  if (uri == "file")
  {
    return new VLogFile;
  }
  QUrl _url(uri);
  if (_url.scheme() == "file")
  {
    QString skip = "file://";
    QString filePath = uri.mid(skip.length());
    return new VLogFile(filePath);
  }
  return NULL;
}

void VLogFile::setFolder(QString folder)
{
  if (folder != "")
  {
    QDir dir(folder);
    if (!dir.isAbsolute())
    {
      folder = QFileInfo(VApp::filePath()).path() + QDir::separator() + folder;
    }
    VFile::createFolder(folder);
    if (folder.right(1) != QDir::separator())
      folder += QDir::separator();
  }
  this->m_folder = folder;
}

void VLogFile::load(VXml xml)
{
  VLog::load(xml);

  QString _folder = xml.getStr("folder", folder()); setFolder(_folder);
  fileName        = xml.getStr("fileName", fileName);
  closeOnWrite    = xml.getBool("closeOnWrite", closeOnWrite);
}

void VLogFile::save(VXml xml)
{
  VLog::save(xml);

  xml.setStr("folder",        folder());
  xml.setStr("fileName",      fileName);
  xml.setBool("closeOnWrite", closeOnWrite);
}
