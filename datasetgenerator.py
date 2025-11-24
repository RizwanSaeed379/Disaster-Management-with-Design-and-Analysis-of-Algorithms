import json
import random
import math
import os

# Settings for datasets
datasets = [
    {"nodes": 50, "edges": 100},
    {"nodes": 100, "edges": 500},
    {"nodes": 200, "edges": 1000},
    {"nodes": 300, "edges": 1500},
    {"nodes": 400, "edges": 2000},
    {"nodes": 500, "edges": 2500},
    {"nodes": 600, "edges": 3000},
    {"nodes": 800, "edges": 4000},
    {"nodes": 1000, "edges": 5000},
    {"nodes": 1500, "edges": 10000},
    {"nodes": 2000, "edges": 15000},
    {"nodes": 2500, "edges": 20000},
    {"nodes": 3000, "edges": 25000},
    {"nodes": 4000, "edges": 40000},
    {"nodes": 4500, "edges": 45000},
    {"nodes": 5000, "edges": 50000},
]

output_folder = "datasets"
os.makedirs(output_folder, exist_ok=True)

for idx, data in enumerate(datasets, start=1):
    N = data["nodes"]
    E = data["edges"]

    # Nodes
    nodes = [{"id": 0, "demand": 0, "priority": 0}]
    for i in range(1, N):
        node = {
            "id": i,
            "demand": random.randint(1, 10),
            "priority": round(random.uniform(0.5, 1.0), 2)
        }
        nodes.append(node)

    # Spanning tree to ensure connectivity
    edges = []
    connected = [0]
    remaining = list(range(1, N))
    while remaining:
        u = random.choice(connected)
        v = random.choice(remaining)
        cost = random.randint(1, 10)
        reliability = round(random.uniform(0.5, 1.0), 2)
        edges.append({"u": u, "v": v, "cost": cost, "reliability": reliability})
        connected.append(v)
        remaining.remove(v)

    # Add remaining random edges
    while len(edges) < E:
        u = random.randint(0, N-1)
        v = random.randint(0, N-1)
        if u == v: 
            continue
        # prevent duplicate edges
        if any(e["u"] == u and e["v"] == v or e["u"] == v and e["v"] == u for e in edges):
            continue
        cost = random.randint(1, 10)
        reliability = round(random.uniform(0.5, 1.0), 2)
        edges.append({"u": u, "v": v, "cost": cost, "reliability": reliability})

    # Vehicles: 1 per 50 nodes, capacity = 1.2 × avg demand
    num_vehicles = max(1, N // 50)
    avg_demand = sum(node["demand"] for node in nodes) / num_vehicles
    vehicles = [{"id": i+1, "capacity": math.ceil(avg_demand * 1.2)} for i in range(num_vehicles)]

    # Build JSON
    dataset_json = {
        "graph": {
            "num_nodes": N,
            "nodes": nodes,
            "edges": edges
        },
        "vehicles": vehicles
    }

    # Save to file
    filename = f"{output_folder}/dataset_{idx}.json"
    with open(filename, "w") as f:
        json.dump(dataset_json, f, indent=2)

    print(f"Generated {filename}")
