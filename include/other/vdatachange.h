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

#include <VDataFind>

// ----------------------------------------------------------------------------
// VDataChangeItem
// ----------------------------------------------------------------------------
class VDataChangeItem : public VRegExp
{
public:
  VDataChangeItem();

public:
  bool       enabled;
  bool       log;
  QByteArray replace;

public:
  int change(QByteArray& ba, int offset = 0);

public:
  virtual void load(VXml xml);
  virtual void save(VXml xml);

#ifdef QT_GUI_LIB
public:
  static const int ENABLED_IDX = 4;
  static const int LOG_IDX     = 5;
  static const int REPLACE_IDX = 6;

public:
  static void initialize(QTreeWidget* treeWidget);
#endif // QT_GUI_LIB
};

#ifdef QT_GUI_LIB
void operator << (QTreeWidgetItem& treeWidgetItem, VDataChangeItem& item);
void operator << (VDataChangeItem& item, QTreeWidgetItem& treeWidgetItem);
#endif // QT_GUI_LIB

// ----------------------------------------------------------------------------
// VDataChange
// ----------------------------------------------------------------------------
class VDataChange : public QObject, public QList<VDataChangeItem>, public VXmlable, public VOptionable, public VListWidgetAccessible
{
  Q_OBJECT

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
public: // VOptionable
  virtual void optionAddWidget(QLayout* layout);
  virtual void optionSaveDlg(QDialog* dialog);

public: // VListWidgetAccessible
  virtual void  widgetClear()                                                             { clear();                                    }
  virtual int   widgetCount()                                                             { return count();                             }
  virtual void* widgetAt(int i)                                                           { return  (void*)&at(i);                      }
  virtual void  widgetPushBack(void* item)                                                { push_back(*(const VDataChangeItem*)item);   }
  virtual void* widgetCeateItem()                                                         { return new VDataChangeItem;                 }
  virtual void  widgetItemIntoTreeWidgetItem(void* item, QTreeWidgetItem* treeWidgetItem) { *treeWidgetItem << *(VDataChangeItem*)item; }
  virtual void  widgetTreeWidgetItemIntoItem(QTreeWidgetItem* treeWidgetItem, void* item) { *(VDataChangeItem*)item << *treeWidgetItem; }
#endif // QT_GUI_LIB
};

#endif // __V_DATA_CHANGE_H__
