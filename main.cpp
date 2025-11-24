#include <iostream>
#include <fstream>
#include <vector>
#include "Graph.h"
#include "node.h"
#include "edge.h"
#include "vehicle.h"
#include "DisasterManager.h"
#include "json.hpp"
#include <filesystem>

using json = nlohmann::json;
using namespace std;

int main(int argc, char* argv[]) {

    // Determine file path
    string filepath;
    if (argc > 1) {
        filepath = argv[1]; // Path from command line
    } else {
        filepath = "input.json"; // Default file in same folder as exe
    }

    // Print current working directory
    cout << "Current working directory: " << std::filesystem::current_path() << endl;
    cout << "Attempting to open file: " << filepath << endl;

    // Open file
    ifstream file(filepath);
    if (!file) {
        cerr << "Failed to open file: " << filepath << endl;
        return 1;
    }

    // Parse JSON
    json cfg;
    file >> cfg;

    int numNodes = cfg["graph"]["num_nodes"];
    Graph g(numNodes);

    // Load nodes
    for (auto &jn : cfg["graph"]["nodes"]) {
        Node n;
        n.id = jn["id"];
        n.demand  = jn["demand"];
        n.priority  = jn["priority"];
        g.nodes.push_back(n);
    }



   for (auto &je : cfg["graph"]["edges"]) {
    int u = je.value("u", -1);
    int v = je.value("v", -1);
    int cost = je.value("cost", 0);
    double rel = je.value("reliability", 1.0);
    if(u >= 0 && v >= 0) g.addEdge(u, v, cost, rel);
}


    // Load vehicles
    vector<Vehicle> vehicles;
    for (auto &jv : cfg["vehicles"]) {
        Vehicle v;
        v.id = jv["id"];
        v.capacity = jv["capacity"];
        vehicles.push_back(v);
    }

    g.setEdgeAvailability(3, 4, false);
    g.setEdgeAvailability(4, 3, false);

    // Create DisasterManager
    DisasterManager dm(g, vehicles);

    // Allocate nodes to vehicles and compute routes
    dm.allocateAndRoute();

    // Compute metrics and print routes
    dm.computeMetrics();

    return 0;
}
