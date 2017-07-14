#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>

/**
 * Class represents segment tree with MAX operation
 * Supports single element modification
 */
class Segment_tree {

    int valid_numbers;
    std::vector<long long> data;

    void build(int vertex, int left, int right, std::vector<long long> &numbers) {
        if (left == right) {
            data[vertex] = numbers[left];
            return;
        }
        int mid = (left + right) / 2;
        build(vertex * 2, left, mid, numbers);
        build(vertex * 2 + 1, mid + 1, right, numbers);
        data[vertex] = std::max(data[vertex * 2], data[vertex * 2 + 1]);
    }

    long long query_helper(int vertex, int node_left, int node_right,
                           int query_left, int query_right) {
        if (query_left > query_right) {
            return 0;
        }
        if (node_left == query_left && node_right == query_right) {
            return data[vertex];
        }
        int mid = (node_left + node_right) / 2;
        long long left_max = query_helper(vertex * 2,
                                          node_left, mid, query_left,
                                          std::min(mid, query_right));
        long long right_max = query_helper(vertex * 2 + 1,
                                           mid + 1, node_right,
                                           std::max(mid + 1, query_left), query_right);
        return std::max(left_max, right_max);
    }

    // get index in data of segment [index, index]
    int get_index_in_tree(int index) {
        int left = 0, right = valid_numbers - 1, vertex = 1;
        while (left != index || right != index) {
            int mid = (left + right) / 2;
            if (index <= mid) {
                right = mid;
                vertex *= 2;
            } else {
                left = mid + 1;
                vertex = vertex * 2 + 1;
            }
        }
        return vertex;
    }

public:
    Segment_tree() {}

    void init(std::vector<long long> &numbers) {
        valid_numbers = numbers.size();
        data.assign(numbers.size() * 4, 0);
        build(1, 0, numbers.size() - 1, numbers);
    }

    // constructor for 'empty' vector of size size
    void init(unsigned size) {
        valid_numbers = size;
        data.assign(valid_numbers * 4, 0);
    }

    // max on interval [left, right]
    long long query(int left, int right) {
        if (left > right) {
            int temp = left;
            left = right;
            right = temp;
        }
        return query_helper(1, 0, valid_numbers - 1, left, right);
    }

    void update(int index, long long new_value) {
        int data_index = get_index_in_tree(index);
        data[data_index] = new_value;
        data_index /= 2;
        while (data_index >= 1) {
            data[data_index] = std::max(data[data_index * 2], data[data_index * 2 + 1]);
            data_index /= 2;
        }
    }

    void update_by_delta(int index, long long delta) {
        int data_index = get_index_in_tree(index);
        data[data_index] += delta;
        data_index /= 2;
        while (data_index >= 1) {
            data[data_index] = std::max(data[data_index * 2], data[data_index * 2 + 1]);
            data_index /= 2;
        }
    }
};

struct Edge {
    int from;
    int to;
    int cost;

    Edge(int from, int to, int cost) : from(from), to(to), cost(cost) {}
};

/**
 * Class represents graph, based on adjacency list
 */
class Graph {
    // adjacency list
    std::vector<std::vector<int> > graph;
    std::vector<Edge> edges;
    int edges_number;
    unsigned vertexes_number;

public:
    explicit Graph(unsigned vertexes_number) {
        this->vertexes_number = vertexes_number;
        graph.assign(vertexes_number, std::vector<int>(0, 0));
        edges_number = 0;
    }

    void add_edge(int from, int to, int cost) {
        graph[from].push_back(edges_number++);
        edges.push_back(Edge(from, to, cost));
        graph[to].push_back(edges_number++);
        edges.push_back(Edge(to, from, cost));
    }

    const std::vector<std::vector<int>> &get_graph() const {
        return graph;
    }

    const std::vector<Edge> &get_edges() const {
        return edges;
    }

    unsigned int get_vertexes_number() const {
        return vertexes_number;
    }
};

class LCA {

    // time of entering and leaving vertex while dfs
    std::vector<int> time_in;
    std::vector<int> time_out;

    int dfs_hepler(Graph &graph, int vertex, int parent, int timer) {

        time_in[vertex] = timer++;

        for (int edge_id : graph.get_graph()[vertex]) {
            int to = graph.get_edges()[edge_id].to;
            if (to != parent) {
                timer = dfs_hepler(graph, to, vertex, timer);
            }
        }
        time_out[vertex] = timer++;
        return timer;
    }


public:
    LCA() {}

    bool is_parent(int vertex_a, int vertex_b) {
        return time_in[vertex_a] <= time_in[vertex_b] &&
               time_out[vertex_a] >= time_out[vertex_b];
    }

