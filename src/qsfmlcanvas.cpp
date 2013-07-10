#ifdef Q_WS_X11
  #include <Qt/qx11info_x11.h>
  #include <X11/Xlib.h>
#endif

#include "qsfmlcanvas.h"

QSFMLCanvas::QSFMLCanvas(QWidget* Parent, const QPoint& Position, const QSize& wSize, unsigned int FrameTime) :
QWidget(Parent),
myInitialized(false)
{
  // set up direct rendering
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_OpaquePaintEvent);
  setAttribute(Qt::WA_NoSystemBackground);

  // enable keyboard events to be received
  setFocusPolicy(Qt::StrongFocus);

  // set up window variables
  move(Position);
  resize(wSize);

  // set up timer
  myTimer.setInterval(FrameTime);
}

QSFMLCanvas::~QSFMLCanvas()
{
}

#include <iostream>
void QSFMLCanvas::showEvent(QShowEvent*)
{
  if (myInitialized) return;

  // with X11, commands need to be flushed to be sent to the server so SFML will update
  #ifdef Q_WS_X11
    //XFlush(QX11Info::display());
  #endif

  // SFML window with widget handle
  sf::RenderWindow::create(winId());

  // initialize some stuff after construction
  OnInit();

  // set frame rate with timer
  connect(&myTimer, SIGNAL(timeout()), this, SLOT(repaint()));
  myTimer.start();

  myInitialized = true;
}

QPaintEngine* QSFMLCanvas::paintEngine() const
{
  // let Qt know we're not using its paint engine
  return 0;
}

void QSFMLCanvas::paintEvent(QPaintEvent*)
{
  // update sfml
  OnUpdate();

  // display screen
  this->display();
}

void QSFMLCanvas::OnInit()
{
}

void QSFMLCanvas::OnUpdate()
{
}
