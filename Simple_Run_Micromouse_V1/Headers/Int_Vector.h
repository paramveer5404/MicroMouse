#ifndef INT_VECTOR_H
#define INT_VECTOR_H

#include <Arduino.h>  // Needed for size_t and compatibility with Arduino projects

class IntVector {
  private:
    int *data;
    size_t capacity;
    size_t size;

    void resize();  // Private method for resizing array

  public:
    IntVector();    // Constructor
    ~IntVector();   // Destructor

    void push_back(int value);   // Add element to vector
    void pop_back();             // Remove last element
    int operator[](size_t index) const;  // Access element at index
    size_t get_size() const;     // Get current size
    void reset();               // Clear vector
    bool analyzeWallPresence(int threshold); // Analyze wall presence (custom function)
};

// Global declarations (if you want these to be global and shared)
extern IntVector rightValues;
extern IntVector leftValues;

#endif  // INT_VECTOR_H
