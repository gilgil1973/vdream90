#include <VMyObject>

REGISTER_ABSTRACT_METACLASS(VA, VMyObject)
REGISTER_ABSTRACT_METACLASS(VB, VMyObject)
REGISTER_ABSTRACT_METACLASS(VC, VMyObject)

REGISTER_METACLASS(VAA, VA)
REGISTER_METACLASS(VAB, VA)
REGISTER_METACLASS(VAC, VA)

REGISTER_METACLASS(VBA, VB)
REGISTER_METACLASS(VBB, VB)
REGISTER_METACLASS(VBC, VB)

REGISTER_METACLASS(VCA, VC)
REGISTER_METACLASS(VCB, VC)
REGISTER_METACLASS(VCC, VC)

// ----------------------------------------------------------------------------
// VMyObject
// ----------------------------------------------------------------------------
void VMyObject::explicitLink()
{
  // ----- by gilgil 2014.03.25 -----
  /*
  VAA aa;
  VAB ab;
  VAC ac;
  VBA ba;
  VBB bb;
  VBC bc;
  VCA ca;
  VCB cb;
  VCC cc;
  */
  // --------------------------------
}

void VMyObject::foo(int i)
{
  emit send(i);
}

void VMyObject::recv(int i)
{
  LOG_DEBUG("recv i=%d", i);
}
