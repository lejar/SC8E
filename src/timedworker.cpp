#include "timedworker.h"

#include <QElapsedTimer>

TimedWorker::TimedWorker(unsigned int freq) : paused(false)
{
  setFrequency(freq);
}

void TimedWorker::run()
{
  QElapsedTimer timer;
  qint64 elapsed;
  while (true) {
    // do cpu cycles
    if (!paused) {
      timer.start();

      tick();
      elapsed = timer.nsecsElapsed() / 1000;
    }

    if (elapsed < usecPerFrame)
      usleep(usecPerFrame - elapsed);
  }
}

void TimedWorker::setFrequency(unsigned int freq)
{
  frequency = freq;
  usecPerFrame = 1000000 / freq;
}
