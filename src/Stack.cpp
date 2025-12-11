#include "Stack.h"

Stack::Stack(int cap) : topNode(nullptr), currentSize(0), capacity(cap) {}

Stack::~Stack() {
    clear();
}

bool Stack::isEmpty() const {
    return topNode == nullptr;
}

bool Stack::isFull() const {
    return currentSize >= capacity;
}

bool Stack::push(int carId) {
    if (isFull()) return false;
    Car* newCar = new Car(carId);
    newCar->next = topNode;
    topNode = newCar;
    ++currentSize;
    return true;
}

bool Stack::pop(int &carId) {
    if (isEmpty()) return false;
    Car* temp = topNode;
    carId = temp->carId;
    topNode = topNode->next;
    delete temp;
    --currentSize;
    return true;
}

bool Stack::peek(int &carId) const {
    if (isEmpty()) return false;
    carId = topNode->carId;
    return true;
}

void Stack::printStack() const {
    Car* current = topNode;
    int pos = 1;
    while (current != nullptr) {
        std::cout << "  Position " << pos << " -> CarID: " << current->carId << std::endl;
        current = current->next;
        ++pos;
    }
}

int Stack::size() const {
    return currentSize;
}

int Stack::getCapacity() const {
    return capacity;
}

void Stack::sort() {
    topNode = mergeSort(topNode);
}

// Find position from top (1 = top), return -1 if not found
int Stack::findPosition(int carId) const {
    Car* current = topNode;
    int position = 1;
    while (current != nullptr) {
        if (current->carId == carId) return position;
        current = current->next;
        ++position;
    }
    return -1;
}

// Free all nodes — called by destructor
void Stack::clear() {
    Car* current = topNode;
    while (current != nullptr) {
        Car* next = current->next;
        delete current;
        current = next;
    }
    topNode = nullptr;
    currentSize = 0;
}

// --- merge sort helpers ---

Car* Stack::mergeSort(Car* head) {
    if (head == nullptr || head->next == nullptr) return head;

    Car* a;
    Car* b;
    splitList(head, &a, &b);

    a = mergeSort(a);
    b = mergeSort(b);

    return sortedMerge(a, b);
}

// Split list into two halves using slow/fast pointer
void Stack::splitList(Car* source, Car** frontRef, Car** backRef) {
    Car* slow = source;
    Car* fast = source->next;

    while (fast != nullptr) {
        fast = fast->next;
        if (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *frontRef = source;
    *backRef = slow->next;
    slow->next = nullptr;
}

// Merge two sorted lists (ascending order)
Car* Stack::sortedMerge(Car* a, Car* b) {
    if (!a) return b;
    if (!b) return a;

    Car* result = nullptr;

    if (a->carId <= b->carId) {
        result = a;
        result->next = sortedMerge(a->next, b);
    } else {
        result = b;
        result->next = sortedMerge(a, b->next);
    }
    return result;
}