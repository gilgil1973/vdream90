// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef _V_SSL_CLIENT_H__
#define _V_SSL_CLIENT_H__

#include <VTcpClient>
#include <VSslSession>

// ----------------------------------------------------------------------------
// VSslClient
// ----------------------------------------------------------------------------
class VSslClient : public VTcpClient
{
public:
  VSslClient(void* owner = NULL);
  virtual ~VSslClient();

protected:
  virtual bool doOpen();
  virtual bool doClose();
  virtual int  doRead(char* buf, int size);
  virtual int  doWrite(char* buf, int size);

public:
  VSslSession*   sslSession; // read only

public:
  VSslMethodType methodType;

protected:
  SSL_METHOD*   m_meth;
  SSL_CTX*      m_ctx;

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public: // for VOptionable
  virtual void addOptionWidget(QLayout* layout);
  virtual void saveOptionDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // _V_SSL_CLIENT_H__

