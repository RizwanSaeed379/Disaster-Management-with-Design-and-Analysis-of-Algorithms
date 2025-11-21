#pragma once
#include <algorithm>
#include "Graph.h"
#include "Vehicle.h"
using namespace std;

struct DisasterManager {
    Graph &graph;
    vector<Vehicle> &vehicles;

    DisasterManager(Graph &g, vector<Vehicle> &v) : graph(g), vehicles(v) {}

    void allocateAndRoute() {
        vector<Node> nodes = graph.nodes;
        nodes.erase(nodes.begin()); // remove depot
        sort(nodes.begin(),nodes.end(),[](Node a, Node b){
            return a.priority > b.priority;
        });

        int vIdx=0;
        for(auto &node: nodes){
            Vehicle &veh = vehicles[vIdx];
            int totalDemand=0;
            for(int n: veh.route) totalDemand+=graph.nodes[n].demand;
            if(totalDemand + node.demand <= veh.capacity) {
                veh.route.push_back(node.id);
            } else {
                vIdx++;
                if(vIdx >= vehicles.size()) break;
                vehicles[vIdx].route.push_back(node.id);
            }
        }

        for(auto &veh: vehicles){
            vector<int> fullRoute;
            fullRoute.push_back(0);
            for(int nid: veh.route){
                vector<int> path = graph.dijkstra(fullRoute.back(), nid);
                path.erase(path.begin());
                fullRoute.insert(fullRoute.end(), path.begin(), path.end());
            }
            vector<int> returnPath = graph.dijkstra(fullRoute.back(),0);
            returnPath.erase(returnPath.begin());
            fullRoute.insert(fullRoute.end(), returnPath.begin(), returnPath.end());
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

        for(auto &veh: vehicles){
            int delivered=0;
            int cost=0;
            double vehReliabilitySum = 0;
            int vehEdges=0;

            for(size_t i=0;i<veh.route.size()-1;i++){
                int u = veh.route[i];
                int v = veh.route[i+1];

                // Find cost
                for(auto &[nbr,c]: graph.adj[u]){
                    if(nbr==v){
                        cost += c;
                        break;
                    }
                }

                // reliability
                vehReliabilitySum += graph.getReliability(u,v);
                vehEdges++;
            }

            for(int n: veh.route){
                delivered += graph.nodes[n].demand;
                priorityScore += graph.nodes[n].priority;
                maxPriority += graph.nodes[n].priority;
            }

            totalDelivered += delivered;
            totalCombinedCost += cost;
            totalReliability += vehReliabilitySum;
            totalEdges += vehEdges;

            cout<<"Vehicle "<<veh.id<<" Route: ";
            for(int n: veh.route) cout<<n<<" ";
            cout<<"\nDelivered Demand: "<<delivered<<"\nTotal Cost: "<<cost<<"\n\n";
        }

        double avgReliability = totalEdges>0 ? totalReliability/totalEdges : 0.0;
        double prioritySatisfaction = maxPriority>0 ? priorityScore/maxPriority : 0.0;

        cout<<"Total Combined Cost: "<<totalCombinedCost<<"\n";
        cout<<"Average Reliability: "<<avgReliability<<"\n";
        cout<<"Priority Satisfaction Score: "<<prioritySatisfaction<<"\n";
    }
};
