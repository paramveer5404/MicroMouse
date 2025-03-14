#include "Headers/Int_Vector.h"

// Define global vectors
IntVector rightValues;
IntVector leftValues;

// Constructor
IntVector::IntVector() : data(nullptr), capacity(0), size(0) {}

// Destructor
IntVector::~IntVector() {
    delete[] data;
}

// Private method to resize
void IntVector::resize() {
    capacity = (capacity == 0) ? 1024 : capacity * 2;
    int *newData = new int[capacity];
    for (size_t i = 0; i < size; i++) newData[i] = data[i];
    delete[] data;
    data = newData;
}

// Public methods
void IntVector::push_back(int value) {
    if (size == capacity) resize();
    data[size++] = value;
}

void IntVector::pop_back() {
    if (size > 0) size--;
}

int IntVector::operator[](size_t index) const {
    return data[index];  // Note: no bounds checking
}

size_t IntVector::get_size() const {
    return size;
}

void IntVector::reset() {
    size = 0;
}

bool IntVector::analyzeWallPresence(int threshold) {
    if (size == 0) return 0;  // No data to analyze

    size_t startIndex = size * 60 / 100; // Start at 60% of data
    size_t endIndex = size * 90 / 100;   // End at 90% of data
    size_t countOne = 0, countZero = 0;

    for (size_t i = startIndex; i < endIndex; i++) {
        if (data[i] > threshold) countOne++;  // Above threshold (No Wall)
        else countZero++;  // Below threshold (Wall)
    }

    return (countOne > countZero) ? false : true;  // Majority wins
}
