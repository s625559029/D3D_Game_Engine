#ifndef _TIMER_
#define _TIMER_

extern double countsPerSecond;
extern __int64 CounterStart;

extern int frameCount;
extern int fps;

extern __int64 frameTimeOld;
extern double frameTime;

void StartTimer();
double GetTime();
double GetFrameTime();

#endif // !_TIMER_

