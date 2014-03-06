#include <VNetClient>

// ----------------------------------------------------------------------------
// VNetClient
// ----------------------------------------------------------------------------
VNetClient::VNetClient(void* owner) : VNet(owner)
{
  onceWriteSize = 0;
  host          = "";
  port          = 0;
  localHost     = "";
  localPort     = 0;
}

VNetClient::~VNetClient()
{
}

void VNetClient::load(VXml &xml)
{
  VNet::load(xml);

  onceWriteSize = xml.getInt("onceWriteSize", onceWriteSize);
  host          = xml.getStr("host", host);
  port          = xml.getInt("port", port);
  localHost     = xml.getStr("localHost", localHost);
  localPort     = xml.getInt("localPort", localPort);
}

void VNetClient::save(VXml &xml)
{
  VNet::save(xml);

  xml.setInt("onceWriteSize", onceWriteSize);
  xml.setStr("host", host);
  xml.setInt("port", port);
  xml.setStr("localHost", localHost);
  xml.setInt("localPort", localPort);
}

#ifdef QT_GUI_LIB
void VNetClient::optionAddWidget(QLayout* layout)
{
  VOptionable::addLineEdit(layout, "leOnceWriteSize", "Once Write Size", QString::number(onceWriteSize));
  VOptionable::addLineEdit(layout, "leHost",          "Host",       host);
  VOptionable::addLineEdit(layout, "lePort",          "Port",       QString::number(port));
  VOptionable::addLineEdit(layout, "leLocalHost",     "Local Host", localHost);
  VOptionable::addLineEdit(layout, "leLocalPort",     "Local Port", QString::number(localPort));
}

void VNetClient::optionSaveDlg(QDialog* dialog)
{
  onceWriteSize = dialog->findChild<QLineEdit*>("leOnceWriteSize")->text().toInt();
  host          = dialog->findChild<QLineEdit*>("leHost")->text();
  port          = dialog->findChild<QLineEdit*>("lePort")->text().toInt();
  localHost     = dialog->findChild<QLineEdit*>("leLocalHost")->text();
  localPort     = dialog->findChild<QLineEdit*>("leLocalPort")->text().toInt();
}
#endif // QT_GUI_LIB
