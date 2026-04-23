// rndahiro did this min_heap.h

#ifndef MIN_HEAP_H
#define MIN_HEAP_H

struct HeapNode {
    int priority; // the lower number is the higher priority it'll be
    int data;     // vertex ID, incident ID, or any associated data
};

class MinHeap {
private:
    HeapNode* heap;
    int capacity;
    int currentSize;

    // restoring the heap property by moving a node up the tree
    void heapifyUp(int index);
    
    // restoring the heap property by moving the node down the tree
    void heapifyDown(int index);
    
    // swapping two nodes in the heap
    void swap(int i, int j);

    // doubling the capacity in case the heap is full
    void resize();

public:
    MinHeap(int cap = 100);
    ~MinHeap();

    // adding/inserting a new element and meaninting the heap property
    bool insert(int priority, int data);
    
    // removing and returning the minimum element
    HeapNode extractMin();
    
    // checking if the heap is emplty or has no element
    bool isEmpty() const;
    
    // getting the current number of elements
    int size() const;

    // getting current capacity useful for testing purposes
    int getCapacity() const;
};

#endif