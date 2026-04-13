#include "hash_table.h"
#include <iostream>

// Print helpers (no iostream) 
static void printStr(const char* s) {
    while (*s) putchar(*s++);
}

static void printInt(int n) {
    if (n < 0) { putchar('-'); n = -n; }
    if (n >= 10) printInt(n / 10);
    putchar('0' + n % 10);
}

static void printLine(const char* s) {
    printStr(s);
    putchar('\n');
}

// Test reporter
static int passed = 0;
static int failed = 0;

static void check(const char* name, bool result) {
    if (result) {
        printStr("[PASS] ");
        passed++;
    } else {
        printStr("[FAIL] ");
        failed++;
    }
    printLine(name);
}

// Tests
void testInsertAndLookup() {
    HashTable ht(11);
    ht.insert(1, 100);
    ht.insert(2, 200);
    ht.insert(3, 300);

    check("lookup existing key 1",
          ht.lookup(1) != nullptr && *ht.lookup(1) == 100);
    check("lookup existing key 2",
          ht.lookup(2) != nullptr && *ht.lookup(2) == 200);
    check("lookup non-existent key returns nullptr",
          ht.lookup(99) == nullptr);
}

void testUpdate() {
    HashTable ht(11);
    ht.insert(5, 50);
    ht.insert(5, 99);   // same key — should update value

    check("update existing key",
          ht.lookup(5) != nullptr && *ht.lookup(5) == 99);
    check("update does not increase size",
          ht.size() == 1);
}

void testTombstone() {
    // Remove a key that other keys probed past.
    // If the slot is marked EMPTY instead of DELETED,
    // lookups for those other keys will falsely return nullptr.
    HashTable ht(11);
    ht.insert(5,  10);
    ht.insert(16, 20);   // collision with 5 — probes past it
    ht.remove(5);         // slot becomes tombstone, not EMPTY

    check("lookup past tombstone still works",
          ht.lookup(16) != nullptr && *ht.lookup(16) == 20);
    check("removed key returns nullptr",
          ht.lookup(5) == nullptr);
}

void testRemove() {
    HashTable ht(11);
    ht.insert(7, 77);

    check("remove returns true for existing key",  ht.remove(7));
    check("removed key no longer found",           ht.lookup(7) == nullptr);
    check("size decreases after remove",           ht.size() == 0);
    check("remove returns false for missing key",  ht.remove(99) == false);
}

void testResize() {
    // Small initial capacity — 8 inserts forces a resize
    HashTable ht(7);
    for (int i = 0; i < 8; i++)
        ht.insert(i * 13, i);

    bool allFound = true;
    for (int i = 0; i < 8; i++) {
        if (ht.lookup(i * 13) == nullptr || *ht.lookup(i * 13) != i) {
            allFound = false;
            break;
        }
    }
    check("all keys reachable after resize", allFound);
}

void testNegativeKeys() {
    HashTable ht(11);
    ht.insert(-3, 42);
    ht.insert(-7, 84);

    check("negative key -3",
          ht.lookup(-3) != nullptr && *ht.lookup(-3) == 42);
    check("negative key -7",
          ht.lookup(-7) != nullptr && *ht.lookup(-7) == 84);
}

void testLoadFactor() {
    HashTable ht(101);
    check("initial load is 0",       ht.load() == 0.0f);
    ht.insert(1, 1);
    check("load increases after insert", ht.load() > 0.0f);
}

// Main
int main() {
    printLine("=== Hash Table Tests ===\n");

    testInsertAndLookup();
    testUpdate();
    testTombstone();
    testRemove();
    testResize();
    testNegativeKeys();
    testLoadFactor();

    printLine("\n--- Results ---");
    printStr("Passed: "); printInt(passed); putchar('\n');
    printStr("Failed: "); printInt(failed); putchar('\n');

    return failed == 0 ? 0 : 1;
}