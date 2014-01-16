// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_LINUX_H__
#define __V_LINUX_H__

#ifdef linux
// ----------------------------------------------------------------------------
// Calling Conventions
// ----------------------------------------------------------------------------
#define __cdecl               __attribute__((__cdecl__))
#define __stdcall             __attribute__((__stdcall__))
// #define __declspec(dllexport) // gilgil temp 2012.08.11

// ----------------------------------------------------------------------------
// GetLastError()
// ----------------------------------------------------------------------------
#include <errno.h>
inline int GetLastError()    { return errno; }
inline int WSAGetLastError() { return errno; }
inline int closesocket(int handle) { return close(handle); }

// ----------------------------------------------------------------------------
// Other types
// ----------------------------------------------------------------------------
typedef int  BOOL;
typedef void (*HMODULE);

typedef signed char        INT8,   *PINT8;
typedef signed short       INT16,  *PINT16;
typedef signed int         INT32,  *PINT32;
typedef signed long long   INT64,  *PINT64;

typedef unsigned char      UINT8,  *PUINT8;
typedef unsigned short     UINT16, *PUINT16;
typedef unsigned int       UINT32, *PUINT32;
typedef unsigned long long UINT64, *PUINT64;

typedef unsigned long      ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR          DWORD_PTR, *PDWORD_PTR;
typedef unsigned char      BYTE,   *PBYTE;
typedef unsigned short     WORD,   *PWORD;
typedef unsigned int       DWORD,  *PDWORD;

// ----------------------------------------------------------------------------
// Macro
// ----------------------------------------------------------------------------
#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))

// ----------------------------------------------------------------------------
// WinSock
// ----------------------------------------------------------------------------
#define INVALID_SOCKET  (SOCKET)(~0)
#define SOCKET_ERROR            (-1)
typedef int SOCKET;

typedef struct sockaddr    SOCKADDR;
typedef struct in_addr     IN_ADDR;
typedef struct sockaddr_in SOCKADDR_IN;

#endif // linux

#endif // __V_LINUX_H__
