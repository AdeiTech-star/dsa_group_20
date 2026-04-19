#include "../include/dispatcher.h"
#include <iostream>
using namespace std;

// -----------------------------------------------------------------------------
//  CONSTRUCTOR / DESTRUCTOR
// -----------------------------------------------------------------------------

Dispatcher::Dispatcher(Graph& g)
    : cityMap(g),
      incidentQueue(MAX_INCIDENTS),
      incidentTable(101),
      unitTable(53),
      unitCount(0),
      incidentCount(0),
      nextIncidentId(1),
      totalResponseTime(0),
      resolvedCount(0),
      currentTime(0)
{
    // Blank out all unit and incident slots
    for (int i = 0; i < MAX_UNITS; i++) {
        units[i].active    = false;
        units[i].available = false;
    }
    for (int i = 0; i < MAX_INCIDENTS; i++) {
        incidents[i].active   = false;
        incidents[i].resolved = false;
    }

    // Zero the visit-count analytics array
    for (int i = 0; i < MAX_VERTICES; i++) {
        visitCount[i] = 0;
    }

    // Build the initial connectivity map from whatever edges are in the graph
    connectivity.buildFromGraph(cityMap);
}

Dispatcher::~Dispatcher() {
    // All members manage their own memory via their own destructors.
    // Nothing extra to free here.
}

// -----------------------------------------------------------------------------
//  PRIVATE HELPERS
// -----------------------------------------------------------------------------

// heapPriority
// Maps severity (1–10) to a min-heap key.
// Severity 10 → key = 0      (extracted first — most urgent)
// Severity  1 → key = 90000  (extracted last  — least urgent)
// incidentId breaks ties so older incidents are handled before newer ones
// at the same severity level.
int Dispatcher::heapPriority(int severity, int incidentId) const {
    return (10 - severity) * 10000 + incidentId;
}

// recordPathVisits
// Walks parent[] backwards from dest to source and increments visitCount
// for every vertex on the route. Called after every successful dispatch.
void Dispatcher::recordPathVisits(DijkstraResult& result, int dest) {
    int curr = dest;
    while (curr != -1) {
        if (curr < 0 || curr >= MAX_VERTICES) break;
        visitCount[curr]++;
        curr = result.parent[curr];
    }
}

void Dispatcher::printDivider() const {
    cout << "-----------------------------------------------------\n";
}

const char* Dispatcher::unitTypeName(UnitType t) const {
    switch (t) {
        case POLICE:    return "Police";
        case AMBULANCE: return "Ambulance";
        case FIRE_TRUCK:return "Fire Truck";
        default:        return "Unknown";
    }
}

const char* Dispatcher::incidentTypeName(IncidentType t) const {
    switch (t) {
        case CRIME:    return "Crime";
        case MEDICAL:  return "Medical";
        case FIRE:     return "Fire";
        case ACCIDENT: return "Accident";
        default:       return "Unknown";
    }
}

// toLowerCopy
// Normalises a unit name for trie storage and lookup.
//
// The trie only accepts lowercase a-z characters, but real unit names contain
// uppercase letters and non-alpha characters such as hyphens and digits
// (e.g. "AMB-01", "FIRE-03", "Police01").
//
// This function:
//   1. Converts uppercase A-Z to lowercase a-z.
//   2. Silently drops any character that is not a-z (hyphens, digits, spaces).
//
// Effect on search:
//   Inserting  "AMB-01"  stores  "amb"   in the trie.
//   Searching prefix "AMB" normalises to "amb" and finds it correctly.
//   Searching prefix "amb" also finds it — the two queries are equivalent.
//
// Trade-off: "AMB01" and "AMB-01" both normalise to "amb" and collide in the
// trie. For this system all unit names are distinct enough that this is not a
// problem. A production system would use a case-insensitive trie with a wider
// alphabet to avoid the collision.
void Dispatcher::toLowerCopy(const char* src, char* dst, int maxLen) const {
    int di = 0;
    for (int si = 0; src[si] != '\0' && di < maxLen - 1; si++) {
        char c = src[si];
        if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
        if (c >= 'a' && c <= 'z') {
            dst[di++] = c;
        }
    }
    dst[di] = '\0';
}

