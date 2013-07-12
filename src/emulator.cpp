#include <string>

#include <QMainWindow>

#include "chip8.h"
#include "qsfmlcanvas.h"

int main(int argc, char* argv[])
{
  // parse arguments
  std::string filename;
  if (argc > 1) {
    filename = argv[1];
  } else {
    std::cerr << "No filename given!" << std::endl;
    return 1;
  }

  QApplication App(argc, argv);

  QMainWindow* MainFrame = new QMainWindow;
  MainFrame->setWindowTitle("SC8E");
  MainFrame->resize(640, 345);
  MainFrame->show();

  EmulatorCanvas* SFMLView = new EmulatorCanvas(MainFrame, QPoint(0, 25), QSize(640, 320), filename);
  SFMLView->show();

  return App.exec();
}
