// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

// http://en.wikipedia.org/wiki/Operators_in_C_and_C%2B%2B

#ifndef __V_INT_H__
#define __V_INT_H__

#include <stdio.h>   // for vsprintf
#include <stdarg.h>  // for va_list
#include <stdlib.h>  // for malloc

#ifdef WIN32
// #pragma warning( disable : 4100 ) // gilgil temp 2012..5.20
#endif // WIN32

// ----------------------------------------------------------------------------
// VInt
// ----------------------------------------------------------------------------
class VInt
{
protected:
  int _i;

public:
  // --------------------------------------------------------------------------
  // Constructor and destructor
  // --------------------------------------------------------------------------
  VInt()                                 { D("%p VInt::VInt()\n",              this);               _i = 0;                                  } // A a      : Default constructor
  VInt(const VInt& b)                    { D("%p VInt::VInt(VInt& %d) %p\n",   this, b._i, &b);     _i = b._i;                               } // A a(b)   : Copy constructor
  VInt(const int i)                      { D("%p VInt::VInt(int %d)\n",        this, i);            _i = i;                                  } // A a(i)   : Conversion constructor
  ~VInt()                                { D("%p VInt::~VInt() %d\n",          this, _i);                                                    } // delete a : Destrutor

  // --------------------------------------------------------------------------
  // Arithmetic operators
  // --------------------------------------------------------------------------
  VInt& operator = (const VInt& b)       { D("%p VInt(%d)::= (VInt& %d) %p\n", this, _i, b._i, &b); _i = b._i;            return *this;      } // a = b : Basic assignment
  VInt  operator + (const VInt& b) const { D("%p VInt(%d)::+ (VInt& %d) %p\n", this, _i, b._i, &b); VInt res(_i + b._i);  return res;        } // a + b : Addition
  VInt  operator - (const VInt& b) const { D("%p VInt(%d)::- (VInt& %d) %p\n", this, _i, b._i, &b); VInt res(_i - b._i);  return res;        } // a - b : Subtraction
  VInt  operator + () const              { D("%p VInt(%d)::+ ()\n",            this, _i);           VInt res(_i);         return res;        } // +a    : Unary plus
  VInt  operator - () const              { D("%p VInt(%d)::- ()\n",            this, _i);           VInt res(-_i);        return res;        } // -a    : Unary minus
  VInt  operator * (const VInt& b) const { D("%p VInt(%d)::* (VInt& %d) %p\n", this, _i, b._i, &b); VInt res(_i * b._i);  return res;        } // a * b : Multiplication
  VInt  operator / (const VInt& b) const { D("%p VInt(%d)::/ (VInt& %d) %p\n", this, _i, b._i, &b); VInt res(_i / b._i);  return res;        } // a / b : Division
  VInt  operator % (const VInt& b) const { D("%p VInt(%d)::% (VInt& %d) %p\n", this, _i, b._i, &b); VInt res(_i % b._i);  return res;        } // a % b : Modulo
  VInt& operator ++()                    { D("%p VInt(%d)::++()\n",            this, _i);           _i++;                 return *this;      } // ++a   : Prefix increment
  VInt  operator ++(int)                 { D("%p VInt(%d)::++(int)\n",         this, _i);           VInt res(_i); _i++;   return res;        } // a++   : Suffix increment
  VInt& operator --()                    { D("%p VInt(%d)::--()\n",            this, _i);           _i--;                 return *this;      } // --a   : Prefix decrement
  VInt  operator --(int)                 { D("%p VInt(%d)::--(int)\n",         this, _i);           VInt res(_i); _i--;   return res;        } // a--   : Suffix decrement

