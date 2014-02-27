#include <VNetClient>

// ----------------------------------------------------------------------------
// VNetClient
// ----------------------------------------------------------------------------
VNetClient::VNetClient(void* owner) : VNet(owner)
{
  onceWriteSize = 0;
}

VNetClient::~VNetClient()
{
}

void VNetClient::load(VXml &xml)
{
  VNet::load(xml);

  onceWriteSize = xml.getInt("onceWriteSize", onceWriteSize);
}

void VNetClient::save(VXml &xml)
{
  VNet::save(xml);

  if (onceWriteSize != 0) xml.setInt("onceWriteSize", onceWriteSize);
}

#ifdef QT_GUI_LIB
void VNetClient::optionAddWidget(QLayout* layout)
{
  VOptionable::addLineEdit(layout, "leOnceWriteSize", "Once Write Size", QString::number(onceWriteSize));
}

void VNetClient::optionSaveDlg(QDialog* dialog)
{
  onceWriteSize = dialog->findChild<QLineEdit*>("leOnceWriteSize")->text().toInt();
}
#endif // QT_GUI_LIB
