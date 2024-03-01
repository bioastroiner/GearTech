#ifndef _TIMER_H_
#define _TIMER_H_
#define TIMER_START timer_start();
#define TIMER_END(message)                \
    float time_timer_temp = timer_stop(); \
    if (time_timer_temp > 0)              \
        printf("took %.00fms %s.\n", time_timer_temp, message);
float timeInMillies();
float timeInSeconds();
float timeInSecondsI();
float getDeltaTime();
float timer_stop();
void timer_start();
void tick();
extern float deltaTime;
#endif