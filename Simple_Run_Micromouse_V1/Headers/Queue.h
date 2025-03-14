#ifndef QUEUE_H
#define QUEUE_H

#include "Cell.h"

class Queue {
  public:
    Queue(int size);               // Constructor
    ~Queue();                     // Destructor

    void push(Coordinates item);  // Add item to the queue
    void pop();                   // Remove item from the queue
    Coordinates frontItem();      // Access the front item
    bool isEmpty();               // Check if empty

  private:
    Coordinates* data;  // Pointer to dynamic array
    int maxSize;
    int front;
    int rear;
    int count;
};

#endif  // QUEUE_H
