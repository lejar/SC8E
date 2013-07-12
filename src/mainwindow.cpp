#include "mainwindow.h"

MainWindow::MainWindow(std::string filename, QWidget* parent) :
	QMainWindow(parent),
	ui(new Ui_MainWindow)
{
	ui->setupUi(this);

	connect(ui->actionClose, SIGNAL(triggered()), SLOT(Exit()));

	show();

	if(!filename.empty()) ui->emulator->loadFile(filename);
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::Exit() {
	qApp->quit();
}
