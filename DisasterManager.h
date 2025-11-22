#ifndef DISASTERMANAGER_H
#define DISASTERMANAGER_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits>
#include "Graph.h"
#include "vehicle.h"

using namespace std;

struct DisasterManager {
    Graph &graph;
    vector<Vehicle> &vehicles;

    DisasterManager(Graph &g, vector<Vehicle> &v) : graph(g), vehicles(v) {}

    void allocateAndRoute() {
        vector<Node> nodes = graph.nodes;
        nodes.erase(nodes.begin()); // remove depot

        // Compute efficiency score = priority / demand
        std::sort(nodes.begin(), nodes.end(), [](Node a, Node b) {
            double scoreA = (a.demand > 0) ? (double)a.priority / a.demand : a.priority;
            double scoreB = (b.demand > 0) ? (double)b.priority / b.demand : b.priority;
            return scoreA > scoreB;
        });

        vector<bool> nodeAssigned(graph.nodes.size(), false); // track assigned nodes across all vehicles

        // First pass: Assign nodes using best-fit heuristic
        for (auto &veh : vehicles) {
            int remainingCapacity = veh.capacity;

            for (auto &node : nodes) {
                if (nodeAssigned[node.id]) continue;       // skip if already assigned
                if (node.demand <= remainingCapacity) {   // fits in remaining capacity
                    veh.route.push_back(node.id);
                    nodeAssigned[node.id] = true;
                    remainingCapacity -= node.demand;
                }
            }
        }

        // Optional second pass: fill remaining capacities with smaller unassigned nodes
        for (auto &veh : vehicles) {
            int remainingCapacity = veh.capacity;
            for (int n : veh.route) remainingCapacity -= graph.nodes[n].demand;

            for (auto &node : nodes) {
                if (nodeAssigned[node.id]) continue;
                if (node.demand <= remainingCapacity) {
                    veh.route.push_back(node.id);
                    nodeAssigned[node.id] = true;
                    remainingCapacity -= node.demand;
                }
            }
        }

        // Build full routes using multi-objective Dijkstra with reliability preference
        for (auto &veh : vehicles) {
            if (veh.route.empty()) continue;

            vector<int> fullRoute;
            fullRoute.push_back(0); // start at depot

            for (int nid : veh.route) {
                if (nid != 0 && std::find(fullRoute.begin(), fullRoute.end(), nid) != fullRoute.end())
                    continue; // skip duplicate nodes in the same route

                vector<int> path = graph.dijkstraMultiObjective(fullRoute.back(), nid);
                if(path.empty()) continue; // skip if no path available
                path.erase(path.begin());  // avoid duplicating current node
                fullRoute.insert(fullRoute.end(), path.begin(), path.end());
            }

            // Return to depot
            vector<int> returnPath = graph.dijkstraMultiObjective(fullRoute.back(), 0);
            if(!returnPath.empty()) {
                returnPath.erase(returnPath.begin());
                fullRoute.insert(fullRoute.end(), returnPath.begin(), returnPath.end());
            }

            veh.route = fullRoute;
        }
    }

void computeMetrics() {
    double totalReliability = 0;
    int totalEdges = 0;
    int totalCombinedCost = 0;
    int totalDelivered = 0;
    double priorityScore = 0.0;
    double maxPriority = 0.0;

    for (auto &veh : vehicles) {
        if (veh.route.size() <= 1) continue; // depot-only, skip

        int delivered = 0;
        int cost = 0;
        double vehReliabilitySum = 0;
        int vehEdges = 0;

        for (size_t i = 0; i < veh.route.size() - 1; i++) {
            int u = veh.route[i];
            int v = veh.route[i + 1];

            // cost
            for (auto edge : graph.adj[u]) {
                int nbr = edge.first;      // neighbor node
                int c   = edge.second;     // cost
            
                if (nbr == v && graph.isEdgeAvailable(u, v)) {
                    cost += c;
                    break;
                }
            }
            

            // reliability
            if (graph.isEdgeAvailable(u,v)) {
                vehReliabilitySum += graph.getReliability(u,v);
                vehEdges++;
            }
        }

        for (int n : veh.route) {
            if (n == 0) continue; // skip depot
            delivered += graph.nodes[n].demand;
            priorityScore += graph.nodes[n].priority;
            maxPriority += graph.nodes[n].priority;
        }

        totalDelivered += delivered;
        totalCombinedCost += cost;
        totalReliability += vehReliabilitySum;
        totalEdges += vehEdges;

        cout << "Vehicle " << veh.id << " Route: ";
        for (int n : veh.route) cout << n << " ";
        cout << "\nDelivered Demand: " << delivered << "\nTotal Cost: " << cost << "\n\n";
    }

    double avgReliability = totalEdges > 0 ? totalReliability / totalEdges : 0.0;
    double prioritySatisfaction = maxPriority > 0 ? priorityScore / maxPriority : 0.0;

    cout << "Total Combined Cost: " << totalCombinedCost << "\n";
    cout << "Average Reliability: " << avgReliability << "\n";
    cout << "Priority Satisfaction Score: " << prioritySatisfaction << "\n";
    cout << "Idle Vehicles: " << count_if(vehicles.begin(), vehicles.end(),
        [](Vehicle &v) { return v.route.size() <= 1; }) << "\n";
}

};

#endif