// -----------------------------------------------------------------------------
//  SIMULATION CLOCK
// -----------------------------------------------------------------------------

void Dispatcher::tick(int minutes) {
    currentTime += minutes;
}

int Dispatcher::getTime() const {
    return currentTime;
}

// -----------------------------------------------------------------------------
//  UNIT MANAGEMENT
// -----------------------------------------------------------------------------

// addUnit
// Registers a new emergency unit with the dispatcher.
// Uses: HashTable (fast lookup), Trie (name search)
bool Dispatcher::addUnit(int id, const char* name, UnitType type, int locationVertex) {
    if (unitCount >= MAX_UNITS) {
        cout << "Error: unit roster is full (max " << MAX_UNITS << ").\n";
        return false;
    }
    if (!cityMap.active[locationVertex]) {
        cout << "Error: vertex " << locationVertex << " is not in the city map.\n";
        return false;
    }
    if (unitTable.lookup(id) != nullptr) {
        cout << "Error: unit ID " << id << " already exists.\n";
        return false;
    }

    // Find an empty slot in the array
    int slot = -1;
    for (int i = 0; i < MAX_UNITS; i++) {
        if (!units[i].active) { slot = i; break; }
    }
    if (slot == -1) return false;

    // Fill the slot
    units[slot].id             = id;
    units[slot].type           = type;
    units[slot].locationVertex = locationVertex;
    units[slot].available      = true;
    units[slot].active         = true;

    // Copy name (manual — no STL)
    int ni = 0;
    while (name[ni] != '\0' && ni < 31) {
        units[slot].name[ni] = name[ni];
        ni++;
    }
    units[slot].name[ni] = '\0';

    unitCount++;
    unitTable.insert(id, slot);

    // Insert lowercase version of name into trie for prefix search
    char lower[32];
    toLowerCopy(name, lower, 32);
    if (lower[0] != '\0') {
        unitNameTrie.insert(lower);
    }

    cout << "[UNIT]     " << unitTypeName(type) << " '" << name
         << "' (ID " << id << ") stationed at vertex " << locationVertex << "\n";
    return true;
}

// relocateUnit
// Moves a unit to a new intersection (e.g. after completing a patrol).
bool Dispatcher::relocateUnit(int unitId, int newVertex) {
    int* slotPtr = unitTable.lookup(unitId);
    if (slotPtr == nullptr) {
        cout << "Error: unit " << unitId << " not found.\n";
        return false;
    }
    if (!cityMap.active[newVertex]) {
        cout << "Error: vertex " << newVertex << " is not in the city map.\n";
        return false;
    }
    units[*slotPtr].locationVertex = newVertex;
    cout << "[RELOCATE] Unit " << unitId << " moved to vertex " << newVertex << "\n";
    return true;
}

void Dispatcher::printUnits() const {
    cout << "\n--- Units (" << unitCount << ") ---\n";
    for (int i = 0; i < MAX_UNITS; i++) {
        if (!units[i].active) continue;
        cout << "  [" << units[i].id << "] "
             << unitTypeName(units[i].type) << " '"
             << units[i].name << "'  vertex=" << units[i].locationVertex
             << (units[i].available ? "  AVAILABLE" : "  BUSY") << "\n";
    }
}

// -----------------------------------------------------------------------------
//  INCIDENT LIFECYCLE
// -----------------------------------------------------------------------------

