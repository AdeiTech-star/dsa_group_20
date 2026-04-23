// rndahiro did this min_heap.cpp

#include "../include/min_heap.h"
#include <iostream>
using namespace std;

// setting up a heap constructor
MinHeap::MinHeap(int cap) : capacity(cap), currentSize(0) {
    heap = new HeapNode[capacity];
}

// cleaning up the dynamic allocated memory
MinHeap::~MinHeap() {
    delete[] heap;
}

// swapping helper without using a temp variable for each field
void MinHeap::swap(int i, int j) {
    HeapNode temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
}

// moving the new added/inserted element up until heap property is satisfied
// the node of the parent i is at the index of (i-1)/2
void MinHeap::heapifyUp(int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        
        // in case the parent is smaller, the process is done
        if (heap[parent].priority <= heap[index].priority)
            break;
            
        swap(index, parent);
        index = parent;
    }
}

// moving the root element down to restore heap property after extraction
// the node of the children i are at 2*i+1 on the left and 2*i+2 on the right
void MinHeap::heapifyDown(int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    // checking if the left child exists and smaller
    if (left < currentSize && heap[left].priority < heap[smallest].priority)
        smallest = left;

    // checking if the right child exists and smaller
    if (right < currentSize && heap[right].priority < heap[smallest].priority)
        smallest = right;

    // if a child is found smaller,swap and continue down that path
    if (smallest != index) {
        swap(index, smallest);
        heapifyDown(smallest);
    }
}

// doubling the heap capacity and copying all elements
// Similar to how the hash table resizes
void MinHeap::resize() {
    int newCapacity = capacity * 2;
    HeapNode* newHeap = new HeapNode[newCapacity];
    
    // copying all existing elements to new array
    for (int i = 0; i < currentSize; i++) {
        newHeap[i] = heap[i];
    }
    
    // freeing old array and updating pointers
    delete[] heap;
    heap = newHeap;
    capacity = newCapacity;
    
    // cout << "Heap resized to capacity " << capacity << "\n";
}

// adding a new element to the heap
bool MinHeap::insert(int priority, int data) {
    if (currentSize == capacity) {
        // cout << "Error: Heap is full. Cannot insert more elements.\n";
        // return false;
        resize();
    }

    // placing the new element at the end
    heap[currentSize].priority = priority;
    heap[currentSize].data = data;
    
    // bubbling it up to the correct position
    heapifyUp(currentSize);
    currentSize++;
    
    return true;
}

// removing and returing the element with minimum priority
HeapNode MinHeap::extractMin() {
    if (currentSize <= 0) {
        // returning the sentinel values to indicate empty heap
        return {-1, -1};
    }

    if (currentSize == 1) {
        currentSize--;
        return heap[0];
    }

    // saving the minimum element (root)
    HeapNode root = heap[0];
    
    // moving the last element to root
    heap[0] = heap[currentSize - 1];
    currentSize--;
    
    // adjusting the heap property from root down
    heapifyDown(0);
    
    return root;
}

bool MinHeap::isEmpty() const {
    return currentSize == 0;
}

int MinHeap::size() const {
    return currentSize;
}

int MinHeap::getCapacity() const {
    return capacity;
}
