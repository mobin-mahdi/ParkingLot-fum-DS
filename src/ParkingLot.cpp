#include "ParkingLot.h"
#include <iostream>

ParkingLot::ParkingLot(int nStacks, int capacityPerStack)
    : numStacks(nStacks),
      stackCapacity(capacityPerStack) {
    stacks = new Stack[numStacks];
    for (int i = 0; i < numStacks; ++i) {
        stacks[i] = Stack(stackCapacity);
    }
}

ParkingLot::~ParkingLot() {
    delete [] stacks;
}

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

    // All stacks full — car is lost (dequeued but not parked)
    std::cout << "Parking full. Car " << carId << " cannot be parked.\n";
}

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
        return;
    }

    target.push(carId);
    std::cout << "Car " << carId << " parked in stack " << stackIndex << ".\n";
}

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

void ParkingLot::sortStack(int stackIndex) {
    if (!isValidStackIndex(stackIndex)) {
        std::cout << "Invalid stack index.\n";
        return;
    }
    stacks[stackIndex - 1].sort();  // Uses merge sort (ascending)
    std::cout << "Stack " << stackIndex << " has been sorted by car ID.\n";
}

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

    // Move as many cars as possible, spilling to next stacks if needed
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

// Search queue + all stacks — used to prevent duplicate car IDs
bool ParkingLot::carAlreadyInSystem(int carId) const {
    if (entranceQueue.contains(carId)) {
        return true;
    }

    for (int i = 0; i < numStacks; ++i) {
        if (stacks[i].findPosition(carId) != -1) {
            return true;
        }
    }
    return false;
}

int ParkingLot::getNumStacks() const {
    return numStacks;
}

int ParkingLot::getStackCapacity() const {
    return stackCapacity;
}