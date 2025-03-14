#include "Headers/Queue.h"

Queue::Queue(int size) : maxSize(size), front(0), rear(0), count(0) {
  data = new Coordinates[size];
}

Queue::~Queue() {
  delete[] data;
}

void Queue::push(Coordinates item) {
  if (count < maxSize) {
    data[rear] = item;
    rear = (rear + 1) % maxSize;
    count++;
  }
}

void Queue::pop() {
  if (count > 0) {
    front = (front + 1) % maxSize;
    count--;
  }
}

Coordinates Queue::frontItem() {
  return data[front];
}

bool Queue::isEmpty() {
  return count == 0;
}
