// ----------------------------------------------------------------------------
//
// vdream Component Suite version 7.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_MEMORY_LEAK_H__
#define __V_MEMORY_LEAK_H__

#include <malloc.h> // for malloc and free
#include <stdio.h>  // for size_t

// ----------------------------------------------------------------------------
// VMemoryLeakInfo
// ----------------------------------------------------------------------------
typedef struct
{
  void*  p;
  size_t size;
  char*  file;
  int    line;
} VMemoryLeakInfo;

// ----------------------------------------------------------------------------
// VMemoryLeak
// ----------------------------------------------------------------------------
class VMemoryLeak
{
  friend void* debug_add(size_t size, const char* file, const int line, void* p);
  friend void  debug_del(void* p,     const char* file, const int line);

protected:
  VMemoryLeakInfo* m_info;
  bool             m_del_check;
  int              m_cnt;
  int              m_cur_max_cnt;

public:
  static const int MEMORY_LEAK_CNT = 65536;

public:
  VMemoryLeak();
  virtual ~VMemoryLeak();

protected:
  void _start(bool del_check, int cnt);
  int  _stop(bool auto_free);

protected:
  VMemoryLeakInfo* find(void* p);
  VMemoryLeakInfo* add (void* p, size_t size, char* file, int line);
  VMemoryLeakInfo* del (void* p);

public:
  static VMemoryLeak& instance();
  static void start(bool del_check = true, int cnt = MEMORY_LEAK_CNT);
  static int  stop(bool auto_free = true);
};

#endif // __V_MEMORY_LEAK_H__
