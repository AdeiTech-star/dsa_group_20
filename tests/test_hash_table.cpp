#include <iostream>
#include "hash_table.h"

static int passed = 0;
static int failed = 0;

void check(const char* name, bool result) {
    if (result) {
        cout << "[PASS] " << name << "\n";
        passed++;
    } else {
        cout << "[FAIL] " << name << "\n";
        failed++;
    }
}

// Tests 

void testInsertAndLookup() {
    HashTable ht(7);
    ht.insert(5,  100);
    ht.insert(18, 200);   // collides with 5 — goes into same chain
    ht.insert(31, 300);   // also collides — third node in chain

    check("lookup key=5",
          ht.lookup(5)  != nullptr && *ht.lookup(5)  == 100);
    check("lookup key=18",
          ht.lookup(18) != nullptr && *ht.lookup(18) == 200);
    check("lookup key=31",
          ht.lookup(31) != nullptr && *ht.lookup(31) == 300);
    check("lookup missing key returns nullptr",
          ht.lookup(99) == nullptr);
}

void testUpdate() {
    HashTable ht(7);
    ht.insert(5, 100);
    ht.insert(5, 999);   // same key — should update value in chain

    check("update value in chain",
          ht.lookup(5) != nullptr && *ht.lookup(5) == 999);
    check("size stays 1 after update",
          ht.size() == 1);
}

void testRemove() {
    HashTable ht(7);
    ht.insert(5,  100);
    ht.insert(18, 200);   // same slot as 5
    ht.insert(31, 300);   // same slot as 5 and 18

    // remove the middle node — no tombstone needed
    bool removed = ht.remove(18);
    check("remove returns true",          removed);
    check("removed key not found",        ht.lookup(18) == nullptr);
    check("other keys in chain intact",   ht.lookup(5)  != nullptr);
    check("other keys in chain intact 2", ht.lookup(31) != nullptr);
    check("size decreases",               ht.size() == 2);
}

void testRemoveHead() {
    HashTable ht(7);
    ht.insert(5,  100);
    ht.insert(18, 200);

    ht.remove(18);   // 18 was prepended so it is the head
    check("remove head of chain",
          ht.lookup(18) == nullptr && ht.lookup(5) != nullptr);
}

void testNoDeletionBug() {
    // This is the key test that shows chaining has NO tombstone problem.
    // Removing a key never breaks lookup for other keys in the same chain.
    HashTable ht(7);
    ht.insert(5,  10);
    ht.insert(18, 20);
    ht.insert(31, 30);

    ht.remove(5);    // remove head
    ht.remove(31);   // remove tail

    check("middle key still found after head+tail removed",
          ht.lookup(18) != nullptr && *ht.lookup(18) == 20);
    check("removed keys are gone",
          ht.lookup(5) == nullptr && ht.lookup(31) == nullptr);
}

void testNegativeKeys() {
    HashTable ht(7);
    ht.insert(-3, 42);
    ht.insert(-7, 84);

    check("negative key -3",
          ht.lookup(-3) != nullptr && *ht.lookup(-3) == 42);
    check("negative key -7",
          ht.lookup(-7) != nullptr && *ht.lookup(-7) == 84);
}

void testResize() {
    HashTable ht(3);   // tiny table — forces resize quickly
    for (int i = 0; i < 5; i++)
        ht.insert(i * 7, i);

    bool allFound = true;
    for (int i = 0; i < 5; i++) {
        if (ht.lookup(i * 7) == nullptr || *ht.lookup(i * 7) != i) {
            allFound = false;
            break;
        }
    }
    check("all keys found after resize", allFound);
}

// Main
int main() {
    cout << "=== Hash Table Tests (chaining) ===\n\n";

    testInsertAndLookup();
    testUpdate();
    testRemove();
    testRemoveHead();
    testNoDeletionBug();
    testNegativeKeys();
    testResize();

    cout << "\n--- Results ---\n";
    cout << "Passed: " << passed << "\n";
    cout << "Failed: " << failed << "\n";

    return failed == 0 ? 0 : 1;
}