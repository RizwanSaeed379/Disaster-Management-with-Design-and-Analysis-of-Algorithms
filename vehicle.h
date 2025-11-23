#pragma once
#include <vector>
struct Vehicle {
    int id;
    int capacity;
    vector<int> route; // ordered list of node IDs
};
