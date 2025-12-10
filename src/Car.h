#ifndef CAR_H
#define CAR_H

// Basic car node used in both Stack and Queue
struct Car {
    int carId;
    Car* next;

    Car(int id = 0) : carId(id), next(nullptr) {}
};

#endif // CAR_H

