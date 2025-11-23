#pragma once
#include <vector>

using namespace std;

struct Vehicle {
    int id;
    int capacity;
    vector<int> route;         // Full path including intermediate nodes: e.g., [0, 1, 2, 3, 0]
    vector<int> assignedNodes; // Only nodes assigned for delivery: e.g., [1, 3]
};