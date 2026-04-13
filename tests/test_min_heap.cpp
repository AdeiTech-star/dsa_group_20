#include "../include/min_heap.h"
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <ctime>
using namespace std;

// testing the basic insertion and extraction
void testBasicOperations() {
    cout << "--- Testing Basic Min-Heap Operations ---" << endl;
    MinHeap pq(10);

    // adding/inserting some incidents with different priorities
    pq.insert(10, 101); 
    pq.insert(5, 102);  
    pq.insert(20, 103); 
    pq.insert(1, 104);  // this is expected to be the most urgent
    
    assert(pq.size() == 4);
    
    // expecting to have an outcome in priority order like 1, 5, 10, 20
    HeapNode top = pq.extractMin();
    cout << "Extracted: Priority " << top.priority << " (ID: " << top.data << ")" << endl;
    assert(top.priority == 1 && top.data == 104);

    top = pq.extractMin();
    cout << "Extracted: Priority " << top.priority << " (ID: " << top.data << ")" << endl;
    assert(top.priority == 5 && top.data == 102);

    cout << "Basic operations passed!" << endl << endl;
}

// testing the simulating of the algorithm of dijkstra
void testDijkstraSimulation() {
    cout << "--- Testing Dijkstra Frontier Simulation ---" << endl;
    MinHeap frontier(20);

    // adding vertices with their respective their distances from source
    frontier.insert(0, 0);   
    frontier.insert(15, 3);  
    frontier.insert(5, 1);   
    frontier.insert(10, 2);  

    // dijkstra always processes the closest unvisited node first
    HeapNode next = frontier.extractMin();
    assert(next.data == 0 && next.priority == 0);

    next = frontier.extractMin();
    assert(next.data == 1 && next.priority == 5);

    next = frontier.extractMin();
    assert(next.data == 2 && next.priority == 10);

    cout << "Dijkstra simulation passed!" << endl << endl;
}

// testing the edge case like the empty and full heap
void testEdgeCases() {
    cout << "--- Testing Edge Cases ---" << endl;
    
    MinHeap smallHeap(3);
    
    // filling it up
    assert(smallHeap.insert(10, 1));
    assert(smallHeap.insert(20, 2));
    assert(smallHeap.insert(5, 3));
    
    // trying to overflow the heap
    bool overflowed = smallHeap.insert(15, 4);
    assert(!overflowed); // expecting to return false
    
    // emptying the heap
    smallHeap.extractMin();
    smallHeap.extractMin();
    smallHeap.extractMin();
    
    assert(smallHeap.isEmpty());
    
    // extracting from an empty heap, expecting to return {-1, -1}
    HeapNode empty = smallHeap.extractMin();
    assert(empty.priority == -1 && empty.data == -1);
    
    cout << "Edge cases passed!" << endl << endl;
}

// testing the heavy load with random data
void testRandomizedStress() {
    cout << "Testing Randomized Stress Test for 100 elementss" << endl;
    
    srand(time(0));
    
    int numElements = 100;
    MinHeap pq(numElements);
    
    cout << "Inserting 100 random priorities" << endl;
    for (int i = 0; i < numElements; i++) {
        int randomPriority = rand() % 1000;
        pq.insert(randomPriority, i);
    }
    
    assert(pq.size() == 100);
    
    // extracting all elements and verifying that they come out sorted
    int lastPriority = -1;
    bool passed = true;
    
    for (int i = 0; i < numElements; i++) {
        HeapNode current = pq.extractMin();
        
        if (current.priority < lastPriority) {
            cout << "error: " << current.priority 
                 << " after " << lastPriority << endl;
            passed = false;
            break;
        }
        lastPriority = current.priority;
    }
    
    if (passed) {
        cout << "stress test passed with all elements came out in order." << endl;
    } else {
        assert(false);
    }
    
    cout << endl;
}

// the main test runner
int main() {
    cout << "**************************************" << endl;
    cout << "   The Min-Heap Test Suit             " << endl;
    cout << "**************************************" << endl << endl;

    testBasicOperations();
    testDijkstraSimulation();
    testEdgeCases();
    testRandomizedStress();

    cout << "************ all is working as expected - r *******" << endl;
    
    return 0;
}