    void init(Graph &graph) {
        unsigned size = graph.get_vertexes_number();
        time_in.assign(size, -1);
        time_out.assign(size, -1);

        dfs_hepler(graph, 0, 0, 0);
    }
};

class Heavy_light {
    // parent of vertex
    std::vector<int> parent;

    // number of vertex, which 'heavy' edge from this vertex is directed to
    std::vector<int> heavy;

    // depth of vertex
    std::vector<int> depth;

    // roots of 'heavy' chains
    std::vector<int> root;

    // position in vertexes list, ordered by chains
    // actual position of node in segment tree
    std::vector<int> tree_pos;

    std::vector<bool> used_in_dfs;

    // segment tree for chains
    Segment_tree tree;

    // structure to determine lca fast
    LCA lca;

    /**
     * Method processes subgraph, which starts in node vertex
     * It assigns heavy edges and parents.
     * @return size of subgraph
     */
    int divide_graph_dfs(Graph &graph, int vertex) {

        used_in_dfs[vertex] = true;

        int subgraph_size = 1;
        int max_child_size = 0;

        for (int edge_id: graph.get_graph()[vertex]) {
            Edge edge = graph.get_edges()[edge_id];

            if (!used_in_dfs[edge.to]) {
                parent[edge.to] = edge.from;
                depth[edge.to] = depth[edge.from] + 1;

                int child_size = divide_graph_dfs(graph, edge.to);
                subgraph_size += child_size;

                if (child_size > max_child_size) {
                    max_child_size = child_size;
                    heavy[vertex] = edge.to;
                }
            }
        }

        return subgraph_size;
    }

    /**
    * Method assigns root node for each element inside chain
    * and assigns number of vertex in list, ordered by chains
    */
    void process_chains(int vertexes_number) {
        int currentPos = 0;
        for (int vertex = 0; vertex < vertexes_number; ++vertex) {
            // if vertex doesn't have 'heavy' edge to it's parent
            if (parent[vertex] == -1 || heavy[parent[vertex]] != vertex) {
                // traverse down through heavy vertexes and
                // assign vertex as root of this chain
                for (int j = vertex; j != -1; j = heavy[j]) {
                    root[j] = vertex;
                    tree_pos[j] = currentPos++;
                }
            }
        }
    }

    void clean_arrays(unsigned n) {
        heavy.assign(n, -1);
        parent.assign(n, -1);
        depth.assign(n, -1);
        root.assign(n, -1);
        tree_pos.assign(n, -1);
        used_in_dfs.assign(n, false);
    }


public:
    explicit Heavy_light(Graph &graph) {
        unsigned vertexes_number = graph.get_vertexes_number();
        clean_arrays(vertexes_number);
        divide_graph_dfs(graph, 0);
        process_chains(vertexes_number);
        tree.init(vertexes_number);
        lca.init(graph);
    }

    void update(int index, long delta) {
        tree.update_by_delta(tree_pos[index], delta);
    }

    long long get_max_between_vertexes(int vertex_a, int vertex_b) {

        if (root[vertex_a] == root[vertex_b]) {
            return tree.query(tree_pos[vertex_a], tree_pos[vertex_b]);
        }

        long long res = tree.query(tree_pos[vertex_a], tree_pos[vertex_a]);

        while (!lca.is_parent(root[vertex_b], vertex_a)) {
            res = std::max(res,
                           tree.query(tree_pos[vertex_b], tree_pos[root[vertex_b]]));
            vertex_b = parent[root[vertex_b]];
        }

        while (!lca.is_parent(root[vertex_a], vertex_b)) {
            res = std::max(res,
                           tree.query(tree_pos[vertex_a], tree_pos[root[vertex_a]]));
            vertex_a = parent[root[vertex_a]];
        }

        res = std::max(res, tree.query(tree_pos[vertex_a], tree_pos[vertex_b]));
        return res;
    }
};

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    std::cin.sync_with_stdio(0);
    std::cin.tie();

    unsigned vertexes;
    std::cin >> vertexes;

    Graph graph(vertexes);

    int from, to;
    for (int i = 0; i < vertexes - 1; ++i) {
        std::cin >> from >> to;
        from--;
        to--;
        graph.add_edge(from, to, 0);
    }

    Heavy_light heavy_light(graph);

    long requests;
    std::cin >> requests;
    char command;
    long first, second;
    for (int req = 0; req < requests; ++req) {
        std::cin >> command >> first >> second;
        if (command == 'I') {
            first--;
            heavy_light.update(first, second);
            continue;
        }
        if (command == 'G') {
            first--;
            second--;
            std::cout << heavy_light.get_max_between_vertexes(first, second)
                      << std::endl;
            continue;
        }
    }
}
