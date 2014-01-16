#include <VLogNull>
#include <VDebugNew>

REGISTER_METACLASS(VLogNull, VLog)

// ----------------------------------------------------------------------------
// VLogNull
// ----------------------------------------------------------------------------
VLog* VLogNull::createByURI(const QString& uri)
{
  if (uri == "null")
  {
    return new VLogNull;
  }
  return NULL;
}
