#include <iostream>
#include "Graph.h"
#include "Vehicle.h"
#include "DisasterManager.h"
using namespace std;

int main() {
    // Nodes
    Graph g(5);
    g.nodes = {{0,0,0},{1,3,5},{2,2,3},{3,4,4},{4,1,2}};

    // Edges (cost, reliability)
    g.addEdge(0,1,4,0.9);
    g.addEdge(0,2,6,0.8);
    g.addEdge(1,2,2,0.7);
    g.addEdge(1,3,5,0.95);
    g.addEdge(2,3,3,0.85);
    g.addEdge(3,4,4,0.9);

    // Vehicles
    vector<Vehicle> vehicles = {{1,5,{}},{2,6,{}}};

    DisasterManager dm(g,vehicles);
    dm.allocateAndRoute();
    dm.computeMetrics();

    return 0;
}
