#include "../include/min_heap.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

// Match the style from test_graph.cpp and test_hash_table.cpp
static int passed = 0;
static int failed = 0;

void check(const char* label, bool condition) {
    if (condition) {
        cout << "  [PASS] " << label << "\n";
        passed++;
    } else {
        cout << "  [FAIL] " << label << "\n";
        failed++;
    }
}

// Basic insertion and extraction in priority order
void testBasicOperations() {
    cout << "\n-- Basic Min-Heap Operations --\n";
    MinHeap pq(10);

    // Insert incidents with different priorities
    pq.insert(10, 101); 
    pq.insert(5, 102);  
    pq.insert(20, 103); 
    pq.insert(1, 104);  // most urgent
    
    check("size after 4 insertions is 4", pq.size() == 4);
    
    // Should extract in order: 1, 5, 10, 20
    HeapNode top = pq.extractMin();
    check("first extraction has priority 1", top.priority == 1 && top.data == 104);
    
    top = pq.extractMin();
    check("second extraction has priority 5", top.priority == 5 && top.data == 102);
    
    top = pq.extractMin();
    check("third extraction has priority 10", top.priority == 10 && top.data == 101);
    
    top = pq.extractMin();
    check("fourth extraction has priority 20", top.priority == 20 && top.data == 103);
    
    check("heap is empty after all extractions", pq.isEmpty());
}

// Simulate Dijkstra's algorithm behavior
void testDijkstraSimulation() {
    cout << "\n-- Dijkstra Frontier Simulation --\n";
    MinHeap frontier(20);

    // Add vertices with distances from source
    frontier.insert(0, 0);   // start node
    frontier.insert(15, 3);  
    frontier.insert(5, 1);   
    frontier.insert(10, 2);  

    // Dijkstra processes nodes in order of distance
    HeapNode next = frontier.extractMin();
    check("closest node is vertex 0 at distance 0", 
          next.data == 0 && next.priority == 0);

    next = frontier.extractMin();
    check("next closest is vertex 1 at distance 5", 
          next.data == 1 && next.priority == 5);

    next = frontier.extractMin();
    check("then vertex 2 at distance 10", 
          next.data == 2 && next.priority == 10);

    next = frontier.extractMin();
    check("finally vertex 3 at distance 15", 
          next.data == 3 && next.priority == 15);
}

// Test boundary conditions
void testEdgeCases() {
    cout << "\n-- Edge Cases --\n";
    
    MinHeap smallHeap(3);
    
    // Fill to capacity
    check("insert into empty heap succeeds", smallHeap.insert(10, 1));
    check("second insert succeeds", smallHeap.insert(20, 2));
    check("third insert succeeds", smallHeap.insert(5, 3));
    
    // Try to overflow
    // check("insert into full heap fails", !smallHeap.insert(15, 4));
    // check("size is still 3 after failed insert", smallHeap.size() == 3);
    
    // Empty it completely
    smallHeap.extractMin();
    smallHeap.extractMin();
    smallHeap.extractMin();
    
    check("heap is empty after removing all elements", smallHeap.isEmpty());
    
    // Extract from empty heap
    HeapNode empty = smallHeap.extractMin();
    check("extracting from empty heap returns {-1, -1}", 
          empty.priority == -1 && empty.data == -1);
}

// Verify heap property holds under random insertions
void testRandomizedStress() {
    cout << "\n-- Randomized Stress Test (100 elements) --\n";
    
    srand(time(0));
    
    int numElements = 100;
    MinHeap pq(numElements);
    
    // Insert random priorities
    for (int i = 0; i < numElements; i++) {
        int randomPriority = rand() % 1000;
        pq.insert(randomPriority, i);
    }
    
    check("all 100 elements inserted", pq.size() == 100);
    
    // Extract all and verify they come out sorted
    int lastPriority = -1;
    bool inOrder = true;
    
    for (int i = 0; i < numElements; i++) {
        HeapNode current = pq.extractMin();
        
        if (current.priority < lastPriority) {
            inOrder = false;
            break;
        }
        lastPriority = current.priority;
    }
    
    check("all elements extracted in ascending order", inOrder);
    check("heap is empty after extracting all", pq.isEmpty());
}

// Test with duplicate priorities
void testDuplicatePriorities() {
    cout << "\n-- Duplicate Priorities --\n";
    MinHeap pq(10);
    
    pq.insert(5, 100);
    pq.insert(5, 200);
    pq.insert(5, 300);
    pq.insert(3, 400);
    
    HeapNode node = pq.extractMin();
    check("lowest priority extracted first", node.priority == 3);
    
    // All three with priority 5 should come out next
    // (order among same-priority items doesn't matter)
    node = pq.extractMin();
    check("first duplicate has priority 5", node.priority == 5);
    
    node = pq.extractMin();
    check("second duplicate has priority 5", node.priority == 5);
    
    node = pq.extractMin();
    check("third duplicate has priority 5", node.priority == 5);
}

// testing if the heap automatically resizes when full
void testAutoResize() {
    cout << "\n-- Auto Resize Test --\n";
    
    // starting with a very small heap
    MinHeap pq(5);
    
    check("initial capacity is 5", pq.getCapacity() == 5);
    
    // adding 5 elements to fill it
    for (int i = 0; i < 5; i++) {
        pq.insert(i * 10, i);
    }
    
    check("size is 5 after filling", pq.size() == 5);
    check("capacity still 5", pq.getCapacity() == 5);
    
    // one more addition in order to trigger resize
    pq.insert(99, 99);
    
    check("size is 6 after resize", pq.size() == 6);
    check("capacity doubled to 10", pq.getCapacity() == 10);
    
    // verifying if all elements still work correctly
    HeapNode min = pq.extractMin();
    check("smallest element still correct", min.priority == 0);
}

// simulating dijkstra with lazy deletion causing many heap entries
void testDijkstraLazyInsertion() {
    cout << "\n-- Dijkstra Lazy Insertion Simulation --\n";
    
    MinHeap pq(20);  // Start small to force resize
    
    // Simulate lazy Dijkstra on 100-node graph
    int numVertices = 100;
    
    // simulating relaxation by adding each vertex 3 times
    for (int v = 0; v < numVertices; v++) {
        pq.insert(v * 10, v);      // first path
        pq.insert(v * 10 - 5, v);  // better path
        pq.insert(v * 10 - 8, v);  // even better path
    }
    
    check("heap holds 300 entries without failing", pq.size() == 300);
    check("capacity grew to accommodate all entries", pq.getCapacity() >= 300);
    
    // verifying if the heap property still holds
    int lastPriority = -999;
    bool inOrder = true;
    
    for (int i = 0; i < 300; i++) {
        HeapNode node = pq.extractMin();
        if (node.priority < lastPriority) {
            inOrder = false;
            break;
        }
        lastPriority = node.priority;
    }
    
    check("all 300 entries extracted in correct order", inOrder);
}

// Main test runner
int main() {
    cout << "======================================\n";
    cout << "   Min-Heap Test Suite               \n";
    cout << "======================================\n";

    testBasicOperations();
    testDijkstraSimulation();
    testEdgeCases();
    testRandomizedStress();
    testDuplicatePriorities();
    testAutoResize();
    testDijkstraLazyInsertion();

    cout << "\n======================================\n";
    cout << "Results: " << passed << " passed, " << failed << " failed.\n";
    
    if (failed > 0) {
        cout << "Some tests FAILED. Review output above.\n";
        return 1;
    }
    
    cout << "All tests passed.\n";
    return 0;
}