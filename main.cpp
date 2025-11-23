#include <iostream>
#include <algorithm>
#include "Graph.h"
#include "vehicle.h"
#include "DisasterManager.h"
using namespace std;

int main() 
{
    // // Nodes
    // Graph g(5);
    // g.nodes = {{0,0,0},{1,3,5},{2,2,3},{3,4,4},{4,1,2}};

    // // Edges (cost, reliability)
    // g.addEdge(0,1,4,0.9);
    // g.addEdge(0,2,6,0.8);
    // g.addEdge(1,2,2,0.7);
    // g.addEdge(1,3,5,0.95);
    // g.addEdge(2,3,3,0.85);
    // g.addEdge(3,4,4,0.9);

    // // Vehicles
    // vector<Vehicle> vehicles = {{1,5,{}},{2,6,{}}};


//     // Nodes
// Graph g(6);
// g.nodes = {
//     {0, 0, 0},   // Depot
//     {1, 4, 5},   // demand, priority
//     {2, 2, 4},
//     {3, 3, 3},
//     {4, 5, 2},
//     {5, 1, 1}
// };

// // Edges (cost, reliability)
// g.addEdge(0,1,5,0.9);
// g.addEdge(0,2,6,0.85);
// g.addEdge(0,3,7,0.8);
// g.addEdge(1,2,3,0.95);
// g.addEdge(1,4,6,0.9);
// g.addEdge(2,3,2,0.9);
// g.addEdge(3,5,4,0.85);

// // Vehicles
// vector<Vehicle> vehicles = {
//     {1, 6, {}},  // vehicle id, capacity
//     {2, 7, {}},
//     {3, 5, {}}
// };


//  // Create graph with 5 nodes
//     Graph g(5);

//     // Nodes: id, demand, priority
//     g.nodes = {
//         {0, 0, 0},  // Depot
//         {1, 3, 5},
//         {2, 2, 3},
//         {3, 4, 4},
//         {4, 1, 2}
//     };

//     // Edges: u, v, cost, reliability
//     g.addEdge(0, 1, 4, 0.9);
//     g.addEdge(0, 2, 6, 0.8);
//     g.addEdge(1, 2, 2, 0.7);
//     g.addEdge(1, 3, 5, 0.95);
//     g.addEdge(2, 3, 3, 0.85);
//     g.addEdge(3, 4, 4, 0.9);

//     // Vehicles: id, capacity
//     vector<Vehicle> vehicles = {
//         {1, 5, {}},
//         {2, 6, {}}
//     };


Graph g(5); // 0 = depot, 1-4 = demand nodes
g.nodes = {
    // Nodes: id, demand, priority
    {0, 0, 0},  
    {1, 3, 5},  
    {2, 2, 3},  
    {3, 4, 4},  
    {4, 1, 2}   
};

// Edges: u, v, cost, reliability
g.addEdge(0,1,4,0.9);
g.addEdge(0,2,6,0.8);
g.addEdge(1,2,2,0.7);
g.addEdge(1,3,5,0.95);
g.addEdge(2,3,3,0.85);
g.addEdge(3,4,4,0.9);

// Vehicles : id, capacity
vector<Vehicle> vehicles = {
    {1, 5, {}}, // capacity 5
    {2, 6, {}}, // capacity 6
};

// Simulate edge failures
g.setEdgeAvailability(0,4,false); // prevent vehicle 3 from reaching node 4 directly
g.setEdgeAvailability(3,4,false); // optional: prevent routing via other nodes if needed


    DisasterManager dm(g,vehicles);
    dm.allocateAndRoute();
    dm.computeMetrics();

    return 0;
}
