#include <iostream>
#include <vector>
#include <cstdio>

struct Edge {
    int number;

    int from;
    int to;
    int cost;

    int capacity;
    int flow;

    bool is_first;

    Edge(int number, int from, int to, int cost, int cap, bool b) :
            number(number), from(from), to(to), cost(cost), capacity(cap), is_first(b) {
        flow = 0;
    }
};

struct smallEdge {
    int to;
    int initial_number;
    bool can_take;

    smallEdge(int to, int initial_number) : to(to), initial_number(initial_number) {
        can_take = true;
    }
};

class Graph {

    unsigned vertexes_number;
    std::vector<Edge> edges;

public:
    Graph(unsigned vertexes_number, const std::vector<Edge> &edges) :
            vertexes_number(vertexes_number), edges(edges) {}

    /**
     * Ford-Bellman algorithm implementation: shortest path from single
     * vertex to all others in a weighted graph
     *
     * @param source - start vertex
     * @param path - stores parent for each vertex in a path
     * @param distance - stores cost from i-th vertex to source vertex
     *
     * @param edge_number - supporting array. Stores number of edge
     *                      in a graph, that is included in path
     */
    void find_min_way(const int source,
                      std::vector<Edge> edgess,
                      std::vector<int> &path,
                      std::vector<int> &distance,
                      std::vector<int> &edge_number) {

        path.assign(vertexes_number, -1);
        distance.assign(vertexes_number, 1000005);
        edge_number.assign(vertexes_number, -1);

        path[source] = source;
        distance[source] = 0;

        bool was_modified = true;
        for (int itera = 0; was_modified && itera < vertexes_number; ++itera) {
            was_modified = false;

            int current_edge = -1;
            for (Edge edge : edgess) {
                current_edge++;
                if (edge.flow < edge.capacity) {
                    if (distance[edge.to] > distance[edge.from] + edge.cost) {
                        distance[edge.to] = distance[edge.from] + edge.cost;
                        path[edge.to] = edge.from;
                        edge_number[edge.to] = current_edge;
                        was_modified = true;
                    }
                }
            }
        }
    }

    /**
     * Given parent for each vertex in the way found forms a way - just sequence of edges
     *
     * @param istock  - start vertex
     * @param stock - end vertex
     * @param path - stores parent for each vertex
     * @param edge_number - supporting array. Stores number of edge
     *                      in a graph, that is included in path
     * @param edges_on_way - numbers of edges in a way
     *
     * @return - cost from s to t
     */
    int form_way(const int istock, const int stock,
                 const std::vector<int> &path,
                 const std::vector<int> &edge_number,
                 std::vector<int> &edges_on_way) {

        edges_on_way.assign(0, 0);

        if (path[stock] == -1) {
            return 0;
        }

        int cost = 0;
        int to = stock;
        int from = path[stock];
        while (to != istock) {
            const int current_edge = edge_number[to];
            edges_on_way.push_back(current_edge);
            cost += edges[current_edge].cost;

            to = from;
            from = path[from];
        }

        return cost;
    }

    void push_flow(const int flow,
                   std::vector<int> edges_way) {
        for (int edge: edges_way) {
            edges[edge].flow += flow;
            if (edges[edge].is_first) {
                edges[edge + 1].flow -= flow;
            } else {
                edges[edge - 1].flow -= flow;
            }
        }
    }


    // поток минимальной стоимости величины power
    // false если не может найти очередной поток
    bool find_min_cost(int power, long long &cost) {
        std::vector<int> path(vertexes_number, -1);
        std::vector<int> distance(vertexes_number, 1000006);
        std::vector<int> edge_number(vertexes_number, -1);
        std::vector<int> way;

        int to = vertexes_number - 1;
        cost = 0;

        for (int itera = 0; itera < power; ++itera) {
            find_min_way(0, edges, path, distance, edge_number);

            if (path[to] != -1) {
                // push 1 flow
                cost += form_way(0, to, path, edge_number, way);
                push_flow(1, way);

            } else {
                return false;
            }
        }
        return true;
    };

    void find_flow_ways(std::vector<std::vector<int> > &ways, int number) {
        ways.assign(number, std::vector<int>(0, 0));
        std::vector<std::vector<smallEdge> > new_graph(vertexes_number,
                                                       std::vector<smallEdge>(0, smallEdge(0, 0)));
        for (Edge edge : edges) {
            if (edge.flow == 1) {
                new_graph[edge.from].push_back(smallEdge(edge.to, edge.number));
            }
        }

        for (int num = 0; num < number; ++num) {
            std::vector<int> way;
            int cur = 0;
            while (cur != vertexes_number - 1) {
                int index = 0;
                while (index < new_graph[cur].size() && !new_graph[cur][index].can_take) {
                    index++;
                }
                new_graph[cur][index].can_take = false;
                way.push_back(new_graph[cur][index].initial_number);
                cur = new_graph[cur][index].to;
            }
            ways.push_back(way);
        }
    }
};

bool solve(Graph &graph, const int pekh_num, double &mean_time,
           std::vector<std::vector<int> > &ways) {

    long long cost = 0;
    bool is_flow = graph.find_min_cost(pekh_num, cost);
    if (!is_flow) {
        return false;
    }
    mean_time = (cost * 1.0) / pekh_num;
    graph.find_flow_ways(ways, pekh_num);
    return true;
}


void read_input(int &vertexes, int &peh_num, std::vector<Edge> &graph) {
    unsigned edges_num;
    std::cin >> vertexes >> edges_num >> peh_num;

    int from, to, cost;
    for (int i = 0; i < edges_num; ++i) {
        std::cin >> from >> to >> cost;
        from--;
        to--;
        graph.push_back(Edge(i + 1, from, to, cost, 1, true)); // прямое туда
        graph.push_back(Edge(i + 1, to, from, -cost, 0, false)); // обратное туда
        graph.push_back(Edge(i + 1, to, from, cost, 1, true)); // прямое оттуда
        graph.push_back(Edge(i + 1, from, to, -cost, 0, false)); // обратное оттуда
    }
}

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    int vertex_number;
    int peh_number;
    std::vector<Edge> edges;

    read_input(vertex_number, peh_number, edges);

    Graph graph(vertex_number, edges);

    double time;
    std::vector<std::vector<int> > ways;
    bool is_solved = solve(graph, peh_number, time, ways);

    if (is_solved) {
        printf("%f\n", time);
        for (int way_num = 0; way_num < ways.size(); ++way_num) {
            if (ways[way_num].size() == 0) {
                continue;
            }
            std::cout << ways[way_num].size() << " ";
            for (int j = 0; j < ways[way_num].size(); ++j) {
                std::cout << ways[way_num][j] << " ";
            }
            std::cout << std::endl;
        }
    } else {
        std::cout << -1 << std::endl;
    }
}
