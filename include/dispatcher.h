#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "graph.h"
#include "min_heap.h"
#include "hash_table.h"
#include "avl_tree.h"
#include "segment_tree.h"
#include "union_find.h"
#include "trie.h"
#include "dijkstra.h"

// Limits 
const int MAX_UNITS     = 50;
const int MAX_INCIDENTS = 10500;  // supports 10,000-op benchmark + headroom

// Enumerations
enum UnitType     { POLICE = 0, AMBULANCE = 1, FIRE_TRUCK = 2 };
enum IncidentType { CRIME  = 0, MEDICAL   = 1, FIRE       = 2, ACCIDENT = 3 };

// Data Records 

struct Unit {
    int      id;
    char     name[32];
    UnitType type;
    int      locationVertex;  // intersection where the unit currently is
    bool     available;       // false while assigned to an open incident
    bool     active;          // false = this slot is unused
};

struct Incident {
    int          id;
    IncidentType type;
    int          locationVertex;
    int          severity;        // 1 (low) to 10 (critical)
    int          reportTime;      // simulation minute when reported
    int          resolveTime;     // simulation minute when closed (0 if open)
    bool         resolved;
    bool         active;          // false = slot unused
    int          assignedUnitId;  // -1 if not yet dispatched
};

// Dispatcher 
//
// Orchestrates all data structures:
//
//   MinHeap      — triage: always handle the highest-severity incident first
//   HashTable    — O(1) lookup of units and incidents by ID
//   AVLTree      — temporal log: range queries over incident timestamps
//   SegmentTree  — time-window analytics: count incidents per minute bucket
//   UnionFind    — fast connectivity gate before running Dijkstra
//   Graph        — city road network with dynamic add/remove
//   Dijkstra     — shortest-path routing from unit to incident
//   Trie         — prefix search on unit names (e.g. "amb" → all ambulances)
//
class Dispatcher {
private:
    // city map (shared reference) 
    Graph& cityMap;

    // all eight data structures 
    MinHeap     incidentQueue;   // priority queue for triage
    HashTable   incidentTable;   // incidentId  → index in incidents[]
    HashTable   unitTable;       // unitId      → index in units[]
    AVLTree     incidentLog;     // reportTime  → incidentId (range queries)
    SegmentTree timeWindow;      // minute bucket → incident count
    UnionFind   connectivity;    // reachability pre-check
    Trie        unitNameTrie;    // prefix search on unit names

    // plain-array storage (no STL)
    Unit     units[MAX_UNITS];
    int      unitCount;

    Incident incidents[MAX_INCIDENTS];
    int      incidentCount;
    int      nextIncidentId;

    // analytics 
    int       visitCount[MAX_VERTICES]; // how many routes passed through each vertex
    long long totalResponseTime;        // sum of (resolveTime - reportTime)
    int       resolvedCount;

    // simulation clock 
    int currentTime;

    // private helpers
    int         heapPriority(int severity, int incidentId) const;
    void        recordPathVisits(DijkstraResult& result, int dest);
    void        printDivider()   const;
    const char* unitTypeName(UnitType t)         const;
    const char* incidentTypeName(IncidentType t) const;

    // Lowercase-copy a name into dst (for trie insertion — trie is a-z only)
    void toLowerCopy(const char* src, char* dst, int maxLen) const;

public:
    explicit Dispatcher(Graph& g);
    ~Dispatcher();

    // simulation clock 
    void tick(int minutes = 1);  // advance simulation time
    int  getTime()        const;

    // unit management 
    bool addUnit(int id, const char* name, UnitType type, int locationVertex);
    bool relocateUnit(int unitId, int newVertex);
    void printUnits()     const;

    // incident lifecycle
    // reportIncident — logs incident to all structures, returns its assigned ID
    int  reportIncident(IncidentType type, int locationVertex, int severity);

    // dispatchUnit — sends a specific unit to a specific incident
    bool dispatchUnit(int unitId, int incidentId);

    // autoDispatch — picks the highest-priority open incident, finds the
    //                closest available unit, and dispatches automatically
    bool autoDispatch();

    // resolveIncident — closes incident, frees unit, records response time
    bool resolveIncident(int incidentId);

    void printIncidents() const;

    // road updates 
    bool closeRoad(int srcId, int destId);
    bool reopenRoad(int srcId, int destId, int weight);

    // search 
    // Find all units whose names start with the given prefix
    void searchUnitsByPrefix(const char* prefix) const;

    // analytics
    // Count open incidents reported in [startMinute, endMinute].
    // Uses SegmentTree — O(log BUCKETS). Only counts unresolved incidents.
    int    countIncidentsInWindow(int startMinute, int endMinute) const;

    // Count ALL incidents (including resolved) reported in [startMinute, endMinute].
    // Uses AVLTree::countRange — O(log n + k). Useful for post-incident reports.
    int    countAllIncidentsInWindow(int startMinute, int endMinute) const;

    int    getBusiestIntersection() const;
    double getAvgResponseTime()     const;
    void   printTopIntersections(int n) const;

    // display
    void printStatus()    const;
    void printAnalytics() const;
};

#endif
