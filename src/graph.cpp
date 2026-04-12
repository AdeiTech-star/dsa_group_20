//graph.cpp

#include "../include/graph.h"
#include <iostream>
using namespace std;

void initGraph(Graph& g){
    g.vertexCount = 0;
    for(int i=0; i<MAX_VERTICES; i++){
        g.active[i]              = false;
        g.vertices[i].id         = -1;
        g.vertices[i].neighbours = nullptr;
    }
}

bool addVertex(Graph& g, int id){
    if(id < 0 || id >= MAX_VERTICES){
        cout << "Error: vertex ID: " << id << " is invalid(out of range). \n";
        return false;
    }

    if (g.active[id]){
        cout << "Error: vertex: "<< id << " already exists. \n";
        return false;
    }

    g.vertices[id].id         = id;
    g.vertices[id].neighbours = nullptr;
    g.active[id]              = true;
    g.vertexCount++;

    cout << "Vertex: " << id << " created. \n";
    return true;
}

// ========================== EDGES ==========================

static void addDirectedEdge(Graph& g, int srcId, int destId, int weight){
    neighbourNode* newNode  = new neighbourNode();
    newNode->destId         = destId;
    newNode->weight         = weight;
    newNode->next           = g.vertices[srcId].neighbours;
    g.vertices[srcId].neighbours = newNode;
}

bool addEdge(Graph& g, int srcId, int destId, int weight, bool directed){
    if(!g.active[srcId]){
        cout << "Error: source vertex: " << srcId << " does not exist. \n";
        return false;
    }
    if(!g.active[destId]){
        cout << "Error: destination vertex: " << destId << " does not exist. \n";
        return false;
    }
    if(srcId == destId){
        cout << "Error: self-loops are not allowed. \n";
        return false;
    }

    neighbourNode* curr = g.vertices[srcId].neighbours;
    while(curr != nullptr){
        if(curr->destId == destId){
            cout << "Error: edge " << srcId << " → " << destId << " already exists.\n";
            return false;
        }
        curr = curr->next;
    }

    addDirectedEdge(g, srcId, destId, weight);
    if(!directed){
        addDirectedEdge(g, destId, srcId, weight);
    }

    cout << "Edge " << srcId << (directed ? " → " : " ↔ ") << destId
         << " (weight " << weight << ") added.\n";
    return true;
}

// ── NEW: updateEdgeWeight ──────────────────────────────────────────────────
// Roads change — construction, traffic, time of day.
// Walks the list and updates the weight in-place. No remove + re-add needed.
// For undirected edges, both directions are updated.
bool updateEdgeWeight(Graph& g, int srcID, int destID, int newWeight, bool directed){

    // ── Guard: both vertices must exist ───────────────────────────────
    if(!g.active[srcID] || !g.active[destID]){
        cout << "Error: one or both vertices do not exist.\n";
        return false;
    }

    // ── Guard: weight must be positive ────────────────────────────────
    if(newWeight <= 0){
        cout << "Error: weight must be a positive value.\n";
        return false;
    }

    // ── Walk srcID's list and find destID ─────────────────────────────
    bool updated = false;
    neighbourNode* curr = g.vertices[srcID].neighbours;

    while(curr != nullptr){
        if(curr->destId == destID){
            curr->weight = newWeight;   // update in-place
            updated = true;
            break;
        }
        curr = curr->next;
    }

    if(!updated){
        cout << "Error: edge " << srcID << " → " << destID << " does not exist.\n";
        return false;
    }

    // ── For undirected: mirror the update in the reverse direction ─────
    if(!directed){
        neighbourNode* rev = g.vertices[destID].neighbours;
        while(rev != nullptr){
            if(rev->destId == srcID){
                rev->weight = newWeight;
                break;
            }
            rev = rev->next;
        }
    }

    cout << "Edge " << srcID << (directed ? " → " : " ↔ ") << destID
         << " weight updated to " << newWeight << ".\n";
    return true;
}

// ========================== READ OPERATIONS ==========================

void getNeighbours(Graph& g, int id){
    if(!g.active[id]){
        cout << "Error: vertex " << id << " does not exist.\n";
        return;
    }

    cout << "Neighbours of vertex " << id << ": ";

    neighbourNode* curr = g.vertices[id].neighbours;

    if(curr == nullptr){
        cout << "none (isolated vertex)\n";
        return;
    }

    while(curr != nullptr){
        cout << "[dest=" << curr->destId << ", w=" << curr->weight << "]";
        if(curr->next != nullptr) cout << " → ";
        curr = curr->next;
    }
    cout << " → nullptr\n";
}

int getEdgeWeight(Graph& g, int srcID, int destID){
    if(!g.active[srcID] || !g.active[destID]){
        cout << "Error: one or both vertices do not exist.\n";
        return -1;
    }

    neighbourNode* curr = g.vertices[srcID].neighbours;
    while(curr != nullptr){
        if(curr->destId == destID) return curr->weight;
        curr = curr->next;
    }

    cout << "No edge between " << srcID << " and " << destID << ".\n";
    return -1;
}

bool isConnected(Graph& g, int srcID, int destID){
    if(!g.active[srcID] || !g.active[destID]){
        cout << "Error: one or both vertices do not exist.\n";
        return false;
    }

    neighbourNode* curr = g.vertices[srcID].neighbours;
    while(curr != nullptr){
        if(curr->destId == destID) return true;
        curr = curr->next;
    }

    return false;
}

