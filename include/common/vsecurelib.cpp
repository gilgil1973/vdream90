#include <VSecureLib>
#include <VDebugNew>

#ifndef __STDC_WANT_SECURE_LIB__     

errno_t fopen_s(FILE** fileHandle, const char* fileName, const char *mode)
{
  *fileHandle = fopen(fileName, mode);
  return 0;
}

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

errno_t wcstombs_s(size_t* size, char*    dst, size_t dstSize, const wchar_t* src, size_t maxCount)
{
  *size = wcstombs(dst, src, min(dstSize, maxCount));
  if (maxCount == *size) {
    dst[*size] = '\0';
  }
  return 0;
}

errno_t mbstowcs_s(size_t* size, wchar_t* dst, size_t dstSize, const char * src,   size_t maxCount)
{
  *size = mbstowcs(dst, src, min(dstSize, maxCount));
  if (maxCount == *size) {
    dst[*size] = '\0';
  }
  return 0;
}

#endif // __STDC_WANT_SECURE_LIB__
