#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

#define MAX_THREAD_NUM 2000

uthread_t GLOBAL_TID = 0; //keep track of tid and assign tid to each thread

//the queue that separate different thread by their states
queue_t READY_QUEUE;
queue_t BLOCKED_QUEUE;
queue_t ZOMBIE_QUEUE;

//only one thread is running
struct TCB* RUNNING_TCB;

// the overall inner data of a thread
struct TCB {
    uthread_t TID; // its TID
    uthread_ctx_t* context; //set of registers
    void* stack; //pointer to the top of the stack
    int retvalue; //self retvalue
    struct TCB* joined; //indicate if the thread is joined
    int* ret_data_address; // the address where retvalue should go to
};

//a function that find given tid, return result to data
static int find_pid(void *data, void *arg)
{
    uthread_t *target_TID = (uthread_t*)arg;
    
    if (((struct TCB *)data)->TID == *target_TID)
        return 1;
    
    return 0;
}

//enqueue function with preempt
void preempt_enqueue(queue_t target_queue, struct TCB* target_TCB){
    preempt_disable();
    queue_enqueue(target_queue, target_TCB);
    preempt_enable();
}

//dequeue function with preempt
void preempt_dequeue(queue_t target_queue, struct TCB* next_TCB){
    preempt_disable();
    queue_dequeue(target_queue, (void **)&next_TCB);
    RUNNING_TCB = next_TCB;
    preempt_enable();
}

//delete function with preempt
void preempt_delete(queue_t target_queue, struct TCB* target_TCB){
    preempt_disable();
    queue_delete(target_queue, target_TCB);
    preempt_enable();
}

//iteration function with preempt
void preempt_iteration(queue_t queue, queue_func_t func, void *arg, void **data){
    preempt_disable();
    queue_iterate(queue, func, arg, data);
    preempt_enable();
}

//get the next available thread from the READY queue
//and assign the new running thread
void next_active_thread(){
    struct TCB* next_TCB = NULL;
    struct TCB* current_TCB = RUNNING_TCB;

    preempt_dequeue(READY_QUEUE, next_TCB);
    uthread_ctx_switch(current_TCB->context, RUNNING_TCB->context);
}

//destory the stack of the thread the free its memory
void destroy_TCB(struct TCB* destroy_TCB){
    uthread_ctx_destroy_stack(destroy_TCB->stack);
    free(destroy_TCB->context);
    free(destroy_TCB);
}

//function that make the running thread into ready state
//and get the next available thread into running
void uthread_yield(void)
{
    preempt_enqueue(READY_QUEUE, RUNNING_TCB);
    next_active_thread();
}

//function that return the tid of current running thread
uthread_t uthread_self(void)
{
    return RUNNING_TCB->TID;
}

//initialize the value of a thread
int initialize_TCB(struct TCB* new_TCB, uthread_ctx_t* new_context, void* new_stack){
    new_TCB->TID = GLOBAL_TID;
    new_TCB->context = new_context;
    new_TCB->stack = new_stack;
    new_TCB->joined = NULL;
    new_TCB->retvalue = -1;
    new_TCB->ret_data_address = NULL;
    return 0;
}

//creating the main thread and start the preempt
int first_create(){
    READY_QUEUE = queue_create();
    BLOCKED_QUEUE = queue_create();
    ZOMBIE_QUEUE = queue_create();
    preempt_start();
    struct TCB* new_TCB = (struct TCB*)malloc(sizeof(struct TCB));
    uthread_ctx_t* new_context = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
    if(new_TCB == NULL){
        return -1;
    }
    if(getcontext(new_context)){
        return -1;
    }
    initialize_TCB(new_TCB, new_context, NULL);
    RUNNING_TCB = new_TCB;
    return 0;
}

//creating threads other than main
int uthread_create(uthread_func_t func, void *arg)
{
	//if the thread is main go the first_create
    if(!GLOBAL_TID){
        if(first_create()){
            return -1;
        }
    }
    GLOBAL_TID++;
    struct TCB* new_TCB = (struct TCB*)malloc(sizeof(struct TCB));
    uthread_ctx_t* new_context = (uthread_ctx_t*)malloc(sizeof(uthread_ctx_t));
	//get stack from context.h
    void* new_stack = uthread_ctx_alloc_stack();
    if((new_TCB == NULL) || (new_stack == NULL) || (GLOBAL_TID >= MAX_THREAD_NUM)){
        return -1;
    }
	//get the context from context.h
    if(uthread_ctx_init(new_context, new_stack, func, arg)){
        return -1;
    }
    //assign value and enqueue them into ready queue
    initialize_TCB(new_TCB, new_context, new_stack);
    preempt_enqueue(READY_QUEUE, new_TCB);
    return GLOBAL_TID;
}

//if main thread exits
void main_exit(struct TCB* main_TCB){
    free(main_TCB);
    queue_destroy(READY_QUEUE);
    queue_destroy(BLOCKED_QUEUE);
    queue_destroy(ZOMBIE_QUEUE);
}

//if any thread wants to exit
void uthread_exit(int retval)
{
	//record the return value
    RUNNING_TCB->retvalue = retval;
    struct TCB* join_TCB;
    if(RUNNING_TCB->TID == 0){
        main_exit(RUNNING_TCB);
    } else {
		//if the thread is not joined yet
        if(RUNNING_TCB->joined == NULL){
            preempt_enqueue(ZOMBIE_QUEUE, RUNNING_TCB);
        } else {
		//if the thread is joined
	    if(RUNNING_TCB->ret_data_address != NULL){
                *(RUNNING_TCB->ret_data_address) = retval;
	    }
            join_TCB = RUNNING_TCB->joined;
            preempt_enqueue(READY_QUEUE, join_TCB);
            preempt_delete(BLOCKED_QUEUE, join_TCB);
        }
		//get the next available thread
        next_active_thread();
    }    
}

//join a thread that is not zombie
int active_thread_join(struct TCB* joined_TCB, int *retval){
    if((joined_TCB)->joined != NULL){
        return -1;
    }	
    joined_TCB->joined = RUNNING_TCB;
	//record where the retval should be return back
    (joined_TCB)->ret_data_address = retval;
    preempt_enqueue(BLOCKED_QUEUE, RUNNING_TCB);
    next_active_thread();
    destroy_TCB(joined_TCB);
    return 0;
}

//join a thread that is zombie
int zombie_thread_join(struct TCB* joined_TCB, int *retval){
    joined_TCB->joined = RUNNING_TCB;
	//the thread should get out of zombie state
    preempt_delete(ZOMBIE_QUEUE, joined_TCB);
	//return the value directly
    *retval = (joined_TCB)->retvalue;
    destroy_TCB(joined_TCB);
    return 0;
}

int uthread_join(uthread_t tid, int *retval)
{
	//error check
    if(tid == 0){
        return -1;
    }
    struct TCB* joined_TCB = NULL;
    //try to find any thread that match the tid in different state queues
    //the first two indicates that's not a zombie state
    preempt_iteration(READY_QUEUE, find_pid, &tid, (void **)&joined_TCB);
    if(joined_TCB != NULL){
        return active_thread_join(joined_TCB, retval);
    }
    preempt_iteration(BLOCKED_QUEUE, find_pid, &tid, (void **)&joined_TCB);
    if(joined_TCB != NULL){
        return active_thread_join(joined_TCB, retval);
    }
	//indicate the mathch is zombie
    preempt_iteration(ZOMBIE_QUEUE, find_pid, &tid, (void **)&joined_TCB);
    if(joined_TCB != NULL){
        return zombie_thread_join(joined_TCB, retval);
    }
    return -1;
}

