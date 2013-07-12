#ifndef EMULATORMENU_H
#define EMULATORMENU_H

#include <QMenuBar>

#include "ui_menu.h"
#include "chip8.h"

class EmulatorMenu : public QMenuBar
{
  Q_OBJECT

public:
  EmulatorMenu(QWidget *parent);

private slots:
  void on_actionClose_triggered();

private:
  Ui_MenuBar ui;
  chip8* emu;
};

#endif /* EMULATORMENU_H */
