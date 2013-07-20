#include <QFileDialog>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  ui(new Ui_MainWindow)
{
  ui->setupUi(this);

  ui->actiongroupClockRate->addAction(ui->actionSetClockRate30);
  ui->actiongroupClockRate->addAction(ui->actionSetClockRate60);
  ui->actiongroupClockRate->addAction(ui->actionSetClockRate120);

  connect(ui->actionClose, SIGNAL(triggered()), SLOT(Exit()));
  connect(ui->actionOpen, SIGNAL(triggered()), SLOT(Open()));
  connect(ui->actionReload, SIGNAL(triggered()), SLOT(Reload()));
  connect(ui->actiongroupClockRate, SIGNAL(triggered(QAction*)), SLOT(FPSActionTriggered(QAction*)));
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::Exit() {
  qApp->quit();
}

void MainWindow::Open() {
  std::string fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                 "",
                                                 tr("Files (*.*)")).toStdString();
  emu()->loadFile(fileName);
}

void MainWindow::Reload() {
  emu()->reloadFile();
}

void MainWindow::FPSActionTriggered(QAction* action) {
  unsigned int freq = 60;
  if (action == ui->actionSetClockRate30 ) freq = 30;
  if (action == ui->actionSetClockRate60 ) freq = 60;
  if (action == ui->actionSetClockRate120) freq = 120;

  emu()->setClockRate(freq);
}
