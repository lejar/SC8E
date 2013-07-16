#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "emulatorcanvas.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget* parent = nullptr);
  ~MainWindow();

  EmulatorCanvas* emu() { return ui->emulator; }

private slots:
  void Exit();
  void Open();
  void Reload();
  void FPSActionTriggered(QAction*);

private:
  Ui_MainWindow* ui;
};

#endif // MAINWINDOW_H
