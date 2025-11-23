#ifndef DISASTERMANAGER_H
#define DISASTERMANAGER_H

#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <climits>
#include <unordered_set>
#include "Graph.h"
#include "vehicle.h"
#include <cmath>


using namespace std;

struct DisasterManager {
    Graph &graph;
    vector<Vehicle> &vehicles;

    DisasterManager(Graph &g, vector<Vehicle> &v) : graph(g), vehicles(v) {}

    // ==========================================
    // MAIN ALLOCATION METHOD
    // Uses Best-Fit Decreasing for optimal long-term performance
    // ==========================================
    void allocateAndRoute() {
        vector<Node> nodes;
        for (const Node& n : graph.nodes)
            if (n.id != 0) nodes.push_back(n);

        // Sort by priority (descending) - critical areas first
        // Break ties by demand (descending) - better bin packing
        sort(nodes.begin(), nodes.end(), [](const Node& a, const Node& b) {
            if (a.priority != b.priority) return a.priority > b.priority;
            return a.demand > b.demand; // Large items first prevents fragmentation
        });

        vector<bool> nodeAssigned(graph.nodes.size(), false);
        vector<int> remainingCap(vehicles.size());
        for (size_t i = 0; i < vehicles.size(); ++i) 
            remainingCap[i] = vehicles[i].capacity;

        vector<vector<int>> assignedNodes(vehicles.size());

        // Best-Fit: For each node, choose vehicle with minimum 
        // remaining capacity that can still fit it (minimizes waste)
        for (auto& node : nodes) {
            if (nodeAssigned[node.id]) continue;

            int bestVehicle = -1;
            int minRemainingCap = INT_MAX;
            
            // Find vehicle with tightest fit
            for (size_t i = 0; i < vehicles.size(); ++i) {
                if (remainingCap[i] >= node.demand && remainingCap[i] < minRemainingCap) {
                    bestVehicle = i;
                    minRemainingCap = remainingCap[i];
                }
            }

            if (bestVehicle != -1) {
                assignedNodes[bestVehicle].push_back(node.id);
                remainingCap[bestVehicle] -= node.demand;
                nodeAssigned[node.id] = true;
            }
        }

        // Build routes using multi-objective Dijkstra
        for (size_t i = 0; i < vehicles.size(); ++i) {
            vehicles[i].assignedNodes = assignedNodes[i];
            
            if (assignedNodes[i].empty()) {
                vehicles[i].route.clear();
                continue;
            }

            vector<int> fullRoute;
            fullRoute.push_back(0); // Start at depot

            for (int nid : assignedNodes[i]) {
                vector<int> path = graph.dijkstraMultiObjective(fullRoute.back(), nid);
                if (path.empty()) continue;
                path.erase(path.begin());
                fullRoute.insert(fullRoute.end(), path.begin(), path.end());
            }

            // Return to depot
            vector<int> returnPath = graph.dijkstraMultiObjective(fullRoute.back(), 0);
            if (!returnPath.empty()) {
                returnPath.erase(returnPath.begin());
                fullRoute.insert(fullRoute.end(), returnPath.begin(), returnPath.end());
            }

            vehicles[i].route = fullRoute;
        }
    }

   

    // ==========================================
    // Helper: Build Routes from Assignments
    // ==========================================
    void buildRoutes(vector<vector<int>>& assignedNodes) {
        for (size_t i = 0; i < vehicles.size(); ++i) {
            vehicles[i].assignedNodes = assignedNodes[i];
            
            if (assignedNodes[i].empty()) {
                vehicles[i].route.clear();
                continue;
            }

            vector<int> fullRoute;
            fullRoute.push_back(0);

            for (int nid : assignedNodes[i]) {
                vector<int> path = graph.dijkstraMultiObjective(fullRoute.back(), nid);
                if (path.empty()) continue;
                path.erase(path.begin());
                fullRoute.insert(fullRoute.end(), path.begin(), path.end());
            }

            vector<int> returnPath = graph.dijkstraMultiObjective(fullRoute.back(), 0);
            if (!returnPath.empty()) {
                returnPath.erase(returnPath.begin());
                fullRoute.insert(fullRoute.end(), returnPath.begin(), returnPath.end());
            }

            vehicles[i].route = fullRoute;
        }
    }