  // --------------------------------------------------------------------------
  // Comparison operators/relational operators
  // --------------------------------------------------------------------------
  bool  operator ==(const VInt& b) const { D("%p VInt(%d)::==(VInt& %d) %p\n", this, _i, b._i, &b);                       return _i == b._i; } // a == b : Equal to
  bool  operator !=(const VInt& b) const { D("%p VInt(%d)::!=(VInt& %d) %p\n", this, _i, b._i, &b);                       return _i != b._i; } // a != b : Not equal to
  bool  operator > (const VInt& b) const { D("%p VInt(%d)::> (VInt& %d) %p\n", this, _i, b._i, &b);                       return _i >  b._i; } // a > b  : Greater than
  bool  operator < (const VInt& b) const { D("%p VInt(%d)::< (VInt& %d) %p\n", this, _i, b._i, &b);                       return _i <  b._i; } // a < b  : Less than
  bool  operator >=(const VInt& b) const { D("%p VInt(%d)::>=(VInt& %d) %p\n", this, _i, b._i, &b);                       return _i >= b._i; } // a >= b : Greater than or equal to
  bool  operator <=(const VInt& b) const { D("%p VInt(%d)::<=(VInt& %d) %p\n", this, _i, b._i, &b);                       return _i <= b._i; } // a <= b : Less than or equal to

  // --------------------------------------------------------------------------
  // Logical operators
  // --------------------------------------------------------------------------
  bool  operator ! () const              { D("%p VInt(%d)::! ()\n",            this, _i);                                 return !_i;        } // !a     : Logical NOT
  bool  operator &&(const VInt& b) const { D("%p VInt(%d)::&&(VInt& %d) %p\n", this, _i, b._i, &b);                       return _i && b._i; } // a && b : Logical AND
  bool  operator ||(const VInt& b) const { D("%p VInt(%d)::||(VInt& %d) %p\n", this, _i, b._i, &b);                       return _i || b._i; } // a || b : Logical OR

  // --------------------------------------------------------------------------
  // Bitwise operators
  // --------------------------------------------------------------------------
  VInt  operator ~ ()              const { D("%p VInt(%d)::~ ()\n",            this, _i);           VInt res(~_i);        return res;        } // ~a     : Bitwise NOT
  VInt  operator & (const VInt& b) const { D("%p VInt(%d)::& (VInt& %d) %p\n", this, _i, b._i, &b); VInt res(_i & b._i);  return res;        } // a & b  : Bitwise AND
  VInt  operator | (const VInt& b) const { D("%p VInt(%d)::| (VInt& %d) %p\n", this, _i, b._i, &b); VInt res(_i | b._i);  return res;        } // a | b  : Bitwise OR
  VInt  operator ^ (const VInt& b) const { D("%p VInt(%d)::^ (VInt& %d) %p\n", this, _i, b._i, &b); VInt res(_i ^ b._i);  return res;        } // a ^ b  : Bitwise XOR
  VInt  operator <<(const VInt& b) const { D("%p VInt(%d)::<<(VInt& %d) %p\n", this, _i, b._i, &b); VInt res(_i << b._i); return res;        } // a << b : Bitwise left shift
  VInt  operator >>(const VInt& b) const { D("%p VInt(%d)::>>(VInt& %d) %p\n", this, _i, b._i, &b); VInt res(_i >> b._i); return res;        } // a >> b : Bitwise right shift

  // --------------------------------------------------------------------------
  // Compound assignment operators
  // --------------------------------------------------------------------------
  VInt& operator +=(const VInt& b)       { D("%p VInt(%d)::+=(VInt& %d) %p\n", this, _i, b._i, &b); _i += b._i;           return *this;      } // a += b  : Addition assignmentf
  VInt& operator -=(const VInt& b)       { D("%p VInt(%d)::-=(VInt& %d) %p\n", this, _i, b._i, &b); _i -= b._i;           return *this;      } // a -= b  : Subtraction assignment
  VInt& operator *=(const VInt& b)       { D("%p VInt(%d)::*=(VInt& %d) %p\n", this, _i, b._i, &b); _i *= b._i;           return *this;      } // a *= b  : Multiplication assignment
  VInt& operator /=(const VInt& b)       { D("%p VInt(%d)::/=(VInt& %d) %p\n", this, _i, b._i, &b); _i /= b._i;           return *this;      } // a /= b  : Division assignment
  VInt& operator %=(const VInt& b)       { D("%p VInt(%d)::%=(VInt& %d) %p\n", this, _i, b._i, &b); _i %= b._i;           return *this;      } // a %= b  : Modulo assignment
  VInt& operator &=(const VInt& b)       { D("%p VInt(%d)::&=(VInt& %d) %p\n", this, _i, b._i, &b); _i &= b._i;           return *this;      } // a &= b  : Bitwise AND assignment
  VInt& operator |=(const VInt& b)       { D("%p VInt(%d)::|=(VInt& %d) %p\n", this, _i, b._i, &b); _i |= b._i;           return *this;      } // a |= b  : Bitwise OR assignment
  VInt& operator ^=(const VInt& b)       { D("%p VInt(%d)::^=(VInt& %d) %p\n", this, _i, b._i, &b); _i ^= b._i;           return *this;      } // a ^= b  : Bitwise XOR assignment
  VInt& operator <<=(const VInt& b)      { D("%p VInt(%d)::<<=(VInt& %d) %p\n", this, _i, b._i, &b); _i <<= b._i;         return *this;      } // a <<= b : Bitwise left shift assignment
  VInt& operator >>=(const VInt& b)      { D("%p VInt(%d)::>>=(VInt& %d) %p\n", this, _i, b._i, &b); _i >>= b._i;         return *this;      } // a >>= b : Bitwise right shift assignment

