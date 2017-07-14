#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

struct Node {
    int destinationVertex;
    int maxWeight;

    Node(int destinationVertex, int maxWeight) :
            destinationVertex(destinationVertex), maxWeight(maxWeight) {}
};

class Graph {

    std::vector<std::vector<int> > capacity;
    unsigned size;

public:
    explicit Graph(const std::vector<std::vector<int>> &capacity) {
        this->size = capacity.size();
        this->capacity.assign(size, std::vector<int>(size, 0));
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                this->capacity[i][j] = capacity[i][j];
            }
        }
    }

    /**
     *
     * @return max flow, that can be pushed from istock to stock
     */
    int findPath(int istock, int stock, std::vector<int> &parents) {
        parents.assign(size, -1);
        parents[istock] = istock;
        std::queue<Node> queue;
        queue.push(Node(istock, INT32_MAX));

        while (!queue.empty()) {
            Node n = queue.front();
            queue.pop();

            for (int i = 0; i < size; ++i) {
                if (capacity[n.destinationVertex][i] > 0 && parents[i] == -1) {
                    parents[i] = n.destinationVertex;
                    queue.push(Node(i, std::min(n.maxWeight, capacity[n.destinationVertex][i])));
                    if (i == stock) {
                        return std::min(n.maxWeight, capacity[n.destinationVertex][i]);
                    }
                }
            }
        }
        return 0;
    }

    void pushFlow(int istock, int stock, int amount, std::vector<int> &parents) {
        int from = parents[stock], to = stock;
        while (to != istock) {
            capacity[from][to] -= amount;
            capacity[to][from] += amount;
            to = from;
            from = parents[from];
        }
    }

    int findMaxFlow(int istock, int stock) {
        std::vector<int> parents(size, 0);
        int maxFlow = 0;
        int minWeight;
        while ((minWeight = findPath(0, size - 1, parents)) != 0) {
            maxFlow += minWeight;
            pushFlow(0, size - 1, minWeight, parents);
        }
        return maxFlow;
    }

    int getEdge(int from, int to) {
        return capacity[from][to];
    }

    void setEdge(int from, int to, int delta) {
        capacity[from][to] += delta;
    }
};

void read_graph(unsigned &n, std::vector<std::vector<int> > &data) {
    unsigned m;
    std::cin >> n >> m;
    data.assign(n, std::vector<int>(n, 0));

    int from, to, c;
    for (int i = 0; i < m; ++i) {
        std::cin >> from >> to >> c;
        from--;
        to--;
        data[from][to] = c;
    }
}

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    unsigned n;
    std::vector<std::vector<int> > data;
    read_graph(n, data);

    Graph g(data);
    int maxFlow = g.findMaxFlow(0, n - 1);
    std::cout << maxFlow << std::endl;

    int commands;
    std::cin >> commands;
    int from, to, sign;
    std::vector<int> parents;
    for (int iteration = 0; iteration < commands; ++iteration) {
        std::cin >> sign >> from >> to;
        from--;
        to--;
        if (sign == 1) {
            // increase
            g.setEdge(from, to, 1);
            int delta = g.findPath(0, n - 1, parents);
            if (delta != 0) {
                g.pushFlow(0, n - 1, delta, parents);
            }
            maxFlow += delta;
            std::cout << maxFlow << std::endl;
        } else {
            // decrease
            if (g.getEdge(from, to) != 0) {
                // ребро ненасыщено
                g.setEdge(from, to, -1);
                std::cout << maxFlow << std::endl;
            } else {
                // ребро было насыщено
                // => уберем немного потока
                g.setEdge(to, from, -1);
                int weight = g.findPath(from, to, parents);
                if (weight != 0) {
                    // можем обойти стороной это ребро
                    g.pushFlow(from, to, 1, parents);
                    std::cout << maxFlow << std::endl;
                } else {
                    // надо уменьшить общий поток
                    g.findPath(from, 0, parents);
                    g.pushFlow(from, 0, 1, parents);

                    g.findPath(n - 1, to, parents);
                    g.pushFlow(n - 1, to, 1, parents);

                    maxFlow--;
                    std::cout << maxFlow << std::endl;
                }
            }
        }
    }
}
