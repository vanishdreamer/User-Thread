#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"

//struct for link list
struct node {
	void* value; //the value of linklist
	struct node* next; //indicating the next node
};

//the basic structure of queue
struct queue {
	struct node* head; // the start of the queue
	struct node* tail; // the end of the queue
	int size; // the size of the queue
};

//function that create a queue
queue_t queue_create(void)
{
	//assign space for the queue
	queue_t my_queue = (struct queue*)malloc(sizeof(struct queue));
	struct node* head = (struct node*)malloc(sizeof(struct node));

	//error check
	if((my_queue == NULL) || (head == NULL)){
		return NULL;
	}

	//initialize the value
	head->value = NULL;
	head->next = NULL;

	my_queue->head = head;
	my_queue->tail = NULL;
	my_queue->size = 0;

	return my_queue;
}

//function that destroy the queue
int queue_destroy(queue_t queue)
{
	if(queue->size != 0){
		return -1;
	}
	//simply free its memory
	free(queue);
	return 0;
}

//funciton that add value into queue
int queue_enqueue(queue_t queue, void *data)
{	
	//error check
	if((queue == NULL) || (data == NULL)){
		return -1;
	}
	//assign space for the queue in initilize value
	struct node* element = (struct node*)malloc(sizeof(struct node));
	if(element == NULL){
		return -1;
	}
	element->value = data;
	element->next = NULL;
	if(queue->size == 0){
		queue->head->next = element;
		queue->tail = element;
	} else {
		queue->tail->next = element;
		queue->tail = queue->tail->next;
	}
	queue->size++;
	return 0;
}

//function that remove the first value in the queue
int queue_dequeue(queue_t queue, void **data)
{
	//error check
	if((queue == NULL) || (data == NULL) || (queue->size == 0)){
		return -1;
	}
	//redirect the node
	struct node* delete_node = queue->head->next;
	*data = delete_node->value;
	queue->head->next = queue->head->next->next;
	free(delete_node);
	queue->size--;
	return 0;
}

//function that delete a certain value
int queue_delete(queue_t queue, void *data)
{
	//error check
	if((queue == NULL) || (data == NULL) || (queue->size == 0)){
		return -1;
	}
	//find the match from the head
	struct node* current = queue->head;
	for(int i = 0; i < queue->size; i++){
		if(current->next->value == data){
			//if found, redirect the node
			struct node* delete_node = current->next;
			if(current->next->next != NULL){
				current->next = current->next->next;
			} else {
				current->next = NULL;
				queue->tail = current;
			}
			free(delete_node);
			queue->size--;
			return 0;
		} else {
			current = current->next;
		}
	}
	return -1;
}

//function that apply func to all elements in the queue
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
	//error check
	if((queue == NULL) || (func == NULL) || (queue->size == 0)){
		return -1;
	}
	//iterate from the head
	struct node* current = queue->head->next;
	for(int i = 0; i < queue->size; i++){
		//if function return 1, end the loop
		if(func(current->value, arg)){
			*data = current->value;
			break;
		}
		current = current->next;
	}
	return 0;
}

//simply return the length of queue
int queue_length(queue_t queue)
{
	if(queue == NULL){
		return -1;
	}
	return queue->size;
}

