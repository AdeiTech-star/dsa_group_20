// main.cpp
#include "../include/graph.h"
#include <iostream>
using namespace std;

void printMenu() {
    cout << "\n========= Urban Incident Response Graph =========\n";
    cout << "  Vertex Operations\n";
    cout << "    1. Add intersection\n";
    cout << "    2. Remove intersection\n";
    cout << "\n  Edge Operations\n";
    cout << "    3. Add road (edge)\n";
    cout << "    4. Remove road\n";
    cout << "    5. Update road weight\n";
    cout << "\n  Query Operations\n";
    cout << "    6. Get neighbours of intersection\n";
    cout << "    7. Get road weight between two intersections\n";
    cout << "    8. Check if two intersections are directly connected\n";
    cout << "    9. Get vertex count\n";
    cout << "   10. Get edge count\n";
    cout << "\n  Display\n";
    cout << "   11. Print entire graph\n";
    cout << "\n   0. Exit\n";
    cout << "=================================================\n";
    cout << "Choice: ";
}

int main() {
    Graph g;
    initGraph(g);

    cout << "Graph initialised. MAX_VERTICES = " << MAX_VERTICES << "\n";

    int choice;

    do {
        printMenu();
        cin >> choice;

        // guard against bad input (letters, symbols etc.)
        if (cin.fail()) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {

            // ── Vertex Operations  ─────────────────
            case 1: {
                int id;
                cout << "Enter intersection ID to add: ";
                cin >> id;
                addVertex(g, id);
                break;
            }

            case 2: {
                int id;
                cout << "Enter intersection ID to remove: ";
                cin >> id;
                removeVertex(g, id);
                break;
            }

            // ── Edge Operations  ───────────────────
            case 3: {
                int src, dest, weight, dir;
                cout << "Source intersection ID      : ";  cin >> src;
                cout << "Destination intersection ID : ";  cin >> dest;
                cout << "Road weight                 : ";  cin >> weight;
                cout << "Directed? (1 = yes, 0 = no) : ";  cin >> dir;
                addEdge(g, src, dest, weight, dir == 1);
                break;
            }

            case 4: {
                int src, dest, dir;
                cout << "Source intersection ID      : ";  cin >> src;
                cout << "Destination intersection ID : ";  cin >> dest;
                cout << "Directed? (1 = yes, 0 = no) : ";  cin >> dir;
                removeEdge(g, src, dest, dir == 1);
                break;
            }

            case 5: {
                int src, dest, newWeight, dir;
                cout << "Source intersection ID      : ";  cin >> src;
                cout << "Destination intersection ID : ";  cin >> dest;
                cout << "New weight                  : ";  cin >> newWeight;
                cout << "Directed? (1 = yes, 0 = no) : ";  cin >> dir;
                updateEdgeWeight(g, src, dest, newWeight, dir == 1);
                break;
            }

            // ── Query Operations  ──────────────────
            case 6: {
                int id;
                cout << "Enter intersection ID: ";
                cin >> id;
                getNeighbours(g, id);
                break;
            }

            case 7: {
                int src, dest;
                cout << "Source intersection ID      : ";  cin >> src;
                cout << "Destination intersection ID : ";  cin >> dest;
                int w = getEdgeWeight(g, src, dest);
                if (w != -1)
                    cout << "Weight of road " << src << " → " << dest << " : " << w << "\n";
                break;
            }

            case 8: {
                int src, dest;
                cout << "Source intersection ID      : ";  cin >> src;
                cout << "Destination intersection ID : ";  cin >> dest;
                bool connected = isConnected(g, src, dest);
                cout << "Intersections " << src << " and " << dest
                     << (connected ? " ARE" : " are NOT")
                     << " directly connected.\n";
                break;
            }

            case 9: {
                cout << "Active vertices: " << getVertexCount(g) << "\n";
                break;
            }

            case 10: {
                int dir;
                cout << "Directed graph? (1 = yes, 0 = no): ";  cin >> dir;
                cout << "Edge count: " << getEdgeCount(g, dir == 1) << "\n";
                break;
            }

            // ── Display   ──────
            case 11: {
                printGraph(g);
                break;
            }

            case 0: {
                cout << "Cleaning up memory...\n";
                destroyGraph(g);
                cout << "Goodbye.\n";
                break;
            }

            default: {
                cout << "Unknown option. Try again.\n";
                break;
            }
        }

    } while (choice != 0);

    return 0;
}