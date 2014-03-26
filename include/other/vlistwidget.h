#ifndef VLISTWIDGET_H
#define VLISTWIDGET_H

#ifdef QT_GUI_LIB

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <VObject>

// ----------------------------------------------------------------------------
// VListWidgetAccessible
// ----------------------------------------------------------------------------
class VListWidgetAccessible
{
public:
  virtual void  widgetClear()                                                             = 0;
  virtual int   widgetCount()                                                             = 0;
  virtual void* widgetAt(int i)                                                           = 0;
  virtual void  widgetPushBack(void* item)                                                = 0;
  virtual void* widgetCeateItem()                                                         = 0;
  virtual void  widgetItemIntoTreeWidgetItem(void* item, QTreeWidgetItem* treeWidgetItem) = 0;
  virtual void  widgetTreeWidgetItemIntoItem(QTreeWidgetItem* treeWidgetItem, void* item) = 0;
};

// ----------------------------------------------------------------------------
// VListWidget
// ----------------------------------------------------------------------------
namespace Ui {
  class VListWidget;
}

class VListWidget : public QWidget
{
  Q_OBJECT

protected:
  VListWidgetAccessible* accessible;

public:
  explicit VListWidget(QWidget *parent, VListWidgetAccessible* accessible);
  ~VListWidget();

public:
  void itemsIntoTreeWidget();
  void treeWidgetIntoItems();

protected slots:
  void __on_pbAdd_clicked();
  void __on_pbDel_clicked();

public:
  Ui::VListWidget *ui;
};
#else // QT_GUI_LIB
class VListWidgetAccessible {};
#endif // QT_GUI_LIB

#endif // VLISTWIDGET_H
