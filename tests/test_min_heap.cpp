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
    check("insert into full heap fails", !smallHeap.insert(15, 4));
    check("size is still 3 after failed insert", smallHeap.size() == 3);
    
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

    cout << "\n======================================\n";
    cout << "Results: " << passed << " passed, " << failed << " failed.\n";
    
    if (failed > 0) {
        cout << "Some tests FAILED. Review output above.\n";
        return 1;
    }
    
    cout << "All tests passed.\n";
    return 0;
}
// #include "../include/min_heap.h"
// #include <iostream>
// #include <cassert>
// #include <cstdlib>
// #include <ctime>
// using namespace std;

// // testing the basic insertion and extraction
// void testBasicOperations() {
//     cout << "--- Testing Basic Min-Heap Operations ---" << endl;
//     MinHeap pq(10);

//     // adding/inserting some incidents with different priorities
//     pq.insert(10, 101); 
//     pq.insert(5, 102);  
//     pq.insert(20, 103); 
//     pq.insert(1, 104);  // this is expected to be the most urgent
    
//     assert(pq.size() == 4);
    
//     // expecting to have an outcome in priority order like 1, 5, 10, 20
//     HeapNode top = pq.extractMin();
//     cout << "Extracted: Priority " << top.priority << " (ID: " << top.data << ")" << endl;
//     assert(top.priority == 1 && top.data == 104);

//     top = pq.extractMin();
//     cout << "Extracted: Priority " << top.priority << " (ID: " << top.data << ")" << endl;
//     assert(top.priority == 5 && top.data == 102);

//     cout << "Basic operations passed!" << endl << endl;
// }

// // testing the simulating of the algorithm of dijkstra
// void testDijkstraSimulation() {
//     cout << "--- Testing Dijkstra Frontier Simulation ---" << endl;
//     MinHeap frontier(20);

//     // adding vertices with their respective their distances from source
//     frontier.insert(0, 0);   
//     frontier.insert(15, 3);  
//     frontier.insert(5, 1);   
//     frontier.insert(10, 2);  

//     // dijkstra always processes the closest unvisited node first
//     HeapNode next = frontier.extractMin();
//     assert(next.data == 0 && next.priority == 0);

//     next = frontier.extractMin();
//     assert(next.data == 1 && next.priority == 5);

//     next = frontier.extractMin();
//     assert(next.data == 2 && next.priority == 10);

//     cout << "Dijkstra simulation passed!" << endl << endl;
// }

// // testing the edge case like the empty and full heap
// void testEdgeCases() {
//     cout << "--- Testing Edge Cases ---" << endl;
    
//     MinHeap smallHeap(3);
    
//     // filling it up
//     assert(smallHeap.insert(10, 1));
//     assert(smallHeap.insert(20, 2));
//     assert(smallHeap.insert(5, 3));
    
//     // trying to overflow the heap
//     bool overflowed = smallHeap.insert(15, 4);
//     assert(!overflowed); // expecting to return false
    
//     // emptying the heap
//     smallHeap.extractMin();
//     smallHeap.extractMin();
//     smallHeap.extractMin();
    
//     assert(smallHeap.isEmpty());
    
//     // extracting from an empty heap, expecting to return {-1, -1}
//     HeapNode empty = smallHeap.extractMin();
//     assert(empty.priority == -1 && empty.data == -1);
    
//     cout << "Edge cases passed!" << endl << endl;
// }

// // testing the heavy load with random data
// void testRandomizedStress() {
//     cout << "Testing Randomized Stress Test for 100 elementss" << endl;
    
//     srand(time(0));
    
//     int numElements = 100;
//     MinHeap pq(numElements);
    
//     cout << "Inserting 100 random priorities" << endl;
//     for (int i = 0; i < numElements; i++) {
//         int randomPriority = rand() % 1000;
//         pq.insert(randomPriority, i);
//     }
    
//     assert(pq.size() == 100);
    
//     // extracting all elements and verifying that they come out sorted
//     int lastPriority = -1;
//     bool passed = true;
    
//     for (int i = 0; i < numElements; i++) {
//         HeapNode current = pq.extractMin();
        
//         if (current.priority < lastPriority) {
//             cout << "error: " << current.priority 
//                  << " after " << lastPriority << endl;
//             passed = false;
//             break;
//         }
//         lastPriority = current.priority;
//     }
    
//     if (passed) {
//         cout << "stress test passed with all elements came out in order." << endl;
//     } else {
//         assert(false);
//     }
    
//     cout << endl;
// }

// // the main test runner
// int main() {
//     cout << "**************************************" << endl;
//     cout << "   The Min-Heap Test Suit             " << endl;
//     cout << "**************************************" << endl << endl;

//     testBasicOperations();
//     testDijkstraSimulation();
//     testEdgeCases();
//     testRandomizedStress();

//     cout << "************ all is working as expected - r *******" << endl;
    
//     return 0;
// }
