#ifndef STACK_H
#define STACK_H

#include "Car.h"
#include <iostream>

class Stack {
private:
    Car* topNode;
    int currentSize;
    int capacity;

    Car* mergeSort(Car* head);
    void splitList(Car* source, Car** frontRef, Car** backRef);
    Car* sortedMerge(Car* a, Car* b);

public:
    Stack(int cap = 0);
    ~Stack();

    bool isEmpty() const;
    bool isFull() const;
    bool push(int carId);
    bool pop(int &carId);
    bool peek(int &carId) const;
    void printStack() const;
    int size() const;
    int getCapacity() const;
    void sort();
    int findPosition(int carId) const;
    void clear();
};

#endif // STACK_H
