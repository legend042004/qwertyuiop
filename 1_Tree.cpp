#include <iostream>
#include <vector>
#include <queue>
#include <omp.h>

using namespace std;

class Tree {
public:
    int V;
    vector<vector<int>> adj;

    Tree(int V) {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int parent, int child) {
        adj[parent].push_back(child);
        adj[child].push_back(parent); // bidirectional for undirected tree
    }

    void parallelBFS(int root) {
        vector<bool> visited(V, false);
        queue<int> q;

        visited[root] = true;
        q.push(root);

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

    void parallelDFSUtil(int node, int parent, vector<bool>& visited) {
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
        int child = adj[node][i];
        if (child != parent && !visited[child]) {
            parallelDFSUtil(child, node, visited);
        }
    }
}


    void parallelDFS(int root) {
        vector<bool> visited(V, false);
        parallelDFSUtil(root, -1, visited);
    }
};

int main() {
    Tree t(7);
    t.addEdge(0, 1);
    t.addEdge(0, 2);
    t.addEdge(1, 3);
    t.addEdge(1, 4);
    t.addEdge(2, 5);
    t.addEdge(2, 6);

    cout << "Parallel BFS on Tree:\n";
    t.parallelBFS(0);

    cout << "\nParallel DFS on Tree:\n";
    t.parallelDFS(0);

    return 0;
}
