#include <VNetServer>

// ----------------------------------------------------------------------------
// VNetServer
// ----------------------------------------------------------------------------
VNetServer::VNetServer(void* owner) : VNet(owner)
{
  onceWriteSize = 0;
}

VNetServer::~VNetServer()
{
}

void VNetServer::load(VXml &xml)
{
  VNet::load(xml);

  onceWriteSize = xml.getInt("onceWriteSize", onceWriteSize);
}

void VNetServer::save(VXml &xml)
{
  VNet::save(xml);

  if (onceWriteSize != 0) xml.setInt("onceWriteSize", onceWriteSize);
}

#ifdef QT_GUI_LIB
void VNetServer::optionAddWidget(QLayout* layout)
{
  VOptionable::addLineEdit(layout, "leOnceWriteSize", "Once Write Size", QString::number(onceWriteSize));
}

void VNetServer::optionSaveDlg(QDialog* dialog)
{
  onceWriteSize = dialog->findChild<QLineEdit*>("leOnceWriteSize")->text().toInt();
}
#endif // QT_GUI_LIB
