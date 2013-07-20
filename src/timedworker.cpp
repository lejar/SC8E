#include "timedworker.h"

#include <QElapsedTimer>

TimedWorker::TimedWorker(int freq) : paused(false)
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

void TimedWorker::setFrequency(int freq)
{
  frequency = freq;
  usecPerFrame = 1000000ll / freq;
}
