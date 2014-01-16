#include <VMemoryLeak>
#include <malloc.h>

// ----------------------------------------------------------------------------
// VMemoryLeak
// ----------------------------------------------------------------------------
VMemoryLeak::VMemoryLeak()
{
#ifdef _DEBUG
  // printf("[VMemoryLeak.cpp] VMemoryLeak::VMemoryLeak()  %p\n", this); // gilgil temp 2012.05.07
#endif // _DEBUG
  _start(false, MEMORY_LEAK_CNT); // initialize memory
  m_del_check = false;
}

VMemoryLeak::~VMemoryLeak()
{
  _stop(false);    // check last memory leak
  start(false, 0); // clear memory
#ifdef _DEBUG
  // printf("[VMemoryLeak.cpp] VMemoryLeak::~VMemoryLeak() %p\n", this); // gilgil temp 2012.05.07
#endif // _DEBUG
}

void VMemoryLeak::_start(bool del_check, int cnt)
{
  if (m_info != NULL)
  {
    ::free(m_info);
    m_info = NULL;
  }
  m_del_check = del_check;
  if (cnt != 0)
  {
    m_info = (VMemoryLeakInfo*) ::malloc(sizeof(VMemoryLeakInfo) * cnt);
    for (int i = 0; i < cnt; i++)
    {
      VMemoryLeakInfo* info = &m_info[i];
      info->p    = NULL;
      info->size = 0;
      info->file = NULL;
      info->line = 0;
    }
  }
  m_cnt          = cnt;
  m_cur_max_cnt  = 0;
}

int VMemoryLeak::_stop(bool auto_free)
{
  m_del_check = false;
  bool write_start = false;
  int res = 0;
  for (int i = 0; i < m_cur_max_cnt; i++)
  {
    VMemoryLeakInfo* info = &m_info[i];
    if (info->p != NULL)
    {
      if (!write_start)
      {
        write_start = true;
        printf("******************************************************************************\n");
      }
      printf("%p memory leak %p(%u bytes) %s:%d\n", this, info->p, info->size, info->file, info->line);
      res++;
      if (auto_free)
      {
        ::free(info->p);
        info->p    = NULL;
        info->size = 0;
        info->file = NULL;
        info->line = 0;
      }
    }
  }
  if (write_start)
  {
    printf("******************************************************************************\n");
  }
  return res;
}

VMemoryLeakInfo* VMemoryLeak::find(void* p)
{
  for (int i = 0; i < m_cur_max_cnt; i++)
  {
    VMemoryLeakInfo* info = &m_info[i];
    if (info->p == p)
    {
      return info;
    }
  }
  return NULL;
}

VMemoryLeakInfo* VMemoryLeak::add(void* p, size_t size, char* file, int line)
{
  for (int i = 0; i < m_cnt; i++)
  {
    VMemoryLeakInfo* info = &m_info[i];
    if (info->p == NULL)
    {
      info->p    = p;
      info->size = size;
      info->file = file;
      info->line = line;
      if (i + 1 > m_cur_max_cnt) m_cur_max_cnt = i + 1;
      return info;
    }
  }
  return NULL;
}

VMemoryLeakInfo* VMemoryLeak::del(void* p)
{
  for (int i = 0; i < m_cur_max_cnt; i++)
  {
    VMemoryLeakInfo* info = &m_info[i];
    if (info->p == p)
    {
      info->p    = NULL;
      info->size = 0;
      info->file = NULL;
      info->line = 0;

      bool m_cur_max_cnt_move = true;
      VMemoryLeakInfo* temp = info + 1;
      for (int j = i + 1; j < m_cur_max_cnt; j++)
      {
        if (temp->p != NULL)
        {
          m_cur_max_cnt_move = false;
          break;
        }
        temp++;
      }

      if (m_cur_max_cnt_move) m_cur_max_cnt = i + 1;

      return info;
    }
  }
  return NULL;
}

VMemoryLeak& VMemoryLeak::instance()
{
  static VMemoryLeak g_memoryLeak;
  return g_memoryLeak;
}

void VMemoryLeak::start(bool del_check, int cnt)
{
  instance()._start(del_check, cnt);
}

int VMemoryLeak::stop(bool auto_free)
{
  return instance()._stop(auto_free);
}