// reportIncident
// Logs a new incident to ALL relevant data structures:
//   MinHeap     — queues it for triage
//   HashTable   — enables O(1) lookup by ID
//   AVLTree     — logs it by timestamp for range queries
//   SegmentTree — increments the corresponding time bucket
//
// Returns the new incident's ID, or -1 on failure.
int Dispatcher::reportIncident(IncidentType type, int locationVertex, int severity) {
    if (incidentCount >= MAX_INCIDENTS) {
        cout << "Error: incident log is full.\n";
        return -1;
    }
    if (!cityMap.active[locationVertex]) {
        cout << "Error: vertex " << locationVertex << " is not in the city map.\n";
        return -1;
    }
    if (severity < 1 || severity > 10) {
        cout << "Error: severity must be between 1 and 10.\n";
        return -1;
    }

    // Find an empty slot
    int slot = -1;
    for (int i = 0; i < MAX_INCIDENTS; i++) {
        if (!incidents[i].active) { slot = i; break; }
    }
    if (slot == -1) return -1;

    int id = nextIncidentId++;

    // Fill the record
    incidents[slot].id             = id;
    incidents[slot].type           = type;
    incidents[slot].locationVertex = locationVertex;
    incidents[slot].severity       = severity;
    incidents[slot].reportTime     = currentTime;
    incidents[slot].resolveTime    = 0;
    incidents[slot].resolved       = false;
    incidents[slot].active         = true;
    incidents[slot].assignedUnitId = -1;
    incidentCount++;

    // 1. MinHeap — triage queue
    incidentQueue.insert(heapPriority(severity, id), id);

    // 2. HashTable — O(1) lookup
    incidentTable.insert(id, slot);

    // 3. AVLTree — temporal log (key = reportTime, value = incidentId)
    incidentLog.insert(currentTime, id);

    // 4. SegmentTree — time-window analytics
    timeWindow.update(currentTime % BUCKETS, +1);

    printDivider();
    cout << "[INCIDENT] #" << id << " reported  |  "
         << incidentTypeName(type) << "  |  severity " << severity << "/10"
         << "  |  vertex " << locationVertex
         << "  |  T+" << currentTime << " min\n";
    printDivider();

    return id;
}

// dispatchUnit
// Sends a specific unit to a specific incident.
// Pipeline:
//   1. Validate unit and incident exist and are eligible
//   2. UnionFind connectivity pre-check (fast O(alpha) gate)
//   3. Dijkstra — find and print the actual shortest path
//   4. Mark unit unavailable, link to incident
//   5. Record route in visitCount[] for analytics
bool Dispatcher::dispatchUnit(int unitId, int incidentId) {
    // Look up unit 
    int* uSlotPtr = unitTable.lookup(unitId);
    if (uSlotPtr == nullptr) {
        cout << "Error: unit " << unitId << " not found.\n";
        return false;
    }
    Unit& unit = units[*uSlotPtr];
    if (!unit.available) {
        cout << "Error: unit " << unitId << " is already assigned to another incident.\n";
        return false;
    }

    // Look up incident 
    int* iSlotPtr = incidentTable.lookup(incidentId);
    if (iSlotPtr == nullptr) {
        cout << "Error: incident " << incidentId << " not found.\n";
        return false;
    }
    Incident& inc = incidents[*iSlotPtr];
    if (inc.resolved) {
        cout << "Error: incident " << incidentId << " is already resolved.\n";
        return false;
    }
    if (inc.assignedUnitId != -1) {
        cout << "Error: incident " << incidentId << " already has unit "
             << inc.assignedUnitId << " assigned.\n";
        return false;
    }

    // Step 1: UnionFind connectivity gate 
    // This check is O(alpha(n)) — nearly O(1).
    // If the unit's location and the incident are in disconnected components,
    // we abort immediately without running the expensive Dijkstra.
    if (!connectivity.connected(unit.locationVertex, inc.locationVertex)) {
        cout << "[BLOCKED]  Unit " << unitId << " cannot reach Incident #"
             << incidentId << " — road network is disconnected.\n";
        return false;
    }

    // Step 2: Dijkstra — shortest path 
    DijkstraResult route = dijkstra(cityMap, unit.locationVertex);

    if (route.dist[inc.locationVertex] == INF) {
        cout << "[BLOCKED]  No path found from unit " << unitId
             << " (vertex " << unit.locationVertex << ")"
             << " to incident #" << incidentId
             << " (vertex " << inc.locationVertex << ").\n";
        return false;
    }

    // Step 3: Confirm dispatch 
    unit.available      = false;
    inc.assignedUnitId  = unitId;

    // Step 4: Record route in analytics 
    recordPathVisits(route, inc.locationVertex);

    // Step 5: Print the dispatch
    printDivider();
    cout << "[DISPATCH] " << unitTypeName(unit.type) << " '" << unit.name
         << "' (ID " << unitId << ")"
         << " -> Incident #" << incidentId
         << " (" << incidentTypeName(inc.type) << ")"
         << "  |  T+" << currentTime << " min\n";

    // Reconstruct and print the path
    int path[MAX_VERTICES];
    int len = 0;
    int curr = inc.locationVertex;
    while (curr != -1 && len < MAX_VERTICES) {
        path[len++] = curr;
        curr = route.parent[curr];
    }
    cout << "  Route (cost=" << route.dist[inc.locationVertex] << "): ";
    for (int i = len - 1; i >= 0; i--) {
        cout << path[i];
        if (i != 0) cout << " -> ";
    }
    cout << "\n";
    printDivider();

    return true;
}

