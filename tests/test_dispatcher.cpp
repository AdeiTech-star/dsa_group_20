#include "../include/dispatcher.h"
#include "../include/graph.h"
#include <iostream>
#include <cassert>
using namespace std;

// -----------------------------------------------------------------------------
//  Shared city map
//
//  Kigali mini-map (9 vertices, 11 roads):
//
//    0 ---5--- 1 ---3--- 2
//    |         |         |
//    7         4         6
//    |         |         |
//    3 ---2--- 4 ---8--- 5 ---5--- 8
//              |
//              9
//              |
//              6 ---1--- 7
//
// -----------------------------------------------------------------------------
Graph buildKigaliMap() {
    Graph g;
    initGraph(g);
    for (int i = 0; i <= 8; i++) addVertex(g, i);

    addEdge(g, 0, 1, 5);
    addEdge(g, 0, 3, 7);
    addEdge(g, 1, 2, 3);
    addEdge(g, 1, 4, 4);
    addEdge(g, 2, 5, 6);
    addEdge(g, 3, 4, 2);
    addEdge(g, 4, 5, 8);
    addEdge(g, 4, 6, 9);
    addEdge(g, 6, 7, 1);
    addEdge(g, 7, 8, 3);
    addEdge(g, 5, 8, 5);
    return g;
}

// -----------------------------------------------------------------------------
//  SCENARIO 1 — Mass Casualty Event
//
//  Demonstrates:
//    - Reporting multiple incidents of different severities
//    - MinHeap triage (highest severity dispatched first)
//    - HashTable O(1) lookups
//    - Dijkstra routing with path reconstruction
//    - AVLTree temporal log
//    - resolveIncident + response time tracking
// -----------------------------------------------------------------------------
void scenario1_MassCasualty() {
    cout << "\n\n";
    cout << "-----------------------------------------------------\n";
    cout << "  SCENARIO 1: MASS CASUALTY EVENT\n";
    cout << "-----------------------------------------------------\n";

    Graph g = buildKigaliMap();
    Dispatcher d(g);

    // Station three units across the city
    d.addUnit(1, "Police01",    POLICE,     0);
    d.addUnit(2, "Ambulance01", AMBULANCE,  3);
    d.addUnit(3, "FireTruck01", FIRE_TRUCK, 2);

    cout << "\n-- Five incidents come in simultaneously --\n\n";

    d.tick(10);
    int i1 = d.reportIncident(ACCIDENT, 7, 4);   // low severity
    d.tick(1);
    int i2 = d.reportIncident(FIRE,     5, 9);   // critical
    d.tick(1);
    int i3 = d.reportIncident(CRIME,    1, 6);   // medium
    d.tick(1);
    int i4 = d.reportIncident(MEDICAL,  8, 8);   // high
    d.tick(1);
    int i5 = d.reportIncident(ACCIDENT, 4, 7);   // high

    d.printStatus();

    cout << "\n-- autoDispatch x3: heap routes in severity order --\n\n";
    d.autoDispatch();   // picks i2 (severity 9)
    d.tick(5);
    d.autoDispatch();   // picks i4 (severity 8)
    d.tick(5);
    d.autoDispatch();   // picks i5 (severity 7)

    d.printStatus();

    cout << "\n-- Resolving two incidents --\n\n";
    d.tick(8);
    d.resolveIncident(i2);
    d.tick(3);
    d.resolveIncident(i4);

    d.printAnalytics();
    destroyGraph(g);
}

// -----------------------------------------------------------------------------
//  SCENARIO 2 — Road Closure & Rerouting
//
//  Demonstrates:
//    - Dynamic edge removal from Graph
//    - UnionFind rebuild after closure  (O(V+E))
//    - UnionFind connectivity gate before Dijkstra (O(alpha))
//    - Dispatcher warning about blocked in-progress dispatches
//    - Dijkstra finding alternative path after reopening
// -----------------------------------------------------------------------------
void scenario2_RoadClosure() {
    cout << "\n\n";
    cout << "-----------------------------------------------------\n";
    cout << "  SCENARIO 2: ROAD CLOSURE & REROUTING\n";
    cout << "-----------------------------------------------------\n";

    Graph g = buildKigaliMap();
    Dispatcher d(g);

    d.addUnit(10, "Ambulance02", AMBULANCE,  0);
    d.addUnit(11, "FireTruck02", FIRE_TRUCK, 1);

    d.tick(20);
    d.reportIncident(FIRE, 8, 8);

    cout << "\n-- Initial dispatch (all roads open) --\n\n";
    d.autoDispatch();

    // Close the main corridor linking the lower half of the map
    cout << "\n-- Road closure: vertex 4 <-> vertex 5 (main corridor) --\n\n";
    d.tick(5);
    d.closeRoad(4, 5);

    // New incident on the now-isolated side
    d.tick(2);
    d.reportIncident(MEDICAL, 7, 9);

    cout << "\n-- autoDispatch with reduced network --\n\n";
    d.autoDispatch();

    // Reopen and retry
    cout << "\n-- Road reopened --\n\n";
    d.tick(3);
    d.reopenRoad(4, 5, 8);

    cout << "\n-- autoDispatch after road restored --\n\n";
    d.autoDispatch();

    d.printStatus();
    destroyGraph(g);
}

