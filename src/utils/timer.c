#include "utils/timer.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>

float timer;
float timer_lastFrame;
float deltaTime;

void tick()
{
    timer_lastFrame = timer;
    timer = glfwGetTime();
    deltaTime = timer - timer_lastFrame;
}
float timeInMillies()
{
    return timeInSeconds() * 1000;
}
float timeInSeconds()
{
    return glfwGetTime();
}
float timeInSecondsI()
{
    return ceilf(timeInSeconds());
}
float getDeltaTime()
{
    return deltaTime;
}