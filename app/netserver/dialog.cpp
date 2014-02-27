#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::Dialog)
{
  ui->setupUi(this);

  move(0, 0);
  resize(640, 480);
  setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint);

  loadFromDefaultDoc("MainWindow");
  setControl();
}

Dialog::~Dialog()
{
  this->saveToDefaultDoc("MainWindow");
  delete ui;
}

void Dialog::setControl()
{

}

void Dialog::load(VXml xml)
{
  QRect rect = geometry();
  rect.setLeft  ((xml.getInt("left",   0)));
  rect.setTop   ((xml.getInt("top",    0)));
  rect.setWidth ((xml.getInt("width",  640)));
  rect.setHeight((xml.getInt("height", 480)));
  setGeometry(rect);
}

void Dialog::save(VXml xml)
{
  QRect rect = geometry();
  xml.setInt("left",   rect.left());
  xml.setInt("top",    rect.top());
  xml.setInt("width",  rect.width());
  xml.setInt("height", rect.height());
}
