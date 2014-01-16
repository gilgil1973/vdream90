// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_RW_OBJECT_H__
#define __V_RW_OBJECT_H__

#include <VObject>

// ----------------------------------------------------------------------------
// VRwObject
// ----------------------------------------------------------------------------
class VRwObject : public VObject
{
public:
  VRwObject(void* owner = NULL) : VObject(owner) {}

public:
  int        read(char* buf, int size);
  int        read(QByteArray& ba, int size = vd::DEFAULT_BUF_SIZE);
  QByteArray read(int size = vd::DEFAULT_BUF_SIZE);

  int        write(char* buf, int size);
  int        write(char* buf);
  int        write(const char* buf);
  int        write(const char* buf, int size);
  int        write(QString& s);
  int        write(QByteArray ba);

protected:
  virtual int doRead(char* buf,  int size) = 0;
  virtual int doWrite(char* buf, int size) = 0;
};

#endif // __V_RW_OBJECT_H__
