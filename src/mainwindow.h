#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "emulatorcanvas.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(std::string filename, QWidget* parent = 0);
	~MainWindow();

private slots:
	void Exit();

private:
	Ui_MainWindow* ui;
	EmulatorCanvas* emu;
};

#endif // MAINWINDOW_H
