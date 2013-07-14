#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui_MainWindow)
{
  ui->setupUi(this);

  ui->actiongroupFPS->addAction(ui->actionSetFPS30);
  ui->actiongroupFPS->addAction(ui->actionSetFPS60);
  ui->actiongroupFPS->addAction(ui->actionSetFPS120);

  connect(ui->actionClose, SIGNAL(triggered()), SLOT(Exit()));
  connect(ui->actiongroupFPS, SIGNAL(triggered(QAction*)), SLOT(FPSActionTriggered(QAction*)));
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::Exit() {
  qApp->quit();
}

void MainWindow::FPSActionTriggered(QAction* action) {
  unsigned int fps = 60;
  if (action == ui->actionSetFPS30 ) fps = 30;
  if (action == ui->actionSetFPS60 ) fps = 60;
  if (action == ui->actionSetFPS120) fps = 120;

  emu()->setFrameRate(fps);
}
