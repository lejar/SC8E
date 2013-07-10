#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <SFML/Graphics.hpp>
#include <Qt/qwidget.h>
#include <Qt/qtimer.h>
#include <Qt/qframe.h>
#include <Qt/qapplication.h>

class QSFMLCanvas : public QWidget, public sf::RenderWindow
{
public:
  QSFMLCanvas(QWidget*, const QPoint&, const QSize&, unsigned int = 0);
  virtual ~QSFMLCanvas();

private:
  virtual void OnInit();
  virtual void OnUpdate();

  void showEvent(QShowEvent*);
  void paintEvent(QPaintEvent*);
  QPaintEngine* paintEngine() const;

  QTimer myTimer;
  bool myInitialized;
};

#endif /* QSFMLCANVAS_H */
