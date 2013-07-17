#ifndef TIMEDWORKER_H
#define TIMEDWORKER_H

#include <QThread>

class TimedWorker : public QThread
{
  Q_OBJECT
public:
  TimedWorker(int frameRate);

  void run();
  void setFrameRate(int fps);

  bool paused;

protected:
  virtual void tick() = 0;

private:
  int frameRate;
  int usecPerFrame;
};

#endif
