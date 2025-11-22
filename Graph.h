#pragma once
#include <vector>
#include <queue>
#include <limits>
#include "node.h"
#include "edge.h"
using namespace std;

struct Graph {
    int N; // number of nodes
    vector<Node> nodes;
    vector<vector<pair<int,int>>> adj;        // node -> [(neighbor, cost)]
    vector<vector<double>> reliability;       // node -> neighbor reliability
    vector<vector<bool>> edgeAvailable;       // dynamic availability

    Graph(int n) : N(n) {
        adj.resize(N);
        reliability.resize(N, vector<double>(N, 1.0));
        edgeAvailable.resize(N, vector<bool>(N, true)); // initially all edges available
    }

    void addEdge(int u, int v, int cost, double rel) {
        adj[u].push_back({v,cost});
        adj[v].push_back({u,cost});   // undirected
        reliability[u][v] = rel;
        reliability[v][u] = rel;
        edgeAvailable[u][v] = true;
        edgeAvailable[v][u] = true;
    }

    void setEdgeAvailability(int u, int v, bool avail) {
        edgeAvailable[u][v] = avail;
        edgeAvailable[v][u] = avail;  // for undirected edges
    }

    bool isEdgeAvailable(int u, int v) {
        return edgeAvailable[u][v];
    }

    double getReliability(int u, int v) {
        return reliability[u][v];
    }

 vector<int> dijkstraMultiObjective(int start, int end, double alpha=1.0, double beta=1.0) {
    struct State {
        double effCost;
        int u;
        double relSum; 
        bool operator>(const State &other) const {
            if (effCost != other.effCost) return effCost > other.effCost;
            return relSum < other.relSum;
        }
    };

    struct CompareState {
        bool operator()(const State &a, const State &b) const {
            if (a.effCost != b.effCost) return a.effCost > b.effCost;
            return a.relSum < b.relSum;
        }
    };

    vector<double> dist(N, numeric_limits<double>::max());
    vector<int> parent(N, -1);
    vector<double> pathReli(N, 0.0); 
    dist[start] = 0;
    pathReli[start] = 1.0;

    priority_queue<State, vector<State>, CompareState> pq;
    pq.push({0.0, start, 1.0});

    while (!pq.empty()) {
        State top = pq.top(); pq.pop();
        double d = top.effCost;
        int u = top.u;
        double relSum = top.relSum;

        if (d > dist[u]) continue;

        for (auto &pair : adj[u]) {
            int v = pair.first;
            double c = pair.second;

            if (!isEdgeAvailable(u, v)) continue;

            double edgeRel = reliability[u][v];
            double effCost = alpha * c + beta * (1.0 - edgeRel);
            double newRelSum = relSum * edgeRel;

            if (dist[u] + effCost < dist[v] || 
                (abs(dist[u] + effCost - dist[v]) < 1e-6 && newRelSum > pathReli[v])) {
                dist[v] = dist[u] + effCost;
                parent[v] = u;
                pathReli[v] = newRelSum;
                pq.push({dist[v], v, newRelSum});
            }
        }
    }

    vector<int> path;
    if (dist[end] == numeric_limits<double>::max()) return path;
    for (int v = end; v != -1; v = parent[v])
        path.insert(path.begin(), v);

    return path;
    }
    
};

