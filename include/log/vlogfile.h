// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_LOG_FILE_H__
#define __V_LOG_FILE_H__

#include <VLog>
#include <VSystem> // for Lockable

// ----------------------------------------------------------------------------
// VLogFile
// ----------------------------------------------------------------------------
class VMyLogFile;
class VLogFile : public VLog, public VLockable
{
public:
  static const char* DEFAULT_LOG_FILE_NAME;

public:
  VLogFile();
  VLogFile(QString filePath);
  virtual ~VLogFile();

protected:
  virtual void write(const char* buf, int len);

public:
  virtual VLog* createByURI(const QString& uri);

protected:
  QString     m_folder;
  QString     m_nowFileName;
  VMyLogFile* m_file;
public:
  QString folder() { return m_folder; }
  void    setFolder(QString folder);
  QString fileName;
  bool    closeOnWrite;

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

#endif // __V_LOG_FILE_H__
