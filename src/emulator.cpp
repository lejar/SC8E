#include <string>

#include <QMainWindow>

#include "chip8.h"
#include "mainwindow.h"

int main(int argc, char* argv[])
{
  // parse arguments
  std::string filename;
  if (argc > 1) {
    filename = argv[1];
  }

  QApplication App(argc, argv);
  MainWindow w(filename);

  return App.exec();
}
