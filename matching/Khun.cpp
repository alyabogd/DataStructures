#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <math.h>

struct Vertex {
    long double x_coord;
    long double y_coord;
    // time in minutes
    int time;

    Vertex(long double x_coord, long double y_coord, int time) :
            x_coord(x_coord), y_coord(y_coord), time(time) {}
};

// bipartite graph
class Graph {

    // stores list of vertexes, adjacent to i-th vertex (for 1-st part)
    std::vector<std::vector<int> > graph;
    std::vector<int> matchings;
    std::vector<bool> used_in_kuhn;
    int num_of_verts;

    int max_match_count;

    bool find_max_matching(int vertex) {
        if (graph[vertex].size() == 0) {
            return false;
        }
        if (used_in_kuhn[vertex]) {
            return false;
        }
        used_in_kuhn[vertex] = true;
        for (int i = 0; i < graph[vertex].size(); ++i) {
            int to = graph[vertex][i];
            if (matchings[to] == -1 || find_max_matching(matchings[to])) {
                matchings[to] = vertex;
                return true;
            }
        }
        return false;
    }


public:
    explicit Graph(const std::vector<std::vector<int>> &graph) : graph(graph) {
        num_of_verts = graph.size();
        max_match_count = 0;
        matchings.assign(num_of_verts, -1);
    }

    int find_max_match_value() {
        for (int i = 0; i < num_of_verts; ++i) {
            used_in_kuhn.assign(num_of_verts, false);
            bool is_chosen = find_max_matching(i);
            if (is_chosen) {
                max_match_count++;
            }
        }
        return max_match_count;
    }
};

class Graph_former {
    std::vector<Vertex> vertexes;
    std::vector<std::vector<int> > graph;
    double speed;

    long double get_distance(int index_f, int index_s) {
        long double x_f = vertexes[index_f].x_coord;
        long double y_f = vertexes[index_f].y_coord;
        long double x_s = vertexes[index_s].x_coord;
        long double y_s = vertexes[index_s].y_coord;
        return sqrt((x_f - x_s) * (x_f - x_s) + (y_f - y_s) * (y_f - y_s));
    }

public:
    Graph_former(const std::vector<Vertex> &vertexes, double speed)
            : vertexes(vertexes), speed(speed) {
        graph.assign(vertexes.size(), std::vector<int>(0, 0));
    }

    void build() {
        for (int firs = 0; firs < vertexes.size(); ++firs) {
            for (int sec = 0; sec < vertexes.size(); ++sec) {
                // check if i and sec are reachable
                if (firs == sec) {
                    continue;
                }
                long long time_delta = vertexes[sec].time - vertexes[firs].time;
                if (time_delta < 0) {
                    continue;
                }

                long double dist = get_distance(firs, sec);
                if (dist * 60 <= speed * time_delta) {
                    graph[firs].push_back(sec);
                }
            }
        }
    }

    const std::vector<std::vector<int>> &get_graph() const {
        return graph;
    }
};

void read(int &number, double &speed, std::vector<Vertex> &vertexes) {
    std::cin >> number >> speed;
    long double x_coord, y_coord;
    int hours, minutes;
    for (int i = 0; i < number; ++i) {
        scanf("%d:%d", &hours, &minutes);
        std::cin >> x_coord >> y_coord;
        vertexes.push_back(Vertex(x_coord, y_coord, hours * 60 + minutes));
    }
}

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    int number;
    double speed;
    std::vector<Vertex> vertexes;

    read(number, speed, vertexes);

    Graph_former former(vertexes, speed);
    former.build();
    Graph graph(former.get_graph());

    int max_match = graph.find_max_match_value();

    std::cout << number - max_match << std::endl;
}
