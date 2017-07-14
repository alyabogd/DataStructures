#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

struct Edge {
    int from;
    int to;
    int capacity;
    int flow;

    Edge(int from, int to, int capacity, int flow) :
            from(from), to(to), capacity(capacity), flow(flow) {}
};

class Graph {

    // adjacency list
    std::vector<std::vector<int> > graph;
    std::vector<Edge> edges;

    std::vector<int> layers;
    std::vector<int> pointers;

    int stock;
    int istock;
    int num_of_verts;

    bool fill_layers(int limit) {
        std::queue<int> queue;
        layers.assign(num_of_verts, -1);

        layers[istock] = 0;
        queue.push(istock);

        while (!queue.empty()) {
            int vertex = queue.front();
            queue.pop();

            for (int i = 0; i < graph[vertex].size(); ++i) {
                Edge edge = edges[graph[vertex][i]];

                if (layers[edge.to] == -1 && edge.capacity - edge.flow >= limit) {
                    layers[edge.to] = layers[vertex] + 1;
                    queue.push(edge.to);
                }
            }
        }
        return layers[stock] != -1;
    }

    int find_blocking_flow(int from, int flow) {
        if (flow == 0 || from == stock) {
            return flow;
        }
        for (int &edge_id = pointers[from]; edge_id < graph[from].size(); ++edge_id) {
            Edge edge = edges[graph[from][edge_id]];

            if (layers[edge.to] != layers[from] + 1 || edge.capacity - edge.flow < flow) {
                continue;
            }

            int flow_portion = find_blocking_flow(
                    edge.to, std::min(flow, edge.capacity - edge.flow));

            if (flow_portion != 0) {
                int edge_number = graph[from][edge_id];
                edges[edge_number].flow += flow_portion;
                edges[edge_number ^ 1].flow -= flow_portion;
                return flow_portion;
            }
        }
        return 0;
    }

public:
    Graph(const std::vector<std::vector<int>> &graph, const std::vector<Edge> &edges)
            : graph(graph), edges(edges) {
        stock = graph.size() - 1;
        istock = 0;
        num_of_verts = graph.size();
    }

    long long get_max_flow() {

        long long max_flow = 0;

        for (int limit = 1000000000; limit >= 1;) {
            if (!fill_layers(limit)) {
                limit /= 2;
                continue;
            }

            pointers.assign(num_of_verts, 0);
            int pushed;
            while ((pushed = find_blocking_flow(istock, limit)) != 0) {
                max_flow += pushed;
            }
        }

        return max_flow;
    }

    const std::vector<Edge> &getEdges() const {
        return edges;
    }
};

class Graph_former {
    std::vector<std::vector<int> > graph;
    std::vector<Edge> edges;

    int edges_count;

public:
    explicit Graph_former(unsigned verts_number) {
        edges_count = 0;
        graph.assign(verts_number, std::vector<int>(0, 0));
    }

    void add_edge(int from, int to, int cap) {
        graph[from].push_back(edges_count++);
        edges.push_back(Edge(from, to, cap, 0));
        graph[to].push_back(edges_count++);
        edges.push_back(Edge(to, from, 0, 0));
    }

    const std::vector<std::vector<int>> &getGraph() const {
        return graph;
    }

    const std::vector<Edge> &getEdges() const {
        return edges;
    }
};

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    std::cin.sync_with_stdio(0);
    std::cin.tie();

    unsigned verts, edges;
    std::cin >> verts >> edges;

    Graph_former former(verts);
    int from, to, cap;
    for (int i = 0; i < edges; ++i) {
        std::cin >> from >> to >> cap;
        from--;
        to--;
        former.add_edge(from, to, cap);
    }

    Graph graph(former.getGraph(), former.getEdges());
    long long maxFlow = graph.get_max_flow();
    std::cout << maxFlow << std::endl;
    std::vector<Edge> edgesList = graph.getEdges();
    for (int i = 0; i < edgesList.size(); i += 2) {
        std::cout << edgesList[i].flow << std::endl;
    }
}
