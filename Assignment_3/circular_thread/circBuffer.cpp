#include "circBuffer.h"
#include "mbed.h"

typedef void (*fooptr)(void);
 
int circBuffPush(circBuf_t *c, fooptr funPtr)
{
    // next is where head will point to after this write.
    int next = c->head + 1;
    if (next >= c->maxLen)
        next = 0;

    if (next == c->tail) // check if circular buffer is full
        return -1;       // and return with an error.

    c->buffer[c->head] = funPtr; // Load data and then move
    c->head = next;            // head to next data offset.
    return 0;  // return success to indicate successful push.
}
 
int circBuffPop(circBuf_t *c, fooptr *funPtr)
{
    // if the head isn't ahead of the tail, we don't have any characters
    if (c->head == c->tail) // check if circular buffer is empty
        return -1;          // and return with an error

    // next is where tail will point to after this read.
    int next = c->tail + 1;
    if(next >= c->maxLen)
        next = 0;

    *funPtr = c->buffer[c->tail]; // Read data and then move
    c->tail = next;             // tail to next data offset.
    return 0;  // return success to indicate successful push.
}