// autoDispatch
// Finds the highest-priority open incident and dispatches the closest
// available unit to it automatically.
//
// Algorithm:
//   1. Extract from MinHeap until we find an open (unresolved) incident.
//      Resolved entries are discarded (lazy deletion — standard heap pattern).
//   2. Run Dijkstra ONCE from the incident location.
//      For undirected graphs this gives us the distance FROM every unit TO
//      the incident, because edge weights are symmetric.
//   3. Scan all available units — pick the one with the smallest distance.
//      UnionFind is checked first as a fast pre-filter.
//   4. Call dispatchUnit for the chosen unit.
bool Dispatcher::autoDispatch() {
    // Step 1: Find the highest-priority open incident 
    int targetIncidentId = -1;

    while (!incidentQueue.isEmpty()) {
        HeapNode top = incidentQueue.extractMin();
        int id = top.data;

        int* slotPtr = incidentTable.lookup(id);
        if (slotPtr == nullptr) continue;           // orphaned entry — skip
        if (incidents[*slotPtr].resolved)  continue; // already closed — skip
        if (incidents[*slotPtr].assignedUnitId != -1) continue; // already assigned

        targetIncidentId = id;
        // Put it back so the queue stays consistent for future autoDispatch calls
        incidentQueue.insert(heapPriority(incidents[*slotPtr].severity, id), id);
        break;
    }

    if (targetIncidentId == -1) {
        cout << "[AUTO]     No open, unassigned incidents in the queue.\n";
        return false;
    }

    int* iSlotPtr = incidentTable.lookup(targetIncidentId);
    Incident& inc = incidents[*iSlotPtr];

    cout << "[AUTO]     Triaging: Incident #" << targetIncidentId
         << " (" << incidentTypeName(inc.type)
         << ", severity " << inc.severity << "/10"
         << ", vertex " << inc.locationVertex << ")\n";

    // Step 2: Dijkstra from the incident location 
    DijkstraResult fromIncident = dijkstra(cityMap, inc.locationVertex);

    // Step 3: Find the best available unit.
    // Pass 1 — prefer the correct unit type for this incident:
    //   FIRE     -> FIRE_TRUCK
    //   MEDICAL  -> AMBULANCE
    //   CRIME    -> POLICE
    //   ACCIDENT -> AMBULANCE (first responder)
    // Pass 2 — if no matching type is reachable, fall back to any available unit.
    UnitType preferred;
    switch (inc.type) {
        case FIRE:     preferred = FIRE_TRUCK; break;
        case MEDICAL:  preferred = AMBULANCE;  break;
        case CRIME:    preferred = POLICE;     break;
        case ACCIDENT: preferred = AMBULANCE;  break;
        default:       preferred = AMBULANCE;  break;
    }

    int bestUnitId   = -1;
    int bestDist     = INF;
    int availableCount = 0;

    // Count all available units once (used in blocked-dispatch message)
    for (int i = 0; i < MAX_UNITS; i++) {
        if (units[i].active && units[i].available) availableCount++;
    }

    // Pass 1: matching type only
    for (int i = 0; i < MAX_UNITS; i++) {
        if (!units[i].active || !units[i].available) continue;
        if (units[i].type != preferred) continue;

        int uVertex = units[i].locationVertex;
        if (!connectivity.connected(uVertex, inc.locationVertex)) continue;

        int d = fromIncident.dist[uVertex];
        if (d < bestDist) {
            bestDist   = d;
            bestUnitId = units[i].id;
        }
    }

    // Pass 2: fallback — any available reachable unit
    if (bestUnitId == -1) {
        for (int i = 0; i < MAX_UNITS; i++) {
            if (!units[i].active || !units[i].available) continue;

            int uVertex = units[i].locationVertex;
            if (!connectivity.connected(uVertex, inc.locationVertex)) continue;

            int d = fromIncident.dist[uVertex];
            if (d < bestDist) {
                bestDist   = d;
                bestUnitId = units[i].id;
            }
        }
        if (bestUnitId != -1)
            cout << "[AUTO]     No " << unitTypeName(preferred)
                 << " available — falling back to nearest unit.\n";
    }

    if (bestUnitId == -1) {
        if (availableCount == 0) {
            cout << "[AUTO]     All units are busy — Incident #"
                 << targetIncidentId << " must wait.\n";
        } else {
            cout << "[AUTO]     " << availableCount << " unit(s) available but none can reach"
                 << " Incident #" << targetIncidentId << " — roads are blocked.\n";
        }
        return false;
    }

    int* bestSlot = unitTable.lookup(bestUnitId);
    cout << "[AUTO]     Closest " << unitTypeName(units[*bestSlot].type)
         << ": ID " << bestUnitId << " (distance " << bestDist << ")\n";

    return dispatchUnit(bestUnitId, targetIncidentId);
}

