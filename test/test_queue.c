
#include <stdio.h>
#include <assert.h>
#include "queue.h"

void test_create(void){
    queue_t q;
    
    q = queue_create();
    assert(q != NULL);
}

void test_queue_simple(void){
    queue_t q;
    int data = 3, *ptr;
    
    q = queue_create();
    queue_enqueue(q, &data);
    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data);
	queue_destroy(q);
}

/* Callback function that increments items by a certain value */
static int inc_item(void *data, void *arg)
{
    int *a = (int*)data;
    int inc = (int)(long)arg;
    
    *a += inc;
    
    return 0;
}

static int dec_item(void *data, void *arg)
{
    int *a = (int*)data;
    int dec = (int)(long)arg;
    
    *a -= dec;
    
    return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(void *data, void *arg)
{
    int *a = (int*)data;
    int match = (int)(long)arg;
    
    if (*a == match)
        return 1;
    
    return 0;
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int i;
    int *ptr;
    
    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);
    
    /* Add value '1' to every item of the queue */
    queue_iterate(q, inc_item, (void*)1, NULL);
    assert(data[0] == 2);
    
    /* Find and get the item which is equal to value '5' */
    ptr = NULL;
    queue_iterate(q, find_item, (void*)5, (void**)&ptr);
    assert(ptr != NULL);
    assert(*ptr == 5);
    assert(ptr == &data[3]);
	queue_destroy(q);
}

void test_destroy(void){
    queue_t q;
    q = queue_create();
    int data = 7, *ptr;
    queue_enqueue(q, &data);
    queue_dequeue(q, (void **)&ptr);
    int ret_value = queue_destroy(q);
    assert(ret_value == 0);
    //assert(q == NULL);
}

void test_destroy_error(void){
    queue_t q;
    q = queue_create();
    int data = 7;
    queue_enqueue(q, &data);
    int ret_value = queue_destroy(q);
    assert(ret_value == -1);
}

void test_delete(void){
    queue_t q;
//    int test_number = 3;
    q = queue_create();
    int data1 = 1;
    int data2 = 2;
    int data3 = 3;
    int data4 = 4;
    int data5 = 5;
    queue_enqueue(q, &data1);
    queue_enqueue(q, &data2);
    queue_enqueue(q, &data3);
    queue_enqueue(q, &data4);
    queue_enqueue(q, &data5);
    queue_delete(q, &data3);
    int *ptr;
    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data1);
    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data2);
    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data4);
    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data5);
	queue_destroy(q);
}

void test_dequeue(void){
    queue_t q;
    q = queue_create();
    int data1 = 1;
    int data2 = 2;
    int data3 = 3;
    int data4 = 4;
    int data5 = 5;
    queue_enqueue(q, &data1);
    queue_enqueue(q, &data2);
    queue_enqueue(q, &data3);
    queue_enqueue(q, &data4);
    queue_enqueue(q, &data5);
    int *ptr;
    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data1);
    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data2);
    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data3);
    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data4);
    queue_dequeue(q, (void **)&ptr);
    assert(ptr == &data5);
	queue_destroy(q);
}

void test_delete_error(void){
    queue_t q = NULL;
    int test_number = 3;
    int data = 10;
    int ret_catch = queue_delete(q, &test_number);
    assert(ret_catch == -1);
    queue_enqueue(q, &data);
    ret_catch = 0;
    ret_catch = queue_delete(q, &test_number);
    assert(ret_catch == -1);
}

void test_enqueue_error(void){
    queue_t q = NULL;
    int data = 3;
    int ret1 = queue_enqueue(q, &data);
    q = queue_create();
    int ret2 = queue_enqueue(q, NULL);
    assert(ret1 == -1 && ret2 == -1);
	queue_destroy(q);
}

void test_queue_length(void){
    queue_t q = NULL;
    assert(queue_length(q) == -1);
    q = queue_create();
    int* ptr;
    int data1 = 1;
    int data2 = 2;
    int data3 = 3;
    assert(queue_length(q) == 0);
    queue_enqueue(q, &data1);
    assert(queue_length(q) == 1);
    queue_enqueue(q, &data2);
    assert(queue_length(q) == 2);
    queue_dequeue(q, (void **)&ptr);
    assert(queue_length(q) == 1);
    queue_enqueue(q, &data2);
    queue_enqueue(q, &data3);
    assert(queue_length(q) == 3);
    queue_delete(q, &data2);
    assert(queue_length(q) == 2);
	queue_destroy(q);
}

void iterator_test(void){
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int i;
    int *ptr;
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);
    queue_iterate(q, dec_item, (void*)1, NULL);
    assert(data[0] == 0);
    assert(data[2] == 2);
    
    ptr = NULL;
    queue_iterate(q, find_item, (void*)5, (void**)&ptr);
    assert(ptr != NULL);
    assert(*ptr == 5);
    assert(ptr == &data[5]);
	queue_destroy(q);
}

int main(int argc, const char * argv[]) {
//     insert code here...
    int success = 0;
    test_create();
    printf("Provided test 1 successfully.\n");
    success++;
    test_queue_simple();
    printf("Provided test 2 successfully.\n");
    success++;
    test_iterator();
    printf("Provided test 3 successfully.\n");
    success++;
    
    
    test_destroy();
    printf("User test 1 successfully.\n");
    success++;
    test_destroy_error();
    printf("User test 2 successfully.\n");
    success++;
    test_enqueue_error();
    printf("User test 3 successfully.\n");
    success++;
    test_dequeue();
    printf("User test 4 successfully.\n");
    success++;
    test_delete();
    printf("User test 5 successfully.\n");
    success++;
    test_delete_error();
    printf("User test 6 successfully.\n");
    success++;
    test_queue_length();
    printf("User test 7 successfully.\n");
    success++;
    iterator_test();
    printf("User test 8 successfully.\n");
    success++;
    
    printf("ALL passed. Total %d tests passed.\n",success);
    return 0;
}



