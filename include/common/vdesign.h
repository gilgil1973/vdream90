// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_DESIGN_H__
#define __V_DESIGN_H__

#include <typeinfo>

// ----------------------------------------------------------------------------
// VNonCopyable
// ----------------------------------------------------------------------------
class VNonCopyable
{
protected:
  VNonCopyable()  {}
  ~VNonCopyable() {}
private:
  VNonCopyable(const VNonCopyable&);
  const VNonCopyable& operator = (const VNonCopyable&);
};

// ----------------------------------------------------------------------------
// VLocalInstance (lazy initialization)
// ----------------------------------------------------------------------------
template <class T>
class VLocalInstance : private T
{
public:
  inline static T& instance()
  {
    // static local object(lazy initialization)
    static VLocalInstance<T> g_instance;
    return  g_instance;
  }
};

// ----------------------------------------------------------------------------
// VInstance (= VLocalInstance)
// ----------------------------------------------------------------------------
template <class T>
class VInstance : public VLocalInstance<T>
{
};

// ----------------------------------------------------------------------------
// VGlobalInstance (non-lazy initialization)
// ----------------------------------------------------------------------------
#ifdef WIN32
template <class T>
class VGlobalInstance : private T
{
protected:
  // static member object(non-lazy initialization)
  static VGlobalInstance<T> g_instance;
public:
  inline static T& instance()
  {
    return g_instance;
  }
};
template <class T> T VGlobalInstance<T>::g_instance;
#endif // WIN32

// ----------------------------------------------------------------------------
// VPrimitive
// ----------------------------------------------------------------------------
template <class T>
class VPrimitive
{
protected:
  T value;
public:
  VPrimitive()                        {                        }
  VPrimitive(const VPrimitive& b)     { this->value = b.value; }
  VPrimitive(const T           value) { this->value = value;   }
  ~VPrimitive()                       {                        } // do not declare as virtual
  operator T()             const      { return value;          }
  const    T* operator &() const      { return &value;         }
};

// ----------------------------------------------------------------------------
// VBase
// ----------------------------------------------------------------------------
template <class T>
class VBase
{
protected:
  T base;
public:
  VBase()                        {                      }
  VBase(const VBase& b)          { this->base = b.base; }
  VBase(const T&  base)          { this->base = base;   }
  ~VBase()                       {                      } // do not declare as virtual
  operator T&()            const { return (T&)base;     }
  const    T* operator &() const { return &base;        }
};

// ----------------------------------------------------------------------------
// Property
// ----------------------------------------------------------------------------
#define EXPORT_PROPERTY(InType,inName,OutType,outName)                                              \
  inline OutType __________get_##outName()           { return (OutType)inName; }                    \
  inline void __________set_##outName(OutType value) { inName = (InType)value; }                    \
  __declspec (property (get=__________get_##outName, put=__________set_##outName)) OutType outName;

#define EXPORT_READ_PROPERTY(InType,inName,OutType,outName)                                         \
  inline OutType __________get_##outName()           { return (OutType)inName; }                    \
  __declspec (property (get=__________get_##outName)) OutType outName;

#define EXPORT_WRITE_PROPERTY(InType,inName,OutType,outName)                                        \
  inline void __________set_##outName(OutType value) { inName = (InType)value; }                    \
  __declspec (property (put=__________set_##outName)) OutType outName;

#define EXPORT_POINTER_PROPERTY(InType,inName,OutType,outName)                                      \
  inline OutType* __________get_##outName()        { return (OutType*)&inName; }                    \
  __declspec (property (get=__________get_##outName)) OutType* ##outName;

// ----- gilgil temp 2012.11.01 -----
/*
// ----------------------------------------------------------------------------
// VClassInitialize
// ----------------------------------------------------------------------------
//
// Class T must have the following methods.
// private:
//   friendclass VClassInitialize<T>;
//   static void doClassInitialize();
//   static void doClassFinalize();
//
template <class T>
class VClassInitialize
{
private:
  static int classRefCount;
public:
  static void classInitialize() { if (classRefCount++ == 0) T::doClassInitialize(); }
  static void classFinalize()   { if (--classRefCount == 0) T::doClassFinalize();   }
};
template <class T> int VClassInitialize<T>::classRefCount = 0;
*/
// ----------------------------------

// ----------------------------------------------------------------------------
// VDependencyWith
// ----------------------------------------------------------------------------
template <class T>
class VDependencyWith
{
public:
  /// class T must have static void dependency()
  VDependencyWith()          { T::dependency(); }
  virtual ~VDependencyWith() {}
};

// ----------------------------------------------------------------------------
// VAutoClassInitializer
// ----------------------------------------------------------------------------
//
// NO_USE_AUTO_CLASS_INITIALIZER is used to not to use
// initialize and finalize class automatically.
// #define NO_USE_AUTO_CLASS_INITIALIZER
//
#ifndef NO_USE_AUTO_CLASS_INITIALIZER
template <class T>
class VAutoClassInitializer
{
public:
  VAutoClassInitializer()          { T::classInitialize(); }
  virtual ~VAutoClassInitializer() { T::classFinalize();   }
};
#else
template <class T>
class VAutoClassInitializer {};
#endif // NO_USE_AUTO_CLASS_INITIALIZER

#endif // __V_DESIGN_H__
