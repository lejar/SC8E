#ifndef QSFMLCANVAS_H
#define QSFMLCANVAS_H

#include <QFocusEvent>
#include <QPaintEngine>
#include <QTimer>
#include <QWidget>
#include <SFML/Graphics.hpp>

class QSFMLCanvas : public QWidget
{
  Q_OBJECT
public:
  QSFMLCanvas(QWidget* = nullptr);
  virtual ~QSFMLCanvas();

protected:
  bool focus;
  sf::RenderWindow render;

private:
  virtual void OnInit();
  virtual void OnRepaint();

  void showEvent(QShowEvent*);
  void paintEvent(QPaintEvent*);
  QPaintEngine* paintEngine() const;

  virtual void focusInEvent(QFocusEvent*);
  virtual void focusOutEvent(QFocusEvent*);

  QTimer repaintTimer;
  bool myInitialized;
};

#endif /* QSFMLCANVAS_H */
