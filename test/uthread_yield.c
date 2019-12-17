
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <uthread.h>

int thread3(void* arg)
{
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 0;
}

int thread2(void* arg)
{
	uthread_create(thread3, NULL);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return 0;
}

int thread1(void* arg)
{
	uthread_create(thread2, NULL);
	uthread_yield();
	printf("thread%d\n", uthread_self());
	uthread_yield();
	return 0;
}


uthread_t tid[2];
int thread5(void* arg){
    int ret;
    printf("thread5\n");
    uthread_join(tid[0],&ret);
    printf("thread4 return %d\n",ret);
    return 2;
}

int thread4(void* arg){
    tid[1] = uthread_create(thread5,NULL);
    printf("thread4\n");
    return 1;
}


int main(void)
{
    uthread_join(uthread_create(thread1,NULL),NULL);

	int ret;
    tid[0] = uthread_create(thread4,NULL);
    uthread_yield();
    uthread_join(tid[1],&ret);
    printf("thread5 returned %d\n", ret);
	return 0;
}
