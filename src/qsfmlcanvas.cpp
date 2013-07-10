#include "qsfmlcanvas.h"

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#endif

QSFMLCanvas::QSFMLCanvas(QWidget* Parent, const QPoint& Position, const QSize& wSize, unsigned int FrameTime) :
QWidget(Parent),
myInitialized(false),
focus(true)
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

void QSFMLCanvas::showEvent(QShowEvent*)
{
  if (myInitialized) return;

  // with X11, commands need to be flushed to be sent to the server so SFML will update
#ifdef Q_WS_X11
  XFlush(QX11Info::display());
#endif

  // SFML window with widget handle
  // the initialization is deferred to the showEvent handler, so that the
  // context is created in the correct thread
  this->sf::RenderWindow::create(winId());
  this->sf::RenderTarget::initialize();

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
  return nullptr;
}

void QSFMLCanvas::paintEvent(QPaintEvent*)
{
  // ensure the SFML window is active for the current thread
  this->setActive(true);
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

void QSFMLCanvas::focusInEvent( QFocusEvent* )
{
  focus = true;
}
void QSFMLCanvas::focusOutEvent( QFocusEvent* )
{
  focus = false;
}
