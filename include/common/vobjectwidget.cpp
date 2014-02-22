#include <VObjectWidget>

#ifdef QT_GUI_LIB

// ----------------------------------------------------------------------------
// VOptionable
// ----------------------------------------------------------------------------
QDialog* VOptionable::createOptionDlg()
{
  QDialog* dialog = new QDialog(NULL);
  new QGridLayout(dialog);
  return dialog;
}

void VOptionable::addOptionWidget(QLayout* layout)
{
  Q_UNUSED(layout)
  LOG_FATAL("virtual function call");
}

bool VOptionable::showOptionDlg(QDialog* dialog)
{
  addOkCancelButtons(dialog);
  int res = dialog->exec();
  return res == QDialog::Accepted;
}

void VOptionable::saveOptionDlg(QDialog* dialog)
{
  Q_UNUSED(dialog)
  LOG_FATAL("virtual function call");
}

void VOptionable::addOkCancelButtons(QDialog* dialog)
{
  QPushButton* pbOK     = new QPushButton(dialog);
  QPushButton* pbCancel = new QPushButton(dialog);
  pbOK->setDefault(true);
  pbOK->setText("OK");
  pbCancel->setText("Cancel");
  QObject::connect(pbOK, SIGNAL(clicked()), dialog, SLOT(accept()));
  QObject::connect(pbCancel, SIGNAL(clicked()), dialog, SLOT(close()));

  QGridLayout* buttonsLayout = new QGridLayout;
  buttonsLayout->addWidget(pbOK,     0, 0);
  buttonsLayout->addWidget(pbCancel, 0, 1);
  dialog->layout()->addItem(buttonsLayout);
}

QLineEdit* VOptionable::addLineEdit(QLayout* layout, QString objectName, QString text, QString value)
{
  if (layout->findChild<QObject*>(objectName) != NULL)
  {
    LOG_FATAL("layout->findChild(%s) is not null", qPrintable(objectName));
    return NULL;
  }

  QLabel*    label    = new QLabel(layout->parentWidget());
  QLineEdit* lineEdit = new QLineEdit(layout->parentWidget());

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
  if (layout->findChild<QObject*>(objectName) != NULL)
  {
    LOG_FATAL("layout->findChild(%s) is not null", qPrintable(objectName));
    return NULL;
  }

  QCheckBox* checkBox = new QCheckBox(layout->parentWidget());

  checkBox->setObjectName(objectName);
  checkBox->setText(text);
  checkBox->setCheckState(value ? Qt::Checked : Qt::Unchecked);

  layout->addWidget(checkBox);

  return checkBox;
}

QComboBox* VOptionable::addComboBox(QLayout *layout, QString objectName, QString text, QStringList strList, int index, QString value)
{
  if (layout->findChild<QObject*>(objectName) != NULL)
  {
    LOG_FATAL("layout->findChild(%s) is not null", qPrintable(objectName));
    return NULL;
  }

  QLabel*    label    = new QLabel(layout->parentWidget());
  QComboBox* comboBox = new QComboBox(layout->parentWidget());

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

// ----------------------------------------------------------------------------
// VShowStatus
// ----------------------------------------------------------------------------
bool VShowStatus::showStatus(QDialog* parent)
{
  parent->exec();
  return true;
}

#endif // QT_GUI_LIB
