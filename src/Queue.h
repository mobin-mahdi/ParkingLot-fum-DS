#ifndef QUEUE_H
#define QUEUE_H
#include "Car.h"

// Entrance queue implemented as a linked list of Car nodes.
class Queue {
private:
    Car* frontNode;
    Car* rearNode;
    int currentSize;

public:
    // Time Complexity: O(1)
    Queue();

    // Time Complexity: O(n)
    ~Queue();

    // Time Complexity: O(1)
    bool isEmpty() const;

    // Time Complexity: O(1)
    void enqueue(int carId);

    // Time Complexity: O(1)
    bool dequeue(int &carId);

    // Time Complexity: O(1)
    bool front(int &carId) const;
    
    // Time Complexity: O(n)
    bool contains(int carId) const;

    // Time Complexity: O(1)
    int size() const;

    // Time Complexity: O(n)
    void printQueue() const;

    // Time Complexity: O(n)
    void clear();
};

#endif // QUEUE_H
