#include <iostream>
#include <fstream>
#include <vector>
#include "Graph.h"
#include "node.h"
#include "edge.h"
#include "vehicle.h"
#include "DisasterManager.h"
#include "json.hpp"

using json = nlohmann::json;


using namespace std;

int main() {

    ifstream file("input.json");
    if (!file) {
        cerr << "Failed to open input.json\n";
        return 1;
    }

    json cfg;
    file >> cfg;

   
    int numNodes = cfg["graph"]["num_nodes"];
    Graph g(numNodes);

    // Load nodes
    for (auto &jn : cfg["graph"]["nodes"]) {
        Node n;
        n.id = jn["id"];
        n.demand  = jn["x"];
        n.priority  = jn["y"];
        g.nodes.push_back(n);
    }

    // Load edges
    for (auto &je : cfg["graph"]["edges"]) {
        int u = je["u"];
        int v = je["v"];
        int cost = je["cost"];
        double rel = je["reliability"];
        g.addEdge(u, v, cost, rel);
    }

   
    vector<Vehicle> vehicles;

    for (auto &jv : cfg["vehicles"]) {
        Vehicle v;
        v.id = jv["id"];
        v.capacity = jv["capacity"];
        vehicles.push_back(v);
    }


    
    

    // Create DisasterManager
    DisasterManager dm(g, vehicles);

    // Allocate nodes to vehicles and compute routes
    dm.allocateAndRoute();

    // Compute metrics and print routes
    dm.computeMetrics();

    return 0;
}
