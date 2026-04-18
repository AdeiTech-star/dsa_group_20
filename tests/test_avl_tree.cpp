#include "../include/avl_tree.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

// Match the testing pattern from min-heap and graph tests
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

// Test basic insertion and search
void testBasicOperations() {
    cout << "\n-- Basic AVL Operations --\n";
    AVLTree tree;
    
    tree.insert(10, 1001);
    tree.insert(20, 1002);
    tree.insert(30, 1003);
    
    int* val = tree.search(20);
    check("search for existing key 20", val != nullptr && *val == 1002);
    
    val = tree.search(10);
    check("search for existing key 10", val != nullptr && *val == 1001);
    
    val = tree.search(40);
    check("search for non-existent key returns null", val == nullptr);
    
    check("tree has 3 nodes", tree.size() == 3);
}

// Test that tree remains balanced after insertions
void testBalanceProperty() {
    cout << "\n-- AVL Balance Property --\n";
    AVLTree tree;
    
    // Insert sequence that would create unbalanced BST
    for (int i = 1; i <= 7; i++) {
        tree.insert(i, i * 100);
    }
    
    int height = tree.getTreeHeight();
    int nodes = tree.size();
    
    // With 7 nodes, a balanced tree should have height 3-4
    check("tree with 7 nodes has reasonable height", height <= 4);
    check("all 7 nodes inserted", nodes == 7);
    
    // Verify all nodes are searchable
    bool allFound = true;
    for (int i = 1; i <= 7; i++) {
        int* val = tree.search(i);
        if (!val || *val != i * 100) {
            allFound = false;
            break;
        }
    }
    check("all inserted nodes are searchable", allFound);
}

// Test duplicate key handling
void testDuplicateKeys() {
    cout << "\n-- Duplicate Key Handling --\n";
    AVLTree tree;
    
    tree.insert(15, 500);
    tree.insert(15, 999);  // same key, different value
    
    int* val = tree.search(15);
    check("duplicate key updates value", val != nullptr && *val == 999);
    check("size is 1 after duplicate insert", tree.size() == 1);
}

// Test temporal range query
void testRangeQuery() {
    cout << "\n-- Temporal Range Query --\n";
    AVLTree logs;
    
    // Simulate incident logs at different times
    logs.insert(8, 501);   // 8 AM
    logs.insert(12, 502);  // 12 PM
    logs.insert(10, 503);  // 10 AM
    logs.insert(15, 504);  // 3 PM
    logs.insert(11, 505);  // 11 AM
    logs.insert(9, 506);   // 9 AM
    
    cout << "  Expected: incidents 503, 505, 502 (times 10, 11, 12)\n";
    logs.printRange(10, 12);
    
    cout << "\n  Expected: incidents 501, 506, 503 (times 8, 9, 10)\n";
    logs.printRange(8, 10);
    
    // Manual verification needed for range queries
    check("range query executed without crash", true);
}

// Test empty tree operations
void testEmptyTree() {
    cout << "\n-- Empty Tree Operations --\n";
    AVLTree tree;
    
    check("empty tree has height 0", tree.getTreeHeight() == 0);
    check("empty tree has size 0", tree.size() == 0);
    check("search in empty tree returns null", tree.search(10) == nullptr);
    
    cout << "  Testing range query on empty tree:\n";
    tree.printRange(1, 100);
    check("range query on empty tree doesn't crash", true);
}

// Test rotations by inserting patterns that force each type
void testRotations() {
    cout << "\n-- Rotation Cases --\n";
    
    // Left-Left case (right rotation needed)
    AVLTree tree1;
    tree1.insert(30, 1);
    tree1.insert(20, 2);
    tree1.insert(10, 3);  // triggers right rotation at 30
    check("left-left case handled", tree1.size() == 3);
    
    // Right-Right case (left rotation needed)
    AVLTree tree2;
    tree2.insert(10, 1);
    tree2.insert(20, 2);
    tree2.insert(30, 3);  // triggers left rotation at 10
    check("right-right case handled", tree2.size() == 3);
    
    // Left-Right case
    AVLTree tree3;
    tree3.insert(30, 1);
    tree3.insert(10, 2);
    tree3.insert(20, 3);  // triggers left-right rotation
    check("left-right case handled", tree3.size() == 3);
    
    // Right-Left case
    AVLTree tree4;
    tree4.insert(10, 1);
    tree4.insert(30, 2);
    tree4.insert(20, 3);  // triggers right-left rotation
    check("right-left case handled", tree4.size() == 3);
}

// Test countRange — programmatic range queries
void testCountRange() {
    cout << "\n-- countRange --\n";
    AVLTree logs;

    logs.insert(5,  501);
    logs.insert(10, 502);
    logs.insert(15, 503);
    logs.insert(20, 504);
    logs.insert(25, 505);

    check("countRange exact match",          logs.countRange(10, 10) == 1);
    check("countRange full span",            logs.countRange(5, 25)  == 5);
    check("countRange middle slice",         logs.countRange(10, 20) == 3);
    check("countRange below all keys",       logs.countRange(1, 4)   == 0);
    check("countRange above all keys",       logs.countRange(30, 50) == 0);
    check("countRange single missing key",   logs.countRange(7, 9)   == 0);
    check("countRange low == high, present", logs.countRange(15, 15) == 1);
    check("countRange low > high returns 0", logs.countRange(20, 10) == 0);
}

// Test that height stays O(log n) after 1000 insertions
void testHeightAfter1000() {
    cout << "\n-- O(log n) height after 1000 insertions --\n";
    AVLTree tree;

    for (int i = 1; i <= 1000; i++)
        tree.insert(i, i);

    int height = tree.getTreeHeight();
    // log2(1000) ≈ 10. AVL guarantee: height <= 1.44 * log2(n+2) ≈ 14.4
    check("height <= 15 after 1000 sequential insertions", height <= 15);
    check("all 1000 nodes present", tree.size() == 1000);
    check("countRange full span = 1000", tree.countRange(1, 1000) == 1000);
}

// Stress test with many insertions
void testStressInsertion() {
    cout << "\n-- Stress Test (100 random insertions) --\n";
    AVLTree tree;
    
    srand(time(0));
    
    // Insert 100 random keys
    for (int i = 0; i < 100; i++) {
        int key = rand() % 1000;
        tree.insert(key, i);
    }
    
    int height = tree.getTreeHeight();
    int nodes = tree.size();
    
    // For 100 nodes, max height should be around 9-10
    check("tree remains balanced after 100 insertions", height <= 10);
    check("tree has at least 50 unique keys", nodes >= 50);  // some duplicates expected
}

// Main test runner
int main() {
    cout << "======================================\n";
    cout << "   AVL Tree Test Suite               \n";
    cout << "======================================\n";
    
    testBasicOperations();
    testBalanceProperty();
    testDuplicateKeys();
    testRangeQuery();
    testCountRange();
    testHeightAfter1000();
    testEmptyTree();
    testRotations();
    testStressInsertion();
    
    cout << "\n======================================\n";
    cout << "Results: " << passed << " passed, " << failed << " failed.\n";
    
    if (failed > 0) {
        cout << "Some tests FAILED. Review output above.\n";
        return 1;
    }
    
    cout << "All tests passed.\n";
    return 0;
}