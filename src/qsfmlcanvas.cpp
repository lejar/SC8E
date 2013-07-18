#include "qsfmlcanvas.h"

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#endif

QSFMLCanvas::QSFMLCanvas(QWidget* Parent) :
  QWidget(Parent),
  focus(true),
  myInitialized(false)
{
  // set up direct rendering
  setAttribute(Qt::WA_PaintOnScreen);
  setAttribute(Qt::WA_OpaquePaintEvent);
  setAttribute(Qt::WA_NoSystemBackground);

  // enable keyboard events to be received
  setFocusPolicy(Qt::StrongFocus);
}

QSFMLCanvas::~QSFMLCanvas()
{
  render.close();
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
  render.create(winId());

  // initialize some stuff after construction
  OnInit();

  // set frame rate with timer
  repaintTimer.setInterval(1000 / 60);
  connect(&repaintTimer, SIGNAL(timeout()), this, SLOT(repaint()));
  repaintTimer.start();

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
  render.setActive(true);
  // update sfml
  OnRepaint();

  // display screen
  render.display();
}

void QSFMLCanvas::OnInit()
{
}

void QSFMLCanvas::OnRepaint()
{
}

void QSFMLCanvas::focusInEvent(QFocusEvent*)
{
  focus = true;
}
void QSFMLCanvas::focusOutEvent(QFocusEvent*)
{
  focus = false;
}
