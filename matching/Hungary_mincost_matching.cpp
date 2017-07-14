#include <iostream>
#include <vector>

class Graph {

    // 1-based.
    std::vector<std::vector<int> > table;
    unsigned tableSize;

    int find_free_col(std::vector<int> &row_potential,
                      std::vector<int> &col_potential,
                      std::vector<int> &mt,
                      std::vector<int> &way) {

        int free_col = 0;
        std::vector<int> min_by_row(tableSize, 150000);
        std::vector<bool> used(tableSize, false); // used in Kuhn

        do {
            // mark current row as visited
            used[free_col] = true;

            // find new minimum
            int delta = -1;
            int col_where_delta = -1;
            int matching_row = mt[free_col];
            for (int it = 1; it < tableSize; ++it) {
                if (!used[it]) {
                    int potential = table[matching_row][it] -
                            row_potential[matching_row] - col_potential[it];
                    if (potential < min_by_row[it]) {
                        min_by_row[it] = potential;
                        way[it] = free_col;
                    }
                    if (delta == -1 || min_by_row[it] < delta) {
                        delta = min_by_row[it];
                        col_where_delta = it;
                    }
                }
            }
            // update potentials for rows and cols
            for (int col = 0; col < tableSize; ++col) {
                if (!used[col]) {
                    min_by_row[col] -= delta;
                } else {
                    row_potential[mt[col]] += delta;
                    col_potential[col] -= delta;
                }
            }
            free_col = col_where_delta;
        } while (mt[free_col] != 0);

        return free_col;
    }

public:
    explicit Graph(const std::vector<std::vector<int> > table) {
        this->tableSize = table.size();
        this->table.assign(tableSize + 1, std::vector<int>(tableSize + 1, 0));
        for (int i = 0; i < tableSize; ++i) {
            for (int j = 0; j < tableSize; ++j) {
                this->table[i + 1][j + 1] = table[i][j];
            }
        }
        tableSize++;
    }

    void find_optimal(std::vector<int> &ans, int &cost) {
        std::vector<int> row_potential(tableSize, 0);
        std::vector<int> column_potential(tableSize, 0);
        std::vector<int> mt(tableSize, 0);
        std::vector<int> way(tableSize, 0);

        for (int row = 1; row < tableSize; ++row) {
            // current row is stored in mt[0]
            mt[0] = row;
            int free_col = find_free_col(row_potential, column_potential, mt, way);

            // we can increase chain
            int element;
            do {
                element = way[free_col];
                mt[free_col] = mt[element];
                free_col = element;
            } while (free_col != 0);
        }
        // end of algorithm
        ans.assign(tableSize, 0);
        for (int i = 1; i < tableSize; ++i) {
            ans[mt[i]] = i;
        }
        cost = -column_potential[0];
    }
};

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    unsigned n;
    std::cin >> n;

    std::vector<std::vector<int> > data(n, std::vector<int>(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cin >> data[i][j];
        }
    }

    Graph g(data);
    std::vector<int> ans;
    int cost;

    g.find_optimal(ans, cost);
    std::cout << cost << std::endl;
    for (int i = 1; i < ans.size(); ++i) {
        std::cout << ans[i] << " ";
    }
    std::cout << std::endl;
}
