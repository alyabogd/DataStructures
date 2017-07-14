
#include<iostream>
#include<vector>
#include<queue>
#include<algorithm>

std::vector<std::vector<int> > graph;
std::vector<std::vector<int> > flow;
std::vector<int> layers;
std::vector<int> pointers;

unsigned numOfVerts;
int istock, stock;
int money;

void assignGlobal() {
    graph.assign(numOfVerts, std::vector<int>(numOfVerts, 0));
    flow.assign(numOfVerts, std::vector<int>(numOfVerts, 0));
    layers.assign(numOfVerts, -1);
    pointers.assign(numOfVerts, 0);
    istock = 0;
    stock = numOfVerts - 1;
}

void readGraph() {
    unsigned num;
    std::cin >> num;
    numOfVerts = 2 * num + 2;

    assignGlobal();

    int weight;
    money = 0;
    // read istock -> vertex
    for (int i = 1; i <= num; ++i) {
        std::cin >> weight;
        money += weight;
        graph[0][i] = weight;
    }

    // read vertex -> stock and make vertex->vertex
    for (int i = 1; i <= num; ++i) {
        std::cin >> weight;
        graph[num + i][2 * num + 1] = weight;

        for (int j = 1; j <= num; ++j) {
            graph[j][num + i] = 200;
        }
    }
}

bool fillLayers() {
    std::queue<int> queue;
    layers.assign(numOfVerts, -1);

    queue.push(istock);
    layers[istock] = 0;

    int vertex;
    while (!queue.empty()) {
        vertex = queue.front();
        queue.pop();
        for (int to = 0; to < numOfVerts; ++to) {
            if (flow[vertex][to] < graph[vertex][to] && layers[to] == -1) {
                queue.push(to);
                layers[to] = layers[vertex] + 1;
            }
        }
    }
    return layers[stock] != -1;
}

int findBlockingFlow(int startVertex, int minFlow) {
    if (minFlow == 0 || startVertex == stock) {
        return minFlow;
    }
    for (int &toVertex = pointers[startVertex]; toVertex < numOfVerts; ++toVertex) {
        if (layers[toVertex] != layers[startVertex] + 1) {
            continue;
        }
        int flowPortion = findBlockingFlow(
                toVertex,
                std::min(minFlow,
                         graph[startVertex][toVertex] - flow[startVertex][toVertex])
        );
        if (flowPortion != 0) {
            flow[toVertex][startVertex] -= flowPortion;
            flow[startVertex][toVertex] += flowPortion;
            return flowPortion;
        }
    }
    return 0;
}

int findMaxFlow() {
    int maFlow = 0;

    while (fillLayers()) {
        pointers.assign(numOfVerts, 0);
        int pushed = findBlockingFlow(istock, INT32_MAX);
        while (pushed != 0) {
            maFlow += pushed;
            pushed = findBlockingFlow(istock, INT32_MAX);
        }
    }
    return maFlow;
}

int main() {

    //freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);

    readGraph();
    int maxFlow = findMaxFlow();
    if (money == maxFlow) {
        std::cout << "YES";
    } else {
        std::cout << "NO";
    }
    std::cout << std::endl;
}

