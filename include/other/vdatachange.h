// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_DATA_CHANGE_H__
#define __V_DATA_CHANGE_H__

#include <VObjectWidget>
#include <QRegExp>

// ----------------------------------------------------------------------------
// VDataChangeItem
// ----------------------------------------------------------------------------
class VDataChangeItem : public VXmlable
{
  friend class VDataChange;

public:
  VDataChangeItem();

public:
  bool                   enabled;
  bool                   log;
  QString                pattern;
  QRegExp::PatternSyntax syntax;
  Qt::CaseSensitivity    cs;
  bool                   minimal;
  QByteArray             replace;

public:
  QRegExp rx;
  bool prepare(VError& error);
  bool change(QByteArray& ba);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);
};

// ----------------------------------------------------------------------------
// VDataChange
// ----------------------------------------------------------------------------
class VDataChange : public QList<VDataChangeItem>, public VXmlable, public VOptionable
{
public:
  VDataChange();
  virtual ~VDataChange();

public:
  bool prepare(VError& error);
  bool change(QByteArray& ba);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
  virtual void optionAddWidget(QLayout* layout);
  virtual void optionSaveDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#endif // __V_DATA_CHANGE_H__
