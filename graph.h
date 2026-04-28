#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <utility>

using namespace std;

class Graph {
private:
    int V; 
    vector<vector<pair<int, int>>> adj;

public:
    Graph(int V);

    void addEdge(int u, int v, int weight);

    vector<int> dijkstra(int src);
    vector<int> shortestPath(int src, int dest);
};

#endif