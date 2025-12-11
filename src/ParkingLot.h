#ifndef PARKINGLOT_H
#define PARKINGLOT_H

#include "stack.h"
#include "Queue.h"

// Represents the entire parking lot system:
// One entrance queue
// An array of stacks (lanes)
class ParkingLot {
private:
    int numStacks;
    int stackCapacity;
    Stack* stacks;
    Queue entranceQueue;

    bool isValidStackIndex(int stackIndex) const;

    bool carAlreadyInSystem(int carId) const;

public:
    // Time Complexity: O(n)
    ParkingLot(int nStacks, int capacityPerStack);

    // Time Complexity: O(n * m) (n for each stack and m for the size of each stack)
    ~ParkingLot();

    // ** Entrance / Enqueue **

    // Time Complexity: O(n * m)
    void addCarToEntrance(int carId);

    // ** Parking operations **

    // Dequeue car and push into the first stack that has free space.
    // If all stacks are full, prints "Parking full".
    // Time Complexity: O(n)
    void parkCarInFirstAvailableStack();

    // Dequeue car and push into a specific stack (1-based index).
    // Time Complexity: O(1) check if stack index valid, O(1) push; overall O(1)
    void parkCarInSpecificStack(int stackIndex);

    // ** Find **

    // Find car by ID.
    // Outputs stackIndex (1-based) and position (1-based) from top of stack.
    // Returns true if found, false otherwise.
    // Time Complexity: O(n * m) where m is capacity per stack.
    bool findCar(int carId, int &stackIndex, int &position) const;

    // ** Exit **

    // Remove car only if it is at the top of the specified stack.
    // Returns true if removed, false otherwise.
    // Time Complexity: O(1)
    bool exitCarFromStackTop(int carId, int stackIndex);

    // ** Sort **

    // Sort a specific stack using recursive merge sort on its linked list.
    // Time Complexity: O(k log k) where k is number of cars in that stack.
    void sortStack(int stackIndex);

    // ** Move Between Stacks **

    // Move as many cars as possible from stack i to stack j.
    // If stack j fills up, continue with j+1, j+2, ...
    // Time Complexity: O(T) where T is total number of cars moved plus number of stacks visited.
    void moveBetweenStacks(int sourceIndex, int targetIndex);

    // ** Display / Debug **

    // Time Complexity: O(n * m)
    void printParkingLotState() const;

    // Time Complexity: O(1)
    int getNumStacks() const;

    // Time Complexity: O(1)
    int getStackCapacity() const;
};

#endif // PARKINGLOT_H
