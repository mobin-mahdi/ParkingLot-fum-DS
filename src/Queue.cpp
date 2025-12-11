#include "Queue.h"
#include <iostream>

Queue::Queue() : frontNode(nullptr), rearNode(nullptr), currentSize(0) {}

Queue::~Queue() {
    clear();
}

bool Queue::isEmpty() const {
    return frontNode == nullptr;
}

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

bool Queue::dequeue(int &carId) {
    if (isEmpty()) {
        return false;
    }
    Car* temp = frontNode;
    carId = temp->carId;
    frontNode = frontNode->next;
    if (frontNode == nullptr) {
        rearNode = nullptr;  // Queue becomes empty
    }
    delete temp;
    --currentSize;
    return true;
}

bool Queue::front(int &carId) const {
    if (isEmpty()) {
        return false;
    }
    carId = frontNode->carId;
    return true;
}

// Search for car ID — used to prevent duplicates
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

int Queue::size() const {
    return currentSize;
}

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

// Free all nodes — called by destructor
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