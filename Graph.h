#pragma once
#include <vector>
#include <queue>
#include <limits>
#include "Node.h"
#include "Edge.h"
using namespace std;

struct Graph {
    int N; // number of nodes
    vector<Node> nodes;
    vector<vector<pair<int,int>>> adj; // node -> [(neighbor, cost)]
    vector<vector<double>> reliability; // node -> neighbor reliability

    Graph(int n) : N(n) {
        adj.resize(N);
        reliability.resize(N, vector<double>(N, 1.0)); // default reliability 1.0
    }

    void addEdge(int u, int v, int cost, double rel) {
        adj[u].push_back({v,cost});
        adj[v].push_back({u,cost}); // undirected
        reliability[u][v] = rel;
        reliability[v][u] = rel;
    }

    vector<int> dijkstra(int start, int end) {
        vector<int> dist(N, numeric_limits<int>::max());
        vector<int> parent(N,-1);
        dist[start]=0;

        using pii = pair<int,int>;
        priority_queue<pii,vector<pii>,greater<pii>> pq;
        pq.push({0,start});

        while(!pq.empty()) {
            auto [d,u] = pq.top(); pq.pop();
            if(d>dist[u]) continue;
            for(auto &[v,c]: adj[u]) {
                if(dist[u]+c < dist[v]){
                    dist[v]=dist[u]+c;
                    parent[v]=u;
                    pq.push({dist[v],v});
                }
            }
        }

        vector<int> path;
        if(dist[end]==numeric_limits<int>::max()) return path;
        for(int v=end; v!=-1; v=parent[v])
            path.insert(path.begin(),v);
        return path;
    }

    double getReliability(int u, int v) {
        return reliability[u][v];
    }
};
