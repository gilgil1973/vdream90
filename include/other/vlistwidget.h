#ifndef VLISTWIDGET_H
#define VLISTWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <VObject>

// ----------------------------------------------------------------------------
// VListWidget
// ----------------------------------------------------------------------------
namespace Ui {
  class VListWidget;
}

class VListWidget : public QWidget
{
  Q_OBJECT

public:
  explicit VListWidget(QWidget *parent = 0);
  ~VListWidget();

public:
  Ui::VListWidget *ui;
};

// ----------------------------------------------------------------------------
// VListItemtWidgetAccessible
// ----------------------------------------------------------------------------
class VListItemtWidgetAccessible
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
// VLisItemtWidget
// ----------------------------------------------------------------------------
class VLisItemtWidget : public VListWidget
{
  Q_OBJECT

protected:
  VListItemtWidgetAccessible* accessible;

public:
  explicit VLisItemtWidget(QWidget *parent, VListItemtWidgetAccessible* accessible);
  virtual ~VLisItemtWidget();

public:
  void itemsIntoTreeWidget();
  void treeWidgetIntoItems();

protected slots:
  void __on_pbAdd_clicked();
  void __on_pbDel_clicked();
};

#endif // VLISTWIDGET_H
