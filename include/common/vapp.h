// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_APP_H__
#define __V_APP_H__

#include <VCommon>
#include <VXmlDoc>
#include <VLogFactory>

// ----------------------------------------------------------------------------
// VApp
// ----------------------------------------------------------------------------
class VApp : private VNonCopyable
{
private: // singleton
  VApp();
  virtual ~VApp();

protected:
  int    m_argc;
  char** m_argv;

public:
  void   setArguments(int argc, char* argv[]) { m_argc = argc; m_argv = argv; }
  int    argc()                               { return m_argc;                }
  char** argv()                               { return m_argv;                }

public:
  static QString fileName();
  static QString _filePath();
  static QString currentPath();
  static bool    setCurrentPath(QString path);

public:
#ifdef QT_GUI_LIB
  static void initialize(bool path = true, bool xml = true, QString uri = "udp;file://log/");
#else // QT_GUI_LIB
  static void initialize(bool path = true, bool xml = true, QString uri = "stdout;udp;file://log/");
#endif // QT_GUI_LIB

  static void finalize(bool xml = true);

public:
  static VApp& instance()
  {
    static VApp app;
    return app;
  }
};

#endif // __V_APP_H__