// resolveIncident
// Closes an incident, frees its assigned unit, and records response time.
// The unit is teleported to the incident vertex (it is now on scene).
bool Dispatcher::resolveIncident(int incidentId) {
    int* iSlotPtr = incidentTable.lookup(incidentId);
    if (iSlotPtr == nullptr) {
        cout << "Error: incident " << incidentId << " not found.\n";
        return false;
    }
    Incident& inc = incidents[*iSlotPtr];
    if (inc.resolved) {
        cout << "Error: incident " << incidentId << " is already resolved.\n";
        return false;
    }

    inc.resolved    = true;
    inc.resolveTime = currentTime;

    // Record response time 
    if (inc.resolveTime >= inc.reportTime) {
        totalResponseTime += (inc.resolveTime - inc.reportTime);
        resolvedCount++;
    }

    // Free the assigned unit 
    if (inc.assignedUnitId != -1) {
        int* uSlotPtr = unitTable.lookup(inc.assignedUnitId);
        if (uSlotPtr != nullptr) {
            units[*uSlotPtr].available      = true;
            // The unit's position is set to the incident vertex after resolution.
            // This models the unit having physically arrived and completed the job.
            // The next autoDispatch will route from this location, not the original
            // station. Use relocateUnit() afterwards to move the unit back to base.
            units[*uSlotPtr].locationVertex = inc.locationVertex;
        }
    }

    printDivider();
    cout << "[RESOLVED] Incident #" << incidentId
         << " (" << incidentTypeName(inc.type) << ")"
         << "  |  response time: "
         << (inc.resolveTime - inc.reportTime) << " min"
         << "  |  T+" << currentTime << " min\n";
    printDivider();
    return true;
}

void Dispatcher::printIncidents() const {
    cout << "\n--- Incidents (" << incidentCount << ") ---\n";
    for (int i = 0; i < MAX_INCIDENTS; i++) {
        if (!incidents[i].active) continue;
        cout << "  [#" << incidents[i].id << "] "
             << incidentTypeName(incidents[i].type)
             << "  sev=" << incidents[i].severity
             << "  vertex=" << incidents[i].locationVertex
             << "  reported=T+" << incidents[i].reportTime
             << (incidents[i].resolved ? "  RESOLVED" : "  OPEN");
        if (incidents[i].assignedUnitId != -1)
            cout << "  unit=" << incidents[i].assignedUnitId;
        cout << "\n";
    }
}

// -----------------------------------------------------------------------------
//  ROAD UPDATES
// -----------------------------------------------------------------------------

