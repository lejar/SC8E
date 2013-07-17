#include "timedworker.h"

#include <QElapsedTimer>

TimedWorker::TimedWorker(int frameRate) : paused(false)
{
  setFrameRate(frameRate);
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

void TimedWorker::setFrameRate(int fps)
{
  frameRate = fps;
  usecPerFrame = 1000000ll / frameRate;
}
