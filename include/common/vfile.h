// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_FILE_H__
#define __V_FILE_H__

#include <QFile>
#include <VDesign>
#include <VRwObject>

// ----------------------------------------------------------------------------
// VFile
// ----------------------------------------------------------------------------
class VFile : public VRwObject, public VBase<QFile>
{
public:
  VFile(void* owner = NULL);
  VFile(const QString& fileName);
  VFile(const QString& fileName, QIODevice::OpenMode mode);
  virtual ~VFile();

public:
  QString             fileName;
  QIODevice::OpenMode mode;

protected:
  bool doOpen();
  bool doClose();
  int  doRead(char* buf,  int size);
  int  doWrite(char* buf, int size);

public:
  static bool createFolder(QString folder);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

// ----------------------------------------------------------------------------
// FileError
// ----------------------------------------------------------------------------
VDECLARE_ERROR_CLASS(VFileError)

// ----------------------------------------------------------------------------
// FileError code
// ----------------------------------------------------------------------------
static const int VERR_END_OF_FILE            = VERR_CATEGORY_FILE + 0;
static const int VERR_ERROR_IN_OPEN_FILE     = VERR_CATEGORY_FILE + 1;
static const int VERR_ERROR_IN_READ_FILE     = VERR_CATEGORY_FILE + 2;
static const int VERR_ERROR_IN_WRITE_FILE    = VERR_CATEGORY_FILE + 3;
static const int VERR_FILE_NOT_EXIST         = VERR_CATEGORY_FILE + 4;
static const int VERR_ERROR_IN_FILESEEK      = VERR_CATEGORY_FILE + 5;
static const int VERR_FILENAME_NOT_SPECIFIED = VERR_CATEGORY_FILE + 6;
static const int VERR_INVALID_FILENAME       = VERR_CATEGORY_FILE + 7;

#endif // __V_FILE_H__

