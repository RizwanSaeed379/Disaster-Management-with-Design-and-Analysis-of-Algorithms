#include <iostream>
#include <vector>
#include "Graph.h"
#include "node.h"
#include "edge.h"
#include "vehicle.h"
#include "DisasterManager.h"

using namespace std;

int main() {
    // Create a graph with 5 nodes: 0 = depot, 1-4 = demand nodes
    Graph g(5);
    g.nodes = {
        {0, 0, 0},   // depot
        {1, 3, 5},
        {2, 2, 3},
        {3, 4, 4},
        {4, 1, 2}
    };

    // Add edges: u, v, cost, reliability
    g.addEdge(0, 1, 4, 0.9);
    g.addEdge(0, 2, 6, 0.8);
    g.addEdge(1, 2, 2, 0.7);
    g.addEdge(1, 3, 5, 0.95);
    g.addEdge(2, 3, 3, 0.85);
    g.addEdge(3, 4, 4, 0.9);

    // Vehicles: id, capacity
    vector<Vehicle> vehicles = {
        {1, 5, {}}, // capacity 5
        {2, 6, {}}, // capacity 6
    };

    // Create DisasterManager
    DisasterManager dm(g, vehicles);

    // Allocate nodes to vehicles and compute routes
    dm.allocateAndRoute();

    // Compute metrics and print routes
    dm.computeMetrics();

    return 0;
}
