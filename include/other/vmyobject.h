// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_MY_OBJECT_H__
#define __V_MY_OBJECT_H__

#include <VObject>
#include <VMetaClass>

// ----------------------------------------------------------------------------
// VMyObject
// ----------------------------------------------------------------------------
class VMyObject : public VObject
{
  Q_OBJECT

public:
  static void explicitLink();

public:
  void foo(int i);

signals:
  void send(int i);

public slots:
  void recv(int i);
};

class VA  : public VMyObject
{
public:
  VA()          { LOG_DEBUG("VA::VA");  }
  virtual ~VA() { LOG_DEBUG("VA::~VA"); }
};

class VB  : public VMyObject
{
public:
  VB()          { LOG_DEBUG("VB::VB");  }
  virtual ~VB() { LOG_DEBUG("VB::~VB"); }
};

class VC  : public VMyObject
{
public:
  VC()          { LOG_DEBUG("VC::VC");  }
  virtual ~VC() { LOG_DEBUG("VC::~VC"); }
};

class VAA : public VA
{
public:
  VAA()          { LOG_DEBUG("VAA::VAA");  }
  virtual ~VAA() { LOG_DEBUG("VAA::~VAA"); }
};

class VAB : public VA
{
public:
  VAB()          { LOG_DEBUG("VAB::VAB");  }
  virtual ~VAB() { LOG_DEBUG("VAB::~VAB"); }
};

class VAC : public VA
{
public:
  VAC()          { LOG_DEBUG("VAC::VAC");  }
  virtual ~VAC() { LOG_DEBUG("VAC::~VAC"); }
};

class VBA : public VB
{
public:
  VBA()          { LOG_DEBUG("VBA::VBA");  }
  virtual ~VBA() { LOG_DEBUG("VBA::~VBA"); }
};

class VBB : public VB
{
public:
  VBB()          { LOG_DEBUG("VBB::VBB");  }
  virtual ~VBB() { LOG_DEBUG("VBB::~VBB"); }
};

class VBC : public VB
{
public:
  VBC()          { LOG_DEBUG("VBC::VBC");  }
  virtual ~VBC() { LOG_DEBUG("VBC::~VBC"); }
};

class VCA : public VC
{
public:
  VCA()          { LOG_DEBUG("VCA::VCA");  }
  virtual ~VCA() { LOG_DEBUG("VCA::~VCA"); }
};

class VCB : public VC
{
public:
  VCB()          { LOG_DEBUG("VCB::VCB");  }
  virtual ~VCB() { LOG_DEBUG("VCB::~VCB"); }
};

class VCC : public VC
{
public:
  VCC()          { LOG_DEBUG("VCC::VCC");  }
  virtual ~VCC() { LOG_DEBUG("VCC::~VCC"); }
};

#endif // __V_MY_OBJECT_H__
