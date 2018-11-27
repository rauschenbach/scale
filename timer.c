#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

long TimerCount = 0;
void (*TimerCallFunc) ();

void dummytimerfunc()
{
}


void timerfunc(int sig)
{
    signal(SIGALRM, timerfunc);
    TimerCount++;
//    TimerCallFunc();
}

void init_timer(int sec, int usec)
{
    struct itimerval iv;

    iv.it_value.tv_sec = iv.it_interval.tv_sec = sec;
    iv.it_value.tv_usec = iv.it_interval.tv_usec = usec;

    TimerCallFunc = dummytimerfunc;
    setitimer(ITIMER_REAL, &iv, NULL);
    signal(SIGALRM, timerfunc);
}

void clear_timer()
{
    struct itimerval iv;

    iv.it_value.tv_sec = iv.it_interval.tv_sec = 0;
    iv.it_value.tv_usec = iv.it_interval.tv_usec = 0;

    signal(SIGALRM, SIG_DFL);
    setitimer(ITIMER_REAL, &iv, NULL);
}
