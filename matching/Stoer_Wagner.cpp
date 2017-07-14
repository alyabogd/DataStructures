#include <iostream>
#include <vector>
#include <climits>

class Graph {
    unsigned size;
    std::vector<std::vector<long long> > graph;

    int findMaxWeightedVertex(std::vector<bool> &exists,
                              std::vector<bool> &is_in_a,
                              std::vector<long long> &weights) {
        int ans = -1;
        for (int i = 0; i < size; ++i) {
            if (exists[i] && !is_in_a[i]) {
                if (ans == -1 || weights[ans] < weights[i]) {
                    ans = i;
                }
            }
        }
        return ans;
    }

public:
    Graph(unsigned int size, const std::vector<std::vector<long long>> &graph) :
            size(size), graph(graph) {}

    void findMinCut(long long &cost, std::vector<int> &vertexesInCut) {
        cost = LONG_LONG_MAX;
        vertexesInCut.assign(0, 0);

        std::vector<std::vector<int> > compressedVerts(size, std::vector<int>(1, 0));
        for (int i = 0; i < size; ++i) {
            compressedVerts[i].assign(1, i);
        }
        std::vector<long long> weights(size, 0LL);
        std::vector<bool> is_in_a(size, false);
        std::vector<bool> exists(size, true);

        for (int phase = 0; phase < size - 1; ++phase) {
            // make A empty
            is_in_a.assign(size, false);
            weights.assign(size, 0);
            int previous = 0;
            for (int iter = 0; iter < size - phase; ++iter) {
                // get the most 'weighted' vertex,
                // add to A
                // previous ans last vertexes = s and t. check if it's best cut
                int maxWeightedVertex = findMaxWeightedVertex(exists, is_in_a, weights);
                if (iter != size - phase - 1) {
                    // it's not last iteration
                    // just process vertexes
                    previous = maxWeightedVertex;
                    is_in_a[maxWeightedVertex] = true;
                    for (int i = 0; i < size; ++i) {
                        weights[i] += graph[maxWeightedVertex][i];
                    }
                } else {
                    // it's last iteration
                    // check if it's best cut
                    if (weights[maxWeightedVertex] < cost) {
                        cost = weights[maxWeightedVertex];
                        vertexesInCut = compressedVerts[maxWeightedVertex];
                    }
                    // update compressedVerts
                    for (int i = 0; i < compressedVerts[maxWeightedVertex].size(); ++i) {
                        compressedVerts[previous].push_back(compressedVerts[maxWeightedVertex][i]);
                    }
                    // update graph
                    for (int i = 0; i < size; ++i) {
                        graph[previous][i] += graph[maxWeightedVertex][i];
                        graph[i][previous] += graph[maxWeightedVertex][i];
                    }
                    exists[maxWeightedVertex] = false;
                }
            }
        }
    }
};

void read_input(unsigned &vertexes, std::vector<std::vector<long long> > &adjacencyMatrix) {
    int edges;
    std::cin >> vertexes >> edges;
    adjacencyMatrix.assign(vertexes, std::vector<long long>(vertexes, 0LL));

    int from, to;
    long long weight;
    for (int i = 0; i < edges; ++i) {
        std::cin >> from >> to >> weight;
        from--;
        to--;
        adjacencyMatrix[from][to] += weight;
        adjacencyMatrix[to][from] += weight;
    }
}

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    unsigned size;
    std::vector<std::vector<long long> > adjacencyMatrix;
    read_input(size, adjacencyMatrix);

    Graph graph(size, adjacencyMatrix);

    long long cost = LONG_LONG_MAX;
    std::vector<int> verts;

    graph.findMinCut(cost, verts);

    std::cout << cost << std::endl;
    std::cout << verts.size() << std::endl;
    for (int i = 0; i < verts.size(); ++i) {
        std::cout << verts[i] + 1 << " ";
    }
    std::cout << std::endl;
}
