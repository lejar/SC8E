#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui_MainWindow)
{
  ui->setupUi(this);

  connect(ui->actionClose, SIGNAL(triggered()), SLOT(Exit()));

}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::Exit() {
  qApp->quit();
}
