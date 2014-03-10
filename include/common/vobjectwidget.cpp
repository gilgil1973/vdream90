#include <VObjectWidget>
#include <VDebugNew>

//
// for remove thw following warning
// This object file does not define any previously undefined public symbols,
// so it will not be used by any link operation that consumes this library
//
void vobjectwidget_foo() {}

#ifdef QT_GUI_LIB

// ----------------------------------------------------------------------------
// VOptionable
// ----------------------------------------------------------------------------
QDialog* VOptionable::optionCreateDlg(QWidget* parent)
{
  QDialog* dialog = new QDialog(parent);
  VObject* object = dynamic_cast<VObject*>(this);
  if (object != NULL)
    dialog->setWindowTitle(object->name == "" ? "Option" : object->name + " Option");
  new QGridLayout(dialog);
  return dialog;
}

void VOptionable::optionAddWidget(QLayout* layout)
{
  Q_UNUSED(layout)
  LOG_FATAL("virtual function call");
}

bool VOptionable::optionShowDlg(QDialog* dialog)
{
  addOkCancelButtons(dialog);
  int res = dialog->exec();
  return res == QDialog::Accepted;
}

void VOptionable::optionSaveDlg(QDialog* dialog)
{
  Q_UNUSED(dialog)
  LOG_FATAL("virtual function call");
}

bool VOptionable::optionDoAll(QWidget* parent)
{
  QDialog* dialog = this->optionCreateDlg(parent);
  this->optionAddWidget(dialog->layout());
  bool res = this->optionShowDlg(dialog);
  if (res)
  {
    this->optionSaveDlg(dialog);
  }
  //
  // do not delete dialog.
  // dialog is automatically deleted when parent is delete.
  //
  // delete dialog;
  return res;
}

void VOptionable::addOkCancelButtons(QDialog* dialog)
{
  QPushButton* pbOK     = new QPushButton(dialog);
  QPushButton* pbCancel = new QPushButton(dialog);
  pbOK->setDefault(true);
  pbOK->setText("OK");
  pbCancel->setText("Cancel");
  VObject::connect(pbOK, SIGNAL(clicked()), dialog, SLOT(accept()));
  VObject::connect(pbCancel, SIGNAL(clicked()), dialog, SLOT(close()));

  QGridLayout* buttonsLayout = new QGridLayout;
  buttonsLayout->addWidget(pbOK,     0, 0);
  buttonsLayout->addWidget(pbCancel, 0, 1);
  dialog->layout()->addItem(buttonsLayout);
}

QLineEdit* VOptionable::addLineEdit(QLayout* layout, QString objectName, QString text, QString value)
{
  QWidget* parentWidget = layout->parentWidget();
  if (parentWidget == NULL)
  {
    LOG_FATAL("parentWidget is null(%s)", qPrintable(objectName));
    return NULL;
  }
  if (parentWidget->findChild<QObject*>(objectName) != NULL)
  {
    LOG_FATAL("parentWidget->findChild(%s) is not null", qPrintable(objectName));
    return NULL;
  }

  QLabel*    label    = new QLabel(parentWidget);
  QLineEdit* lineEdit = new QLineEdit(parentWidget);

  label->setText(text);
  lineEdit->setObjectName(objectName);
  lineEdit->setText(value);

  QGridLayout* myLayout = new QGridLayout;
  myLayout->setHorizontalSpacing(8);
  myLayout->addWidget(label,    0, 0);
  myLayout->addWidget(lineEdit, 0, 1);
  layout->addItem(myLayout);

  return lineEdit;
}

QCheckBox* VOptionable::addCheckBox(QLayout* layout, QString objectName, QString text, bool value)
{
  QWidget* parentWidget = layout->parentWidget();
  if (parentWidget == NULL)
  {
    LOG_FATAL("parentWidget is null(%s)", qPrintable(objectName));
    return NULL;
  }
  if (parentWidget->findChild<QObject*>(objectName) != NULL)
  {
    LOG_FATAL("parentWidget->findChild(%s) is not null", qPrintable(objectName));
    return NULL;
  }

  QCheckBox* checkBox = new QCheckBox(parentWidget);

  checkBox->setObjectName(objectName);
  checkBox->setText(text);
  checkBox->setCheckState(value ? Qt::Checked : Qt::Unchecked);

  layout->addWidget(checkBox);

  return checkBox;
}

QComboBox* VOptionable::addComboBox(QLayout *layout, QString objectName, QString text, QStringList strList, int index, QString value)
{
  QWidget* parentWidget = layout->parentWidget();
  if (parentWidget == NULL)
  {
    LOG_FATAL("parentWidget is null(%s)", qPrintable(objectName));
    return NULL;
  }
  if (parentWidget->findChild<QObject*>(objectName) != NULL)
  {
    LOG_FATAL("parentWidget->findChild(%s) is not null", qPrintable(objectName));
    return NULL;
  }
  QLabel*    label    = new QLabel(parentWidget);
  QComboBox* comboBox = new QComboBox(parentWidget);

  label->setText(text);
  comboBox->setObjectName(objectName);
  foreach (QString text, strList)
  {
    comboBox->addItem(text);
  }
  comboBox->setCurrentIndex(index);
  comboBox->setCurrentText(value);

  QGridLayout* myLayout = new QGridLayout;
  myLayout->setHorizontalSpacing(8);
  myLayout->addWidget(label,    0, 0);
  myLayout->addWidget(comboBox, 0, 1);
  layout->addItem(myLayout);

  return comboBox;
}

#endif // QT_GUI_LIB