// closeRoad
// Removes an edge from the city map and rebuilds the connectivity index.
// After rebuilding, checks whether any in-progress dispatches are now blocked.
bool Dispatcher::closeRoad(int srcId, int destId) {
    printDivider();
    cout << "[ROAD]     Closing road " << srcId << " <-> " << destId << "\n";

    bool removed = removeEdge(cityMap, srcId, destId, false);
    if (!removed) {
        cout << "[ROAD]     Edge not found — nothing changed.\n";
        return false;
    }

    // Rebuild the UnionFind from scratch — O(V + E)
    connectivity.rebuildFromGraph(cityMap);
    cout << "[ROAD]     Connectivity map rebuilt.\n";

    // Warn about any active dispatches that are now cut off
    for (int i = 0; i < MAX_INCIDENTS; i++) {
        if (!incidents[i].active)           continue;
        if (incidents[i].resolved)          continue;
        if (incidents[i].assignedUnitId == -1) continue;

        int* uSlotPtr = unitTable.lookup(incidents[i].assignedUnitId);
        if (uSlotPtr == nullptr) continue;

        int unitVertex     = units[*uSlotPtr].locationVertex;
        int incidentVertex = incidents[i].locationVertex;

        if (!connectivity.connected(unitVertex, incidentVertex)) {
            cout << "[WARNING]  Unit " << incidents[i].assignedUnitId
                 << " can no longer reach Incident #" << incidents[i].id
                 << " (road network disconnected).\n";
        }
    }
    printDivider();
    return true;
}

// reopenRoad
// Re-adds a previously closed road and rebuilds connectivity.
bool Dispatcher::reopenRoad(int srcId, int destId, int weight) {
    printDivider();
    cout << "[ROAD]     Reopening road " << srcId << " <-> " << destId
         << " (weight " << weight << ")\n";

    bool added = addEdge(cityMap, srcId, destId, weight, false);
    if (!added) return false;

    connectivity.rebuildFromGraph(cityMap);
    cout << "[ROAD]     Connectivity map rebuilt.\n";
    printDivider();
    return true;
}

// -----------------------------------------------------------------------------
//  SEARCH
// -----------------------------------------------------------------------------

// searchUnitsByPrefix
// Uses the Trie to find all unit names that start with the given prefix.
// Demonstrates O(prefix_length) prefix lookup.
void Dispatcher::searchUnitsByPrefix(const char* prefix) const {
    char lower[32];
    toLowerCopy(prefix, lower, 32);
    cout << "\n[SEARCH]   Units matching prefix \"" << prefix << "\":\n";
    unitNameTrie.printWordsWithPrefix(lower);
}

// -----------------------------------------------------------------------------
//  ANALYTICS
// -----------------------------------------------------------------------------

// countIncidentsInWindow
// Uses the SegmentTree to count ALL incidents reported in [startMinute, endMinute].
// O(log BUCKETS) — returns a number only, no individual records.
// Use listIncidentsInWindow when you need the actual incident details.
int Dispatcher::countIncidentsInWindow(int startMinute, int endMinute) const {
    int left  = startMinute % BUCKETS;
    int right = endMinute   % BUCKETS;
    if (left > right) {
        // Wraps around the bucket array — count both halves separately
        return timeWindow.query(left, BUCKETS - 1) + timeWindow.query(0, right);
    }
    return timeWindow.query(left, right);
}

// listIncidentsInWindow
// Uses AVLTree::collectRange to get all incident IDs in [startMinute, endMinute],
// then looks each one up in the incident table to print full details.
// This is the "logbook" query — use it when you need the actual records,
// not just a count.
void Dispatcher::listIncidentsInWindow(int startMinute, int endMinute) const {
    cout << "\n  Incidents in window [T+" << startMinute
         << ", T+" << endMinute << "]:\n";

    int ids[MAX_INCIDENTS];
    int found = incidentLog.collectRange(startMinute, endMinute, ids, MAX_INCIDENTS);

    if (found == 0) {
        cout << "    (none)\n";
        return;
    }

    for (int i = 0; i < found; i++) {
        int* slotPtr = incidentTable.lookup(ids[i]);
        if (!slotPtr) continue;
        const Incident& inc = incidents[*slotPtr];
        cout << "    #" << inc.id
             << "  " << incidentTypeName(inc.type)
             << "  sev=" << inc.severity << "/10"
             << "  vertex=" << inc.locationVertex
             << "  T+" << inc.reportTime;
        if (inc.assignedUnitId != -1)
            cout << "  unit=" << inc.assignedUnitId;
        cout << (inc.resolved ? "  [resolved]" : "  [open]") << "\n";
    }
}

