#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <QFocusEvent>
#include <QPaintEngine>
#include <QTimer>
#include <QWidget>
#include <SFML/Graphics.hpp>

class QSFMLCanvas : public QWidget, public sf::RenderWindow
{
public:
  QSFMLCanvas(QWidget*, const QPoint&, const QSize&, unsigned int = 5);
  virtual ~QSFMLCanvas();

private:
  virtual void OnInit();
  virtual void OnUpdate();

  void showEvent(QShowEvent*);
  void paintEvent(QPaintEvent*);
  QPaintEngine* paintEngine() const;

  virtual void focusInEvent( QFocusEvent* );
  virtual void focusOutEvent( QFocusEvent* );

  QTimer myTimer;
  bool myInitialized;

protected:
  bool focus;
};

#endif /* QSFMLCANVAS_H */
