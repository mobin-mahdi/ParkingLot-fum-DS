#include "ParkingLot.h"
#include <iostream>

// Time Complexity: O(n)
ParkingLot::ParkingLot(int nStacks, int capacityPerStack)
    : numStacks(nStacks),
      stackCapacity(capacityPerStack) {
    stacks = new Stack[numStacks];
    for (int i = 0; i < numStacks; ++i) {
        stacks[i] = Stack(stackCapacity);
    }
}

// Time Complexity: O(1) (each Stack destructor frees its own memory)
ParkingLot::~ParkingLot() {
    delete [] stacks;
}

// Time Complexity: O(1)
bool ParkingLot::isValidStackIndex(int stackIndex) const {
    return stackIndex >= 1 && stackIndex <= numStacks;
}

void ParkingLot::addCarToEntrance(int carId) {
    if (carAlreadyInSystem(carId)) {
        std::cout << "Error: A car with ID " << carId
                  << " already exists in the system (queue or stacks).\n";
        return;
    }
    entranceQueue.enqueue(carId);
    std::cout << "Car " << carId << " added to entrance queue.\n";
}

// Time Complexity: O(n)
void ParkingLot::parkCarInFirstAvailableStack() {
    if (entranceQueue.isEmpty()) {
        std::cout << "Entrance queue is empty. No car to park.\n";
        return;
    }
    int carId;
    entranceQueue.dequeue(carId);

    for (int i = 0; i < numStacks; ++i) {
        if (!stacks[i].isFull()) {
            stacks[i].push(carId);
            std::cout << "Car " << carId << " parked in stack " << (i + 1) << ".\n";
            return;
        }
    }

    // If we reach here, all stacks were full:
    std::cout << "Parking full. Car " << carId << " cannot be parked.\n";
}

// Time Complexity: O(1) for valid index; O(1) push
void ParkingLot::parkCarInSpecificStack(int stackIndex) {
    if (!isValidStackIndex(stackIndex)) {
        std::cout << "Invalid stack index.\n";
        return;
    }
    if (entranceQueue.isEmpty()) {
        std::cout << "Entrance queue is empty. No car to park.\n";
        return;
    }

    int carId;
    entranceQueue.dequeue(carId);

    Stack &target = stacks[stackIndex - 1];
    if (target.isFull()) {
        std::cout << "Selected stack is full. Car " << carId << " cannot be parked.\n";
        // Optionally, push back to queue or handle differently if your teacher wants.
        return;
    }

    target.push(carId);
    std::cout << "Car " << carId << " parked in stack " << stackIndex << ".\n";
}

// Time Complexity: O(n * m)
bool ParkingLot::findCar(int carId, int &stackIndex, int &position) const {
    for (int i = 0; i < numStacks; ++i) {
        int pos = stacks[i].findPosition(carId);
        if (pos != -1) {
            stackIndex = i + 1;
            position = pos;
            return true;
        }
    }
    return false;
}

// Time Complexity: O(1)
bool ParkingLot::exitCarFromStackTop(int carId, int stackIndex) {
    if (!isValidStackIndex(stackIndex)) {
        std::cout << "Invalid stack index.\n";
        return false;
    }

    Stack &s = stacks[stackIndex - 1];
    if (s.isEmpty()) {
        std::cout << "Stack " << stackIndex << " is empty.\n";
        return false;
    }

    int topId;
    s.peek(topId);
    if (topId != carId) {
        std::cout << "Cannot remove car " << carId
                  << ". Only the car at the top (car " << topId
                  << ") can exit from stack " << stackIndex << ".\n";
        return false;
    }

    int removedId;
    s.pop(removedId);
    std::cout << "Car " << removedId << " exited from stack " << stackIndex << ".\n";
    return true;
}

// Time Complexity: O(k log k) where k is number of cars in stack
void ParkingLot::sortStack(int stackIndex) {
    if (!isValidStackIndex(stackIndex)) {
        std::cout << "Invalid stack index.\n";
        return;
    }
    stacks[stackIndex - 1].sort();
    std::cout << "Stack " << stackIndex << " has been sorted by car ID.\n";
}

// Time Complexity: O(T) where T is number of cars moved + number of stacks visited
void ParkingLot::moveBetweenStacks(int sourceIndex, int targetIndex) {
    if (!isValidStackIndex(sourceIndex) || !isValidStackIndex(targetIndex)) {
        std::cout << "Invalid stack index.\n";
        return;
    }
    if (sourceIndex == targetIndex) {
        std::cout << "Source and target stacks are the same. No movement performed.\n";
        return;
    }

    Stack &source = stacks[sourceIndex - 1];

    if (source.isEmpty()) {
        std::cout << "Source stack " << sourceIndex << " is already empty.\n";
        return;
    }

    int currentTarget = targetIndex - 1;

    while (!source.isEmpty() && currentTarget < numStacks) {
        Stack &target = stacks[currentTarget];

        while (!source.isEmpty() && !target.isFull()) {
            int carId;
            source.pop(carId);
            target.push(carId);
            std::cout << "Moved car " << carId
                      << " from stack " << sourceIndex
                      << " to stack " << (currentTarget + 1) << ".\n";
        }

        ++currentTarget;
    }

    if (!source.isEmpty()) {
        std::cout << "Warning: Not enough space to move all cars from stack "
                  << sourceIndex << ". Some cars remain.\n";
    } else {
        std::cout << "All cars moved. Stack " << sourceIndex << " is now empty.\n";
    }
}

// Time Complexity: O(n * m)
void ParkingLot::printParkingLotState() const {
    std::cout << "======= Parking Lot State =======\n";
    std::cout << "Entrance Queue size: " << entranceQueue.size() << std::endl;
    entranceQueue.printQueue();
    std::cout << "Number of stacks: " << numStacks
              << ", Capacity per stack: " << stackCapacity << "\n\n";

    for (int i = 0; i < numStacks; ++i) {
        std::cout << "Stack " << (i + 1) << " (size: " << stacks[i].size()
                  << "/" << stacks[i].getCapacity() << "):\n";
        stacks[i].printStack();
        std::cout << "---------------------------------\n";
    }
}

bool ParkingLot::carAlreadyInSystem(int carId) const {
    // Check entrance queue
    if (entranceQueue.contains(carId)) {
        return true;
    }

    // Check all stacks
    for (int i = 0; i < numStacks; ++i) {
        if (stacks[i].findPosition(carId) != -1) {
            return true;
        }
    }
    return false;
}

// Time Complexity: O(1)
int ParkingLot::getNumStacks() const {
    return numStacks;
}

// Time Complexity: O(1)
int ParkingLot::getStackCapacity() const {
    return stackCapacity;
}


