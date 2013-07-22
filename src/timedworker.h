#ifndef TIMEDWORKER_H
#define TIMEDWORKER_H

#include <QThread>

class TimedWorker : public QThread
{
  Q_OBJECT
public:
  TimedWorker(unsigned int frequency);

  void run();
  void setFrequency(unsigned int freq);

  bool paused;

protected:
  virtual void tick() = 0;

private:
  unsigned int frequency;
  unsigned int usecPerFrame;
};

#endif
