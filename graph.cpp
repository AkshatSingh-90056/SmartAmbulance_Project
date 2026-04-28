#include "graph.h"
#include <algorithm>
#include <queue>
#include <climits>

using namespace std;

Graph::Graph(int V) {
    this->V = V;
    adj.resize(V);
}

void Graph::addEdge(int u, int v, int weight) {
    adj[u].push_back({v, weight});
    adj[v].push_back({u, weight});
}

vector<int> Graph::dijkstra(int src) {
    vector<int> dist(V, INT_MAX);

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        int currentDist = pq.top().first;
        int currentNode = pq.top().second;
        pq.pop();

        if (currentDist > dist[currentNode]) {
            continue;
        }

        for (auto neighbor : adj[currentNode]) {
            int nextNode = neighbor.first;
            int weight = neighbor.second;

            if (currentDist + weight < dist[nextNode]) {
                dist[nextNode] = currentDist + weight;
                pq.push({dist[nextNode], nextNode});
            }
        }
    }

    return dist;
}

vector<int> Graph::shortestPath(int src, int dest) {
    vector<int> dist(V, INT_MAX);
    vector<int> parent(V, -1);
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    dist[src] = 0;
    pq.push({0, src});

    while (!pq.empty()) {
        int currentDist = pq.top().first;
        int currentNode = pq.top().second;
        pq.pop();

        if (currentDist > dist[currentNode]) {
            continue;
        }

        for (auto neighbor : adj[currentNode]) {
            int nextNode = neighbor.first;
            int weight = neighbor.second;

            if (currentDist + weight < dist[nextNode]) {
                dist[nextNode] = currentDist + weight;
                parent[nextNode] = currentNode;
                pq.push({dist[nextNode], nextNode});
            }
        }
    }

    if (dest < 0 || dest >= V || dist[dest] == INT_MAX) {
        return {};
    }

    vector<int> path;
    for (int node = dest; node != -1; node = parent[node]) {
        path.push_back(node);
    }
    reverse(path.begin(), path.end());
    return path;
}