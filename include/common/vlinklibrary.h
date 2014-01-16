// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_LINK_LIBRARY_H__
#define __V_LINK_LIBRARY_H__

// ----------------------------------------------------------------------------
// Link Library
// ----------------------------------------------------------------------------
#ifdef _MSC_VER
  #ifdef _DEBUG
    #ifdef QT_GUI_LIB
      #pragma comment(lib, "vdreamd_gui.lib")
    #else
      #pragma comment(lib, "vdreamd.lib")
    #endif // QT_GUI_LIB
  #else
    #ifdef QT_GUI_LIB
      #pragma comment(lib, "vdream_gui.lib")
    #else
      #pragma comment(lib, "vdream.lib")
    #endif // QT_GUI_LIB
  #endif // _DEBUG
#endif // _MSC_VER

#endif // __V_LINK_LIBRARY_H__