// ── NEW: getVertexCount ───────────────────────────────────────────────────
// Returns how many vertices are currently active.
// The dispatcher uses this to size buffers and loop bounds.
int getVertexCount(Graph& g){
    return g.vertexCount;
}

// ── NEW: getEdgeCount  
// Counts edges by summing every active vertex's adjacency list length.
// For undirected graphs each physical edge appears in TWO lists
// (A→B and B→A), so we divide by 2 at the end.
// For directed graphs pass directed=true and you get the raw arc count.
int getEdgeCount(Graph& g, bool directed){
    int total = 0;

    for(int i = 0; i < MAX_VERTICES; i++){
        if(!g.active[i]) continue;

        neighbourNode* curr = g.vertices[i].neighbours;
        while(curr != nullptr){
            total++;
            curr = curr->next;
        }
    }

    return directed ? total : total / 2;
}

// ── NEW: printGraph  ──
// Prints the entire adjacency list — one row per active vertex.
// Much faster to read at a glance than calling getNeighbours in a loop.
// Example output:
//   [0] → [dest=1,w=4] → [dest=2,w=7] → nullptr
//   [1] → [dest=2,w=3] → [dest=0,w=4] → nullptr
//   [2] → [dest=1,w=3] → [dest=0,w=7] → nullptr
void printGraph(Graph& g){
    cout << "\n=== Graph (vertices: " << g.vertexCount
         << ", edges: "  << getEdgeCount(g, false) << ") ===\n";

    bool anyActive = false;

    for(int i = 0; i < MAX_VERTICES; i++){
        if(!g.active[i]) continue;
        anyActive = true;

        cout << "  [" << i << "] → ";

        neighbourNode* curr = g.vertices[i].neighbours;

        if(curr == nullptr){
            cout << "nullptr (isolated)\n";
            continue;
        }

        while(curr != nullptr){
            cout << "[dest=" << curr->destId << ", w=" << curr->weight << "]";
            if(curr->next != nullptr) cout << " → ";
            curr = curr->next;
        }
        cout << " → nullptr\n";
    }

    if(!anyActive) cout << "  (empty graph)\n";
    cout << "=========================\n\n";
}

// ========================== REMOVE OPERATIONS ==========================

// Extracted helper — replaces the lambda in removeEdge.
// Removes the node with destId == target from src's adjacency list.
// Returns true if found and deleted, false if not found.
// (Avoids the lambda / auto question under the no-STL rule.)
static bool removeFromList(intersection& src, int target){
    neighbourNode* curr = src.neighbours;
    neighbourNode* prev = nullptr;

    while(curr != nullptr){
        if(curr->destId == target){
            if(prev == nullptr){
                src.neighbours = curr->next;  // removing the head
            } else {
                prev->next = curr->next;       // removing mid / tail
            }
            delete curr;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }

    return false;
}

bool removeEdge(Graph& g, int srcID, int destID, bool directed){
    if(!g.active[srcID] || !g.active[destID]){
        cout << "Error: one or both vertices do not exist.\n";
        return false;
    }

    bool removed = removeFromList(g.vertices[srcID], destID);

    if(!removed){
        cout << "Error: edge " << srcID << " → " << destID << " does not exist.\n";
        return false;
    }

    if(!directed){
        removeFromList(g.vertices[destID], srcID);
    }

    cout << "Edge " << srcID << (directed ? " → " : " ↔ ")
         << destID << " removed.\n";
    return true;
}

bool removeVertex(Graph& g, int id){
    if(!g.active[id]){
        cout << "Error: vertex " << id << " does not exist.\n";
        return false;
    }

    // Stage 1: free this vertex's own adjacency list
    neighbourNode* curr = g.vertices[id].neighbours;
    while(curr != nullptr){
        neighbourNode* toDelete = curr;
        curr = curr->next;
        delete toDelete;
    }
    g.vertices[id].neighbours = nullptr;

    // Stage 2: remove back-references from all other active vertices
    for(int i = 0; i < MAX_VERTICES; i++){
        if(g.active[i] && i != id){
            removeFromList(g.vertices[i], id);
        }
    }

    // Stage 3: deactivate the slot
    g.active[id] = false;
    g.vertices[id].id = -1;
    g.vertexCount--;

    cout << "Vertex " << id << " removed.\n";
    return true;
}

// ── NEW: destroyGraph  
// Frees every dynamically allocated neighbourNode in the graph,
// then resets the graph back to its post-initGraph state.
// Call this at the end of main (or whenever you're done with the graph)
// to satisfy Valgrind's zero-leak requirement.
void destroyGraph(Graph& g){
    for(int i = 0; i < MAX_VERTICES; i++){
        if(!g.active[i]) continue;

        // Walk and delete every node in this vertex's list
        neighbourNode* curr = g.vertices[i].neighbours;
        while(curr != nullptr){
            neighbourNode* toDelete = curr;
            curr = curr->next;
            delete toDelete;
        }

        // Reset the slot to a clean state
        g.vertices[i].neighbours = nullptr;
        g.vertices[i].id         = -1;
        g.active[i]              = false;
    }

    g.vertexCount = 0;
    cout << "Graph destroyed. All memory freed.\n";
}