#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>

volatile int flag = 0;

int thread1(void* arg)
{
    flag = 1;
	printf("reach thread%d\n", uthread_self());
	return 0;
}

int main(void){

    uthread_create(thread1, NULL);
    while(!flag){}
    printf("preemt test passed\n");
    return 0;
}
