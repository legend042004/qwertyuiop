#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <omp.h>

using namespace std;

class Graph {
public:
    int V;
    vector<vector<int>> adj;

    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u); // undirected graph
    }

    void parallelBFS(int start) {
        vector<bool> visited(V, false);
        queue<int> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty()) {
            int levelSize = q.size();
            vector<int> nextLevel;

            #pragma omp parallel for
            for (int i = 0; i < levelSize; ++i) {
                int current;
                #pragma omp critical
                {
                    current = q.front();
                    q.pop();
                    cout << "Visited BFS: " << current << endl;
                }

                for (int neighbor : adj[current]) {
                    if (!visited[neighbor]) {
                        #pragma omp critical
                        {
                            if (!visited[neighbor]) {
                                visited[neighbor] = true;
                                nextLevel.push_back(neighbor);
                            }
                        }
                    }
                }
            }

            for (int node : nextLevel)
                q.push(node);
        }
    }

    void parallelDFSUtil(int node, vector<bool>& visited) {
    bool alreadyVisited = false;

    #pragma omp critical
    {
        if (visited[node]) {
            alreadyVisited = true;
        } else {
            visited[node] = true;
            cout << "Visited DFS: " << node << endl;
        }
    }

    if (alreadyVisited) return;

    #pragma omp parallel for
    for (int i = 0; i < adj[node].size(); ++i) {
        int neighbor = adj[node][i];
        if (!visited[neighbor]) {
            parallelDFSUtil(neighbor, visited);
        }
    }
}


    void parallelDFS(int start) {
        vector<bool> visited(V, false);
        parallelDFSUtil(start, visited);
    }
};

int main() {
    Graph g(7);

    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 3);
    g.addEdge(1, 4);
    g.addEdge(2, 5);
    g.addEdge(2, 6);

    cout << "Parallel BFS:\n";
    g.parallelBFS(0);

    cout << "\nParallel DFS:\n";
    g.parallelDFS(0);

    return 0;
}
