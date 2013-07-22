#ifndef TIMEDWORKER_H
#define TIMEDWORKER_H

#include <QThread>

class TimedWorker : public QThread
{
  Q_OBJECT
public:
  TimedWorker(int frequency);

  void run();
  void setFrequency(int freq);

  bool paused;

protected:
  virtual void tick() = 0;

private:
  int frequency;
  int usecPerFrame;
};

#endif
