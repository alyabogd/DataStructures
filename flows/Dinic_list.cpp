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

    bool fill_layers() {
        std::queue<int> queue;
        layers.assign(num_of_verts, -1);

        layers[istock] = 0;
        queue.push(istock);

        while (!queue.empty()) {
            int vertex = queue.front();
            queue.pop();

            for (int i = 0; i < graph[vertex].size(); ++i) {
                Edge edge = edges[graph[vertex][i]];

                if (layers[edge.to] == -1 && edge.flow < edge.capacity) {
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
            if (layers[edge.to] != layers[from] + 1) {
                continue;
            }
            int flow_portion = find_blocking_flow(
                    edge.to, std::min(flow, edge.capacity - edge.flow)
            );
            if (flow_portion != 0) {
                edges[graph[from][edge_id]].flow += flow_portion;
                int edgeNumber = graph[from][edge_id];
                if (edgeNumber % 2 == 0) {
                    edgeNumber++;
                } else {
                    edgeNumber--;
                }
                edges[edgeNumber].flow -= flow_portion;
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

    int get_max_flow() {
        int max_flow = 0;

        while (fill_layers()) {
            pointers.assign(num_of_verts, 0);
            int pushed = find_blocking_flow(istock, INT32_MAX);
            while (pushed != 0) {
                max_flow += pushed;
                pushed = find_blocking_flow(istock, INT32_MAX);
            }
        }
        return max_flow;
    }
};

class Graph_former {

    int height;
    int width;
    std::vector<std::vector<char> > matrix;

    std::vector<std::vector<int> > graph;
    std::vector<Edge> edges;
    int edges_count;

    int capacity_from_istock;
    int capacity_to_stock;

    int get_int_value(char symbol) {
        switch (symbol) {
            case '0':
                return 0;
            case '1':
                return 1;
            case '2':
                return 2;
            case '3':
                return 3;
            case '4':
                return 4;
            default:
                return -1;
        }
    }

    void add_edge(int from_x, int from_y, int to_x, int to_y, int cap) {
        int u_id_from = from_x * width + from_y + 1;
        int u_id_to = to_x * width + to_y + 1;

        // u_id_from -> u_id_to
        graph[u_id_from].push_back(edges_count++);
        edges.push_back(Edge(u_id_from, u_id_to, cap, 0));
        graph[u_id_to].push_back(edges_count++);
        edges.push_back(Edge(u_id_to, u_id_from, 0, 0));
    }


public:
    Graph_former(int height, int width, const std::vector<std::vector<char>> &matrix) :
            height(height), width(width), matrix(matrix) {

        graph.assign(width * height + 2, std::vector<int>(0, 0));
        edges_count = 0;
        capacity_from_istock = 0;
        capacity_to_stock = 0;
    }

    void form_graph() {
        for (int row = 0; row < height; ++row) {
            for (int col = 0; col < width; ++col) {

                if (matrix[row][col] == '0') {
                    continue;
                }

                int u_id = row * width + col + 1;

                // put 'connection' edge
                if ((row + col) % 2 == 0) {
                    // edge from istock
                    graph[0].push_back(edges_count++);
                    edges.push_back(Edge(0, u_id, get_int_value(matrix[row][col]), 0));
                    graph[u_id].push_back(edges_count++);
                    edges.push_back(Edge(u_id, 0, 0, 0));
                    capacity_from_istock += get_int_value(matrix[row][col]);

                } else {
                    // edge to stock
                    int stock_id = height * width + 1;
                    graph[u_id].push_back(edges_count++);
                    edges.push_back(Edge(u_id, stock_id, get_int_value(matrix[row][col]), 0));
                    graph[stock_id].push_back(edges_count++);
                    edges.push_back(Edge(stock_id, u_id, 0, 0));
                    capacity_to_stock += get_int_value(matrix[row][col]);
                }

                // top cell
                if (row - 1 >= 0 && matrix[row - 1][col] != '0') {
                    add_edge(row, col, row - 1, col, 1);
                }

                // left cell
                if (col - 1 >= 0 && matrix[row][col - 1] != '0') {
                    add_edge(row, col, row, col - 1, 1);
                }

                // bottom cell
                if (row + 1 < height && matrix[row + 1][col] != '0') {
                    add_edge(row, col, row + 1, col, 1);
                }

                // right cell
                if (col + 1 < width && matrix[row][col + 1] != '0') {
                    add_edge(row, col, row, col + 1, 1);
                }
            }
        }
    }

    int get_capacity_from_istock() const {
        return capacity_from_istock;
    }

    int get_capacity_to_stock() const {
        return capacity_to_stock;
    }

    const std::vector<std::vector<int>> &get_graph() const {
        return graph;
    }

    const std::vector<Edge> &get_edges() const {
        return edges;
    }
};

void read_input(unsigned &height, unsigned &width, std::vector<std::vector<char> > &matrix) {
    std::cin >> height >> width;
    matrix.assign(height, std::vector<char>(width, '0'));
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            std::cin >> matrix[i][j];
        }
    }
}


int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    std::cin.sync_with_stdio(0);
    std::cin.tie();

    unsigned height, width;
    std::vector<std::vector<char> > matrix;

    read_input(height, width, matrix);
    Graph_former former(height, width, matrix);
    former.form_graph();

    Graph graph(former.get_graph(), former.get_edges());

    int max_flow = graph.get_max_flow();

    if (max_flow != 0 &&
        max_flow == former.get_capacity_from_istock() &&
        max_flow == former.get_capacity_to_stock()) {
        std::cout << "Valid" << std::endl;
    } else {
        std::cout << "Invalid" << std::endl;
    }
}
