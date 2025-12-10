#include "Queue.h"
#include <iostream>

// Time Complexity: O(1)
Queue::Queue() : frontNode(nullptr), rearNode(nullptr), currentSize(0) {}

// Time Complexity: O(n)
Queue::~Queue() {
    clear();
}

// Time Complexity: O(1)
bool Queue::isEmpty() const {
    return frontNode == nullptr;
}

// Time Complexity: O(1)
void Queue::enqueue(int carId) {
    Car* newCar = new Car(carId);
    if (isEmpty()) {
        frontNode = rearNode = newCar;
    } else {
        rearNode->next = newCar;
        rearNode = newCar;
    }
    ++currentSize;
}



// Time Complexity: O(1)
bool Queue::dequeue(int &carId) {
    if (isEmpty()) {
        return false;
    }
    Car* temp = frontNode;
    carId = temp->carId;
    frontNode = frontNode->next;
    if (frontNode == nullptr) {
        rearNode = nullptr;
    }
    delete temp;
    --currentSize;
    return true;
}

// Time Complexity: O(1)
bool Queue::front(int &carId) const {
    if (isEmpty()) {
        return false;
    }
    carId = frontNode->carId;
    return true;
}

bool Queue::contains(int carId) const {
    Car* current = frontNode;
    while (current != nullptr) {
        if (current->carId == carId) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Time Complexity: O(1)
int Queue::size() const {
    return currentSize;
}

// Time Complexity: O(n)
void Queue::printQueue() const {
    Car* current = frontNode;
    std::cout << "Entrance Queue (front -> rear): ";
    while (current != nullptr) {
        std::cout << current->carId;
        if (current->next != nullptr) {
            std::cout << " <- ";
        }
        current = current->next;
    }
    std::cout << std::endl;
}

// Time Complexity: O(n)
void Queue::clear() {
    Car* current = frontNode;
    while (current != nullptr) {
        Car* next = current->next;
        delete current;
        current = next;
    }
    frontNode = rearNode = nullptr;
    currentSize = 0;
}
