#ifdef WIN32

#include <VLogDBWin32>
#include <winsock2.h>
#include <windows.h> // for OutputDebugString 
#include <VDebugNew>

REGISTER_METACLASS(VLogDBWin32, VLog)

// ----------------------------------------------------------------------------
// VLogDBWin32
// ----------------------------------------------------------------------------
void VLogDBWin32::write(const char* buf, int len)
{
#ifdef UNICODE
  std::wstring ws = char_to_wcs(buf, len);
  OutputDebugString(ws.c_str());
  OutputDebugString(L"\n");
#else
  OutputDebugString(buf);
  OutputDebugString("\n");
#endif // UNICODE
}

VLog* VLogDBWin32::createByURI(const QString& uri)
{
  if (uri == "dbwin32")
  {
    return new VLogDBWin32;
  }
  return NULL;
}

#endif // WIN32
