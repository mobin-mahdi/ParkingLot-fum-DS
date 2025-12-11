#ifndef STACK_H
#define STACK_H
#include "Car.h"
#include <iostream>

class Stack {
private:
    Car* topNode;
    int currentSize;
    int capacity;

    // Merge sort helper functions
    Car* mergeSort(Car* head);
    void splitList(Car* source, Car** frontRef, Car** backRef);
    Car* sortedMerge(Car* a, Car* b);

public:
    // Time Complexity: O(1)
    Stack(int cap = 0);

    // Time Complexity: O(k) where k = number of cars in the stack
    ~Stack();

    // Time Complexity: O(1)
    bool isEmpty() const;

    // Time Complexity: O(1)
    bool isFull() const;

    // Time Complexity: O(1)
    bool push(int carId);

    // Time Complexity: O(1)
    bool pop(int &carId);

    // Time Complexity: O(1)
    bool peek(int &carId) const;

    // Time Complexity: O(k) where k = number of cars in stack
    void printStack() const;

    // Time Complexity: O(1)
    int size() const;

    // Time Complexity: O(1)
    int getCapacity() const;

    // Time Complexity: O(k log k) where k = number of cars in stack
    void sort();

    // Time Complexity: O(k) where k = number of cars in stack
    int findPosition(int carId) const;

    // Time Complexity: O(k) where k = number of cars in stack
    void clear();
};

#endif // STACK_H