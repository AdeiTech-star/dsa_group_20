//graph.cpp

#include "../include/graph.h"
#include <iostream>
using namespace std;

void initGraph(Graph& g){
    g.vertexCount = 0; // initialise vertexcount
    for(int i=0; i<MAX_VERTICES; i++){
        g.active[i]              = false; // slot is empty
        g.vertices[i].id         = -1;    // -1 = "not a real vertex"
        g.vertices[i].neighbours = nullptr;
    }
}

bool addVertex(Graph& g, int id){
    //check if id is valid
    if(id < 0 || id >= MAX_VERTICES){
        cout << "Error: vertex ID: " << id << " is invalid(out of range). \n";
        return false;
    }

    //check if is available or free
    if (g.active[id]){
        cout << "Error: vertex: "<< id << " already exists. \n";
        return false;
    
    }

    //now create the slot
    g.vertices[id].id = id; //store it's unique id
    g.vertices[id].neighbours = nullptr; //no edges as yet
    g.active[id] = true; //mark the slot as live
    g.vertexCount++; //increment the number of vertices

    cout <<"Vertex: " << id << " created. \n";
    return true;
}


// ============================ADDING EDGES ======================

//we'll start with directed edges

static void addDirectedEdge(Graph& g, int srcId, int destId, int weight){
    neighbourNode* newNode = new neighbourNode();
    newNode -> destId = destId;
    newNode -> weight = weight;

    //new node now also points to what was the old head
    newNode -> next = g.vertices[srcId].neighbours;

    g.vertices[srcId].neighbours = newNode; //our newnode is now the head

}

bool addEdge(Graph& g, int srcId, int destId, int weight, bool directed){
    if(!g.active[srcId]){
        cout << "Error: source vertex: "<<srcId<<" does not exist. \n";
        return false;
    }

    if(!g.active[destId]){
      cout << "Error: destination vertex: "<<destId<<" does not exist. \n";
        return false;
    }

    //to avoid self loops
    if(srcId == destId){
        cout << "Error: self-loops are not allowed. \n";
        return false;
    }

    //avoid duplicate edges
    neighbourNode* curr = g.vertices[srcId].neighbours;
    while(curr != nullptr){
        if(curr -> destId == destId){
            cout << "Error: edge " << srcId << " → " << destId << " already exists.\n";
            return false;
        }

        curr = curr -> next;
    }

    //now we can add the valid edges
    addDirectedEdge(g, srcId, destId, weight);

    if(!directed){
        addDirectedEdge(g, destId,srcId,weight); //unidirected, so we reverse the direction

    }

    cout << "Edge " << srcId << (directed ? " → " : " ↔ ") << destId
         << " (weight " << weight << ") added.\n";
    return true;
}

