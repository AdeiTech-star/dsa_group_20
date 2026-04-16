#include <iostream>
#include "../include/segment_tree.h"
using namespace std;

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

// Test 1: single bucket update and query 
void testSingleUpdate() {
    cout << "\n-- Single bucket update --\n";
    SegmentTree st;

    st.update(5, 1);   // one incident at minute 5
    check("query exact bucket returns 1",     st.query(5, 5) == 1);
    check("query outside bucket returns 0",   st.query(0, 4) == 0);
    check("query range including bucket = 1", st.query(0, 10) == 1);
    check("total is 1",                       st.total() == 1);
}

// Test 2: multiple updates same bucket 
void testMultipleUpdates() {
    cout << "\n-- Multiple updates same bucket --\n";
    SegmentTree st;

    st.update(10, 1);
    st.update(10, 1);
    st.update(10, 1);

    check("three incidents at bucket 10",     st.query(10, 10) == 3);
    check("total is 3",                       st.total() == 3);
}

// Test 3: range query across multiple buckets 
void testRangeQuery() {
    cout << "\n-- Range query --\n";
    SegmentTree st;

    st.update(0,  1);    // minute 0
    st.update(50, 1);    // minute 50
    st.update(100, 1);   // minute 100
    st.update(200, 1);   // minute 200

    check("query [0, 100] returns 3",         st.query(0, 100) == 3);
    check("query [50, 200] returns 3",        st.query(50, 200) == 3);
    check("query [51, 199] returns 1",        st.query(51, 199) == 1);
    check("query [0, 0] returns 1",           st.query(0, 0) == 1);
    check("query [201, 500] returns 0",       st.query(201, 500) == 0);
    check("total is 4",                       st.total() == 4);
}

// Test 4: delta -1 removes an incident
void testRemoval() {
    cout << "\n-- Removal with delta -1 --\n";
    SegmentTree st;

    st.update(20, 1);
    st.update(20, 1);
    st.update(20, -1);   // resolve one incident

    check("after removal bucket 20 has 1",    st.query(20, 20) == 1);
    check("total is 1 after removal",         st.total() == 1);
}

// Test 5: full range query 
void testFullRange() {
    cout << "\n-- Full range query --\n";
    SegmentTree st;

    for (int i = 0; i < 10; i++)
        st.update(i * 100, 1);   // 10 incidents spread across buckets

    check("full range [0, 2047] returns 10",
          st.query(0, BUCKETS - 1) == 10);
    check("total matches full range query",
          st.total() == st.query(0, BUCKETS - 1));
}

// Test 6: reset 
void testReset() {
    cout << "\n-- Reset --\n";
    SegmentTree st;

    st.update(5, 1);
    st.update(10, 1);
    st.update(15, 1);

    check("total is 3 before reset",          st.total() == 3);
    st.reset();
    check("total is 0 after reset",           st.total() == 0);
    check("query returns 0 after reset",      st.query(0, 100) == 0);
}

// Test 7: boundary buckets
void testBoundaries() {
    cout << "\n-- Boundary buckets --\n";
    SegmentTree st;

    st.update(0, 1);              // first bucket
    st.update(BUCKETS - 1, 1);   // last bucket

    check("first bucket query",       st.query(0, 0) == 1);
    check("last bucket query",        st.query(BUCKETS-1, BUCKETS-1) == 1);
    check("full range includes both", st.query(0, BUCKETS-1) == 2);
    check("middle range excludes both",
          st.query(1, BUCKETS-2) == 0);
}

// Test 8: stress test — 1000 updates 
void testStress() {
    cout << "\n-- Stress test (1000 updates) --\n";
    SegmentTree st;

    // Insert 1000 incidents spread across first 500 buckets
    for (int i = 0; i < 1000; i++)
        st.update(i % 500, 1);

    check("total is 1000",                    st.total() == 1000);
    check("first 500 buckets hold all 1000",
          st.query(0, 499) == 1000);
    check("last 1548 buckets hold 0",
          st.query(500, BUCKETS - 1) == 0);

    // Each of the first 500 buckets should have exactly 2
    bool allTwo = true;
    for (int i = 0; i < 500; i++) {
        if (st.query(i, i) != 2) {
            allTwo = false;
            break;
        }
    }
    check("each of first 500 buckets has exactly 2", allTwo);
}

// Test 9: COUNT_WINDOW simulation 
// Simulates the actual dispatcher use case:
// REPORT_INCIDENT uses timestamp % BUCKETS as the bucket index
// COUNT_WINDOW queries a range of buckets
void testCountWindowSimulation() {
    cout << "\n-- COUNT_WINDOW simulation --\n";
    SegmentTree st;

    // Simulate 10 incidents at various timestamps
    int timestamps[] = {60, 61, 62, 120, 121, 300, 301, 302, 303, 500};
    for (int i = 0; i < 10; i++)
        st.update(timestamps[i] % BUCKETS, 1);

    // How many incidents in minutes 60-130?
    check("incidents in window [60, 130] = 5",  st.query(60, 130) == 5);

    // How many incidents in minutes 300-400?
    check("incidents in window [300, 400] = 4", st.query(300, 400) == 4);

    // How many incidents in minutes 0-59?
    check("incidents in window [0, 59] = 0",    st.query(0, 59) == 0);

    // Total
    check("total incidents = 10",               st.total() == 10);
}

// Main 
int main() {
    cout << "----------------------------------------\n";
    cout << "   Segment Tree Test Suite              \n";
    cout << "----------------------------------------\n";

    testSingleUpdate();
    testMultipleUpdates();
    testRangeQuery();
    testRemoval();
    testFullRange();
    testReset();
    testBoundaries();
    testStress();
    testCountWindowSimulation();

    cout << "\n--------------------------------------\n";
    cout << "Results: " << passed << " passed, "
         << failed << " failed.\n";

    if (failed > 0) {
        cout << "Some tests FAILED. Review output above.\n";
        return 1;
    }

    cout << "All tests passed.\n";
    return 0;
}