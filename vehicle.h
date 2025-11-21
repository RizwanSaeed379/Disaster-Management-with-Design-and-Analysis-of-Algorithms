#pragma once
#include <vector>
struct Vehicle {
    int id;
    int capacity;
    std::vector<int> route; // ordered list of node IDs
};