  // --------------------------------------------------------------------------
  // Member and pointer operators
  // --------------------------------------------------------------------------
  // R&   operator [](const T2& b)                                                                                                             // a[b]  : Array subscript
  // R&   operator *()                                                                                                                         // *a    : Indirection ("object pointed to by a")
  // VInt* operator & ()                    { D("%p VInt(%d)::& ()\n",            this, _i);                                 return this;    } // &a    : Reference ("address of a")
  // R*   operator ->()                                                                                                                        // a->b  : Structure dereference ("member b of object pointed to by a")
  // N/A                                                                                                                                       // a.b   : Structure reference ("member b of object a")
  // R    operator->*(R)                                                                                                                       // a->*b : Member pointed to by b of object pointed to by a
  // N/A                                                                                                                                       // a.*b  : Member pointed to by b of object a

  
  // --------------------------------------------------------------------------
  // Other operators
  // --------------------------------------------------------------------------
  // R    operator ()(Arg1 a1, Arg2 a2, ¡¦)                                                                                                     // a(a1, a2)               : Function call
  // R&   operator ,(R& b) const                                                                                                               // a, b                    : Comma
  // N/A                                                                                                                                       // a ? b : c               : Ternary conditional
  // N/A                                                                                                                                       // a::b                    : Scope resolution
  // N/A                                                                                                                                       // sizeof(a), sizeof(A)    : Size-of
  // N/A                                                                                                                                       // alignof(A), _Alignof(A) : Align-of
  // N/A                                                                                                                                       // typeid(a), typeid(A)    : Type identification
  operator int() const                   { D("%p VInt::int(%d)\n",             this, _i);                                 return _i;         } // (type)a                 : Cast
  void* operator new(size_t x)           { void* res = malloc(x); D("%p VInt::new(%d)\n", res, x);                        return res;        } // new A                   : Allocate storage
  void* operator new[](size_t x)         { void* res = malloc(x); D("%p VInt::new[](%d)\n", res, x);                      return res;        } // new A[n]                : Allocate storage (array)
  void  operator delete(void* p)         { D("%p VInt::delete\n",              p);                  free(p);                                 } // delete a                : Deallocate storage
  void  operator delete[](void* p)       { D("%p VInt::delete[]\n",            p);                  free(p);                                 } // delete[] a              : Deallocate storage (array)

public:
#ifdef _DEBUG
  static void D(const char* fmt, ...)
  {
    static const int BUF_SIZE = 4096;
    char  buf[BUF_SIZE];
    va_list args;
    va_start(args, fmt);
    #ifdef __STDC_WANT_SECURE_LIB__
      int len = vsprintf_s(buf, BUF_SIZE, fmt, args);
    #else
      int len = vsprintf(buf, fmt, args);
    #endif // __STDC_WANT_SECURE_LIB__
    if (len != -1) printf("%s", buf);
    va_end(args);
  }
#else // _DEBUG
  static void D(const char* fmt, ...)
  {
    (void)fmt; // Q_UNUSED(fmt);
  }
#endif // _DEBUG
};

#endif // __V_INT_H__
