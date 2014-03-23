#define _SCL_SECURE_NO_DEPRECATE // for remive warinig 'std::_Copy_opt' was declared deprecated
#include <VString>
#include <VSecureLib>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// String function
// ----------------------------------------------------------------------------
static const int DEFAULT_STR_BUF_SIZE = 4096;
std::string format (const char* fmt, ...)
{
  char buf[DEFAULT_STR_BUF_SIZE];
  va_list args;
  va_start(args, fmt);
  vsprintf_s(buf, DEFAULT_STR_BUF_SIZE, fmt, args);
  va_end(args);
  return (std::string)buf;
}

std::wstring wformat(const char* fmt, ...)
{
  char buf[DEFAULT_STR_BUF_SIZE];
  va_list args;
  va_start(args, fmt);
  vsprintf_s(buf, DEFAULT_STR_BUF_SIZE, fmt, args);
  va_end(args);
  return char_to_wcs(buf, DEFAULT_STR_BUF_SIZE);
}

QString qformat(const char* fmt, ...)
{
  char buf[DEFAULT_STR_BUF_SIZE];
  va_list args;
  va_start(args, fmt);
  vsprintf_s(buf, DEFAULT_STR_BUF_SIZE, fmt, args);
  va_end(args);
  return QString(buf);
}

size_t char_to_wchar(wchar_t* ws, size_t ws_len, const char* s, size_t s_len)
{
  size_t size;
  if (s_len == 0) s_len = strlen(s);
  mbstowcs_s(&size, ws, ws_len, s, s_len);
  return size;
}

std::wstring char_to_wcs(const char* s, size_t s_len)
{
  size_t size;
  if (s_len == 0) s_len = strlen(s);
  wchar_t* ws = new wchar_t[s_len + 1];
  mbstowcs_s(&size, ws, s_len + 1, s, s_len);
  std::wstring res = ws;
  delete[] ws;
  return res;
}

size_t mbs_to_wchar (wchar_t* ws, size_t ws_len, const std::string s)
{
  size_t size;
  mbstowcs_s(&size, ws, ws_len, s.c_str(), s.length());
  return size;
}

std::wstring mbs_to_wcs (const std::string s)
{
  size_t size;
  wchar_t* ws = new wchar_t[s.length() + 1];
  mbstowcs_s(&size, ws, s.length() + 1, s.c_str(), s.length());
  std::wstring res = ws;
  delete[] ws;
  return res;
}

size_t wchar_to_char(char* s, size_t s_len, const wchar_t* ws, size_t ws_len)
{
  size_t size;
  if(ws_len == 0) ws_len = wcslen(ws);
  wcstombs_s(&size, s, s_len, ws, ws_len);
  return size;
}

std::string wchar_to_mbs(const wchar_t* ws, size_t ws_len)
{
  size_t size;
  if (ws_len == 0) ws_len = wcslen(ws);
  char* s = new char[ws_len + 1];
  wcstombs_s(&size, s, ws_len + 1, ws, ws_len);
  std::string res = s;
  delete[] s;
  return res;
}

size_t wcs_to_char(char* s, size_t s_len, const std::wstring ws)
{
  size_t size;
  wcstombs_s(&size, s, s_len, ws.c_str(), ws.length());
  return size;
}

std::string wcs_to_mbs(const std::wstring ws)
{
  size_t size;
  char* s = new char[ws.length() + 1];
  wcstombs_s(&size, s, ws.length() + 1, ws.c_str(), ws.length());
  std::string res = s;
  delete[] s;
  return res;
}

#ifdef GTEST
#include <gtest/gtest.h>

TEST( String, ss_to_vs )
{
  std::string ss = "std::string";
  QString     qs = ss.c_str();
  printf("%s\n", qPrintable(qs));
}

TEST( String, vs_to_ss )
{
  QString qs = "test";
  std::string ss = qs.toLatin1().data();
  printf("%s\n", ss.c_str());
}

#endif // GTEST
