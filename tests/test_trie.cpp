// test_trie.cpp
// Build: cmake --build build --target test_trie
// Run:   ./build/test_trie

#include "../include/trie.h"
#include <iostream>
using namespace std;

// simple test counters
static int passed = 0;
static int failed = 0;

// simple check helper
void check(const char* label, bool condition) {
    if (condition) {
        cout << "  [PASS] " << label << "\n";
        passed++;
    } else {
        cout << "  [FAIL] " << label << "\n";
        failed++;
    }
}

// test insert and search
void testInsertAndSearch() {
    cout << "\n-- Insert and search tests --\n";

    Trie trie;

    check("insert kigali", trie.insert("kigali") == true);
    check("insert kicukiro", trie.insert("kicukiro") == true);
    check("insert kimironko", trie.insert("kimironko") == true);

    check("search kigali", trie.search("kigali") == true);
    check("search kicukiro", trie.search("kicukiro") == true);
    check("search kimironko", trie.search("kimironko") == true);

    check("search missing word fails", trie.search("kanombe") == false);
}

// test prefix check
void testStartsWith() {
    cout << "\n-- Prefix tests --\n";

    Trie trie;
    trie.insert("ambulance");
    trie.insert("ambassador");
    trie.insert("crime");
    trie.insert("fire");

    check("startsWith amb", trie.startsWith("amb") == true);
    check("startsWith cri", trie.startsWith("cri") == true);
    check("startsWith fi", trie.startsWith("fi") == true);
    check("startsWith pol is false", trie.startsWith("pol") == false);
}

// test remove 
void testRemove() {
    cout << "\n-- Remove tests --\n";

    Trie trie;
    trie.insert("kigali");
    trie.insert("kigoma");

    check("search kigali before remove", trie.search("kigali") == true);
    check("remove kigali", trie.remove("kigali") == true);
    check("search kigali after remove", trie.search("kigali") == false);

    // make sure close word is still there
    check("search kigoma still true", trie.search("kigoma") == true);

    check("remove missing word returns false", trie.remove("kanombe") == false);
}

// test invalid input 
void testInvalidInput() {
    cout << "\n-- Invalid input tests --\n";

    Trie trie;

    check("insert empty word fails", trie.insert("") == false);
    check("search empty word fails", trie.search("") == false);
    check("startsWith empty prefix fails", trie.startsWith("") == false);

    check("insert uppercase fails", trie.insert("Kigali") == false);
    check("insert with digit fails", trie.insert("node1") == false);
}

// test empty trie 
void testIsEmpty() {
    cout << "\n-- Empty trie tests --\n";

    Trie trie;
    check("new trie is empty", trie.isEmpty() == true);

    trie.insert("fire");
    check("trie not empty after insert", trie.isEmpty() == false);

    trie.remove("fire");
    check("trie empty after removing only word", trie.isEmpty() == true);
}

// runner 
int main() {
    cout << "========= Trie Test Suite =========\n";

    testInsertAndSearch();
    testStartsWith();
    testRemove();
    testInvalidInput();
    testIsEmpty();

    cout << "\n===================================\n";
    cout << "Results: " << passed << " passed, " << failed << " failed.\n";

    if (failed > 0) {
        cout << "Some tests FAILED. Check the output above.\n";
        return 1;
    }

    cout << "All tests passed.\n";
    return 0;
}