#include <VMemoryLeak>

#ifdef _DEBUG

#include <malloc.h>

#define debug_new_log // printf

// ----------------------------------------------------------------------------
// Overload functions
// ----------------------------------------------------------------------------
void* debug_add(size_t size, const char* file, const int line, void* p)
{
  VMemoryLeakInfo* info = VMemoryLeak::instance().find(p);
  if (info != NULL)
  {
    printf("already exist %p %s %d\n", p, file, line);
    printf("existing info : %p(%u) %s:%d\n", info->p, info->size, info->file, info->line);
    return NULL;
  }
  info = VMemoryLeak::instance().add(p, size, (char*)file, (int)line);
  if (info == NULL)
  {
    printf("can not add %p(%u) %s:%d\n", p, size, file, line);
    return NULL;
  }
  return p;
}

void debug_del(void* p, const char* file, const int line)
{
  VMemoryLeakInfo* info = VMemoryLeak::instance().del(p);
  if (VMemoryLeak::instance().m_del_check && info == NULL)
  {
    printf("%p can not del %p %s:%d\n", &VMemoryLeak::instance(),p, file, line);
  }
}

void* debug_malloc(size_t size, const char* file, const int line)
{
  void* p = ::malloc(size);
  debug_new_log("debug_malloc(%u, %s, %d) %p\n", size, file, line, p);
  void* res = debug_add(size, file, line, p);
  if (res == NULL) ::free(p);
  return res;
}

void debug_free(void* p, const char* file, const int line)
{
  debug_new_log("debug_free(%p, %s, %d)\n", p, file, line);
  debug_del(p, file, line);
  ::free(p);
}

void* operator new (size_t size, const char* file, const int line)
{
  void* p = debug_malloc(size, file, line);
  debug_new_log("new(%u, %s, %d) %p\n", size, file, line, p);
  return p;
}

void operator delete (void* p, const char* file, const int line)
{
  debug_new_log("delete(%p, %s, %d)\n", p, file, line);
  operator delete(p);
  debug_free(p, file, line);
}

void operator delete (void* p)
{
  debug_new_log("delete(%p)\n", p);
  return debug_free(p, "unknown", 0);
}

void* operator new[] (size_t size, const char* file, const int line)
{
  void* p = debug_malloc(size, file, line);
  debug_new_log("new[](%u, %s, %d) %p\n", size, file, line, p);
  return p;
}

void operator delete[] (void* p, const char* file, const int line)
{
  debug_new_log("delete[](%p, %s, %d)\n", p, file, line);
  return debug_free(p, file, line);
}

void  operator delete[] (void* p)
{
  debug_new_log("delete[](%p)\n", p);
  return debug_free(p, "unknown", 0);
}

#endif // _DEBUG