// getBusiestIntersection
// Linear scan over visitCount[] — O(V).
int Dispatcher::getBusiestIntersection() const {
    int busiestId = -1;
    int maxVisits = 0;
    for (int i = 0; i < MAX_VERTICES; i++) {
        if (visitCount[i] > maxVisits) {
            maxVisits = visitCount[i];
            busiestId = i;
        }
    }
    return busiestId;
}

// getAvgResponseTime
double Dispatcher::getAvgResponseTime() const {
    if (resolvedCount == 0) return 0.0;
    return (double)totalResponseTime / (double)resolvedCount;
}

// printTopIntersections
// Prints the N vertices with the highest dispatch-route traffic.
// Uses repeated linear scans: O(N * V), acceptable for small N.
void Dispatcher::printTopIntersections(int n) const {
    // Work on a local copy so we can mark off already-printed vertices
    int temp[MAX_VERTICES];
    for (int i = 0; i < MAX_VERTICES; i++) {
        temp[i] = cityMap.active[i] ? visitCount[i] : -1;
    }

    cout << "\n  Top " << n << " busiest intersections:\n";
    for (int rank = 1; rank <= n; rank++) {
        int maxVal = 0;
        int maxIdx = -1;
        for (int i = 0; i < MAX_VERTICES; i++) {
            if (temp[i] > maxVal) { maxVal = temp[i]; maxIdx = i; }
        }
        if (maxIdx == -1) { cout << "  (no more data)\n"; break; }
        cout << "    #" << rank << "  Vertex " << maxIdx
             << "  —  " << maxVal
             << (maxVal == 1 ? " route" : " routes") << "\n";
        temp[maxIdx] = -1;
    }
}

// -----------------------------------------------------------------------------
//  DISPLAY
// -----------------------------------------------------------------------------

void Dispatcher::printStatus() const {
    int available = 0;
    for (int i = 0; i < MAX_UNITS; i++) {
        if (units[i].active && units[i].available) available++;
    }

    int openIncidents = 0;
    int dispatched = 0;
    for (int i = 0; i < MAX_INCIDENTS; i++) {
        if (incidents[i].active && !incidents[i].resolved) openIncidents++;
        if (incidents[i].active && incidents[i].assignedUnitId != -1) dispatched++;
    }

    cout << "\n--------------------------------------------------\n";
    cout << "  DISPATCHER STATUS  |  T+" << currentTime << " min\n";
    cout << "----------------------------------------------------\n";
    cout << "  Units total    : " << unitCount        << "\n";
    cout << "  Units available: " << available        << "\n";
    cout << "  Open incidents : " << openIncidents    << "\n";
    cout << "  Dispatched     : " << dispatched       << "\n";
    cout << "--------------------------------------------------\n\n";
}

void Dispatcher::printAnalytics() const {
    int busiest = getBusiestIntersection();

    cout << "\n------------------------------------------------\n";
    cout << "  ANALYTICS  |  T+" << currentTime << " min\n";
    cout << "--------------------------------------------------\n";

    // Busiest intersection
    if (busiest == -1) {
        cout << "  Busiest intersection : none recorded yet\n";
    } else {
        cout << "  Busiest intersection : vertex " << busiest
             << "  (" << visitCount[busiest] << " dispatch routes)\n";
    }
    printTopIntersections(3);

    // Average response time
    if (resolvedCount == 0) {
        cout << "  Avg response time    : no resolved incidents yet\n";
    } else {
        cout << "  Avg response time    : " << getAvgResponseTime()
             << " min  (over " << resolvedCount << " incidents)\n";
    }

    // Time-window queries: last 60 minutes
    int windowStart = (currentTime > 60) ? currentTime - 60 : 0;

    // Segment tree: how many? — O(log BUCKETS), count only
    int stCount = countIncidentsInWindow(windowStart, currentTime);
    cout << "  Incidents reported (last 60 min) [SegmentTree]: " << stCount << "\n";

    // AVL tree: which ones? — O(log n + k), returns actual records
    listIncidentsInWindow(windowStart, currentTime);

    cout << "-----------------------------------------------------------\n\n";
}
