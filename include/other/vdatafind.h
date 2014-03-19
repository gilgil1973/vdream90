// ----------------------------------------------------------------------------
//
// VDream Component Suite version 9.0
//
// http://www.gilgil.net
//
// Copyright (c) Gilbert Lee All rights reserved
//
// ----------------------------------------------------------------------------

#ifndef __V_DATA_FIND_H__
#define __V_DATA_FIND_H__

#include <VObjectWidget>
#include <QRegExp>
#ifdef QT_GUI_LIB
#include <QTreeWidget>
#include <QHeaderView>
#endif // QT_GUI_LIB

// ----------------------------------------------------------------------------
// VRegExp
// ----------------------------------------------------------------------------
class VRegExp : public VXmlable
{
public:
  VRegExp();

public:
  QString                pattern;
  QRegExp::PatternSyntax syntax;
  Qt::CaseSensitivity    cs;
  bool                   minimal;

public:
  QRegExp rx;
  bool prepare(VError& error);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public:
  static const int PATTERN_IDX   = 0;
  static const int SYNTAX_IDX    = 1;
  static const int CS_IDX        = 2;
  static const int MINIMAL_IDX   = 3;

  static void initialize(QTreeWidget* treeWidget);
#endif // QT_GUI_LIB
};
#ifdef QT_GUI_LIB
void operator << (QTreeWidgetItem& treeWidgetItem, VRegExp& regexp);
void operator << (VRegExp& regexp, QTreeWidgetItem& treeWidgetItem);
#endif // QT_GUI_LIB

// ----------------------------------------------------------------------------
// VDataFindItem
// ----------------------------------------------------------------------------
class VDataFindItem : public VRegExp
{
public:
  VDataFindItem();

public:
  bool enabled;
  // bool log; // gilgil temp 2014.03.19

public:
  int find(QByteArray& ba, int offset = 0);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public:
  static const int ENABLED_IDX = 4;

  static void initialize(QTreeWidget* treeWidget);
#endif // QT_GUI_LIB
};

#ifdef QT_GUI_LIB
void operator << (QTreeWidgetItem& treeWidgetItem, VDataFindItem& item);
void operator << (VDataFindItem& item, QTreeWidgetItem& treeWidgetItem);
#endif // QT_GUI_LIB

// ----------------------------------------------------------------------------
// VDataFind
// ----------------------------------------------------------------------------
class VDataFind : public QList<VDataFindItem>, public VXmlable, public VOptionable
{
public:
  VDataFind();
  virtual ~VDataFind();

public:
  bool prepare(VError& error);
  bool find(QByteArray& ba);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public:
  virtual void optionAddWidget(QLayout* layout);
  virtual void optionSaveDlg(QDialog* dialog);
#endif // QT_GUI_LIB
};

#ifdef QT_GUI_LIB
void operator << (QTreeWidget& treeWidget, VDataFind& dataFind);
void operator << (VDataFind& dataFind, QTreeWidget& treeWidget);
#endif // QT_GUI_LIB

#endif // __V_DATA_CHANGE_H__