// -----------------------------------------------------------------------------
//  SCENARIO 3 — Temporal Analytics
//
//  Demonstrates:
//    - SegmentTree time-window incident counts  (O(log n))
//    - AVLTree range query over timestamps      (O(log n))
//    - Trie prefix search on unit names         (O(prefix length))
//    - Busiest intersection after many dispatches
//    - Average response time across resolved incidents
// -----------------------------------------------------------------------------
void scenario3_TemporalAnalytics() {
    cout << "\n\n";
    cout << "-----------------------------------------------------\n";
    cout << "  SCENARIO 3: TEMPORAL ANALYTICS\n";
    cout << "-----------------------------------------------------\n";

    Graph g = buildKigaliMap();
    Dispatcher d(g);

    // Mix of unit types — names designed to show prefix search
    d.addUnit(20, "ambulanceA", AMBULANCE,  0);
    d.addUnit(21, "ambulanceB", AMBULANCE,  3);
    d.addUnit(22, "policeX",    POLICE,     1);
    d.addUnit(23, "policeY",    POLICE,     4);
    d.addUnit(24, "fireunit",   FIRE_TRUCK, 2);

    // Spread incidents across two time windows
    d.tick(0);
    int a = d.reportIncident(CRIME,    1, 5);
    d.tick(5);
    int b = d.reportIncident(MEDICAL,  4, 8);
    d.tick(5);
    int c = d.reportIncident(FIRE,     5, 9);
    d.tick(30);
    int e = d.reportIncident(ACCIDENT, 7, 6);
    d.tick(5);
    int f = d.reportIncident(CRIME,    8, 7);
    d.tick(5);
    int h = d.reportIncident(MEDICAL,  2, 4);

    cout << "\n-- Dispatching all incidents --\n\n";
    for (int i = 0; i < 6; i++) {
        d.autoDispatch();
        d.tick(2);
    }

    cout << "\n-- Resolving incidents --\n\n";
    d.tick(10);
    d.resolveIncident(a);
    d.tick(4);
    d.resolveIncident(b);
    d.tick(6);
    d.resolveIncident(c);
    d.tick(2);
    d.resolveIncident(e);
    d.tick(3);
    d.resolveIncident(f);

    // SegmentTree: count open incidents by time window 
    cout << "\n-- SegmentTree: incident counts by time window --\n\n";
    int earlyCount = d.countIncidentsInWindow(0, 15);
    int lateCount  = d.countIncidentsInWindow(40, 70);
    cout << "  Open incidents in T+0  to T+15 : " << earlyCount << "\n";
    cout << "  Open incidents in T+40 to T+70 : " << lateCount  << "\n";

    // Trie: prefix search 
    cout << "\n-- Trie: prefix search on unit names --\n\n";
    d.searchUnitsByPrefix("amb");
    d.searchUnitsByPrefix("police");
    d.searchUnitsByPrefix("fire");

    // Full analytics
    d.printAnalytics();
    d.printStatus();

    destroyGraph(g);
}

// -----------------------------------------------------------------------------
//  CORRECTNESS TESTS
// -----------------------------------------------------------------------------
static int passed = 0;
static int failed = 0;

void check(bool condition, const char* label) {
    if (condition) {
        cout << "  [PASS] " << label << "\n";
        passed++;
    } else {
        cout << "  [FAIL] " << label << "\n";
        failed++;
    }
}

void correctnessTests() {
    cout << "\n\n------------------------------------------------\n";
    cout << "  CORRECTNESS TESTS\n";
    cout << "----------------------------------------------------\n";

    Graph g = buildKigaliMap();
    Dispatcher d(g);

    // Unit registration
    check(d.addUnit(1, "unitA", POLICE,    0) == true,  "addUnit succeeds");
    check(d.addUnit(1, "unitB", AMBULANCE, 1) == false, "duplicate unit ID rejected");
    check(d.addUnit(2, "unitB", AMBULANCE, 1) == true,  "second unit registered");

    // Incident reporting
    d.tick(5);
    int id = d.reportIncident(FIRE, 8, 9);
    check(id == 1,                               "first incident ID is 1");
    check(d.reportIncident(CRIME, 99, 5) == -1,  "invalid vertex rejected");
    check(d.reportIncident(MEDICAL, 5, 11) == -1,"severity > 10 rejected");

    // Dispatch
    check(d.dispatchUnit(1, id) == true,   "valid dispatch succeeds");
    check(d.dispatchUnit(1, id) == false,  "busy unit cannot be re-dispatched");
    check(d.dispatchUnit(9, id) == false,  "unknown unit rejected");

    // Resolve
    d.tick(10);
    check(d.resolveIncident(id)  == true,  "resolve open incident");
    check(d.resolveIncident(id)  == false, "resolve already-resolved fails");
    check(d.resolveIncident(999) == false, "resolve unknown incident fails");
    check(d.getAvgResponseTime() == 10.0,  "avg response time = 10 min");

    // Road closure blocks dispatch
    d.closeRoad(0, 3);
    d.closeRoad(1, 4);
    d.closeRoad(0, 1);   // vertex 0 now fully isolated
    d.addUnit(3, "unitC", AMBULANCE, 5);
    int id2 = d.reportIncident(MEDICAL, 0, 8);
    check(d.dispatchUnit(3, id2) == false, "dispatch blocked by disconnected network");

    // Reopen road restores connectivity
    d.reopenRoad(0, 1, 5);
    d.reopenRoad(1, 4, 4);
    d.reopenRoad(0, 3, 7);
    check(d.dispatchUnit(3, id2) == true,  "dispatch succeeds after road restored");

    destroyGraph(g);

    cout << "\n--- Results: " << passed << " passed, " << failed << " failed ---\n";
}

// -----------------------------------------------------------------------------
int main() {
    scenario1_MassCasualty();
    scenario2_RoadClosure();
    scenario3_TemporalAnalytics();
    correctnessTests();
    return failed == 0 ? 0 : 1;
}