    // ==========================================
    // Compute and Display Metrics
    // ==========================================
    void computeMetrics() {
        double totalReliability = 0.0;
        int totalEdges = 0;
        int totalCombinedCost = 0;
        int totalDelivered = 0;

        double maxPriority = 0.0;
        double maxDemand = 0.0;
        for (const Node &n : graph.nodes) {
            if (n.id != 0) {
                maxPriority += n.priority;
                maxDemand += n.demand;
            }
        }
        double priorityScore = 0.0;

        for (auto &veh : vehicles) {
            if (veh.route.empty()) {
                cout << "Vehicle " << veh.id << " Route: 0\n";
                cout << "Assigned Nodes: [empty]\n";
                cout << "Delivered Demand: 0\nTotal Cost: 0\n\n";
                continue;
            }

            int delivered = 0;
            int cost = 0;
            double vehReliabilitySum = 0.0;
            int vehEdges = 0;

            // Calculate cost and reliability from edges
            for (size_t i = 0; i + 1 < veh.route.size(); ++i) {
                int u = veh.route[i];
                int v = veh.route[i + 1];

                bool edgeFound = false;
                for (auto &e : graph.adj[u]) {
                    if (e.first == v && graph.isEdgeAvailable(u, v)) {
                        cost += e.second;
                        vehReliabilitySum += graph.getReliability(u, v);
                        ++vehEdges;
                        edgeFound = true;
                        break;
                    }
                }
                if (!edgeFound) continue;
            }

            // Count delivered demand ONLY from assigned nodes
            for (int nid : veh.assignedNodes) {
                delivered += graph.nodes[nid].demand;
                priorityScore += graph.nodes[nid].priority;
            }

            totalDelivered += delivered;
            totalCombinedCost += cost;
            totalReliability += vehReliabilitySum;
            totalEdges += vehEdges;

            cout << "Vehicle " << veh.id << " Route: ";
            for (int n : veh.route) cout << n << " ";
            cout << "\nAssigned Nodes: ";
            for (int n : veh.assignedNodes) cout << n << " ";
            cout << "\nDelivered Demand: " << delivered;
            cout << " / " << veh.capacity << " capacity";
            
            // Show capacity utilization
            double utilization = veh.capacity > 0 ? (100.0 * delivered / veh.capacity) : 0.0;
            cout << " (" << utilization << "% utilization)";
            cout << "\nTotal Cost: " << cost << "\n\n";
        }

        double avgReliability = totalEdges > 0 ? totalReliability / totalEdges : 0.0;
        double prioritySatisfaction = maxPriority > 0.0 ? priorityScore / maxPriority : 0.0;
        double demandSatisfaction = maxDemand > 0.0 ? totalDelivered / maxDemand : 0.0;

        cout << "========================================\n";
        cout << "PERFORMANCE METRICS\n";
        cout << "========================================\n";
        cout << "Total Combined Cost: " << totalCombinedCost << "\n";
        cout << "Average Reliability: " << avgReliability << "\n";
        cout << "Priority Satisfaction Score: " << prioritySatisfaction << "\n";
        
        int idleVehicles = count_if(vehicles.begin(), vehicles.end(),
            [](const Vehicle &v) { return v.route.empty(); });
        cout << "Idle Vehicles: " << idleVehicles << " / " << vehicles.size() << "\n";
        
        // Calculate total capacity utilization
        int totalCapacity = 0;
        for (const auto& v : vehicles) totalCapacity += v.capacity;
        double overallUtilization = totalCapacity > 0 ? (100.0 * totalDelivered / totalCapacity) : 0.0;
        cout << "Overall Capacity Utilization: " << overallUtilization << "%\n";
        cout << "========================================\n";
    }

    // ==========================================
    // Validation: Check Capacity Constraints
    // ==========================================
    bool validateAllocation() const {
        bool valid = true;
        for (size_t i = 0; i < vehicles.size(); ++i) {
            int totalDemand = 0;
            for (int nid : vehicles[i].assignedNodes) {
                totalDemand += graph.nodes[nid].demand;
            }
            
            if (totalDemand > vehicles[i].capacity) {
                cerr << "ERROR: Vehicle " << vehicles[i].id 
                     << " capacity violation! Demand=" << totalDemand 
                     << " > Capacity=" << vehicles[i].capacity << "\n";
                valid = false;
            }
        }
        return valid;
    }
};

#endif