#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "preempt.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
#define SEC 0.01
#define USEC 10000

sigset_t mask;

//block signal
void preempt_disable(void)
{
	sigprocmask(SIG_BLOCK, &mask, NULL);
}

//unblock signal
void preempt_enable(void)
{
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

//signal handler function
void handler(int signal){
	uthread_yield();
}

void preempt_start(void)
{
	//set up timer and signal action
	struct sigaction sa;
	struct itimerval it;
	
	//initialize signal action values
	sa.sa_flags = 0;
	sigemptyset(&mask);
        sigaddset(&mask, SIGVTALRM);
	sa.sa_handler = handler;

	//set timer value
	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = USEC;
	it.it_value = it.it_interval;

	//finish timer and signal action
	sigaction(SIGVTALRM, &sa, NULL);
	setitimer(ITIMER_VIRTUAL, &it, NULL);
}

