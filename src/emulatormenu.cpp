#include "emulatormenu.h"
#include "emulatorcanvas.h"

EmulatorMenu::EmulatorMenu(QWidget *parent) :
QMenuBar(parent)
{
  ui.setupUi(this);
  this->emu = &(dynamic_cast<EmulatorCanvas*>(parent)->emu);
}

void EmulatorMenu::on_actionClose_triggered()
{
  QWidget::parentWidget()->close();
}
