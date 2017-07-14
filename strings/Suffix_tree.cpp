#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

struct Node {

    // describes edge, which leads to this node
    // starting from 'left_border' symbol, length symbols
    int left_border;
    int length;

    int suffix_link;

    // jumps from this node
    std::vector<int> go;

    Node(int left_border, int length) : left_border(left_border), length(length) {
        suffix_link = -1;
        go.assign(27, 0);
    }
};

class Suffix_tree {

    const int INF = 1000000000;

    std::string text;

    std::vector<Node> nodes;

    // store the longest suffix
    // node, form where the longest suffix starts
    int active_node;
    // how many chars we need to pass from 'node' to get the longest suffix
    int active_length;

    // current size of suffix tree
    int size;
    // char we are inserting on current step
    int current_char_index;

    // used for suffix links
    int last_added_vertex;

    // vertex counter while dfs traversing
    int counter_dfs;

    /**
     * Put new node into nodes array, return it's position
     */
    int make_node(int position, int length) {
        nodes.push_back(Node(position, length));
        return size++;
    };

    /**
     * Update active node and active length variables
     * 'walk down' edge while it's possible
     */
    void walk_down_edge() {
        char active_char = text[current_char_index - active_length];
        int active_char_int = active_char == '$' ? 0 : active_char - 'a' + 1;

        int next_node = nodes[active_node].go[active_char_int];

        while (active_length > nodes[next_node].length) {
            active_node = next_node;
            active_length -= nodes[active_node].length;

            active_char = text[current_char_index - active_length];
            active_char_int = active_char == '$' ? 0 : active_char - 'a' + 1;
            next_node = nodes[active_node].go[active_char_int];
        }
    }

    void add_letter(char letter) {
        int letter_int = letter == '$' ? 0 : letter - 'a' + 1;

        current_char_index++;
        active_length++;

        last_added_vertex = 0;

        while (active_length > 0) {
            walk_down_edge();
            int active_char = text[current_char_index - active_length];
            int active_char_int = active_char == '$' ? 0 : active_char - 'a' + 1;

            int &dest_vertex = nodes[active_node].go[active_char_int];
            int suffix_begin = text[nodes[dest_vertex].left_border + active_length - 1];
            int suffix_begin_int = suffix_begin == '$' ? 0 : suffix_begin - 'a' + 1;

            if (dest_vertex == 0) {
                // there is no edge from active node by current char
                // create new vertex and connect it
                int left_border = current_char_index - active_length;
                dest_vertex = make_node(left_border, INF);
                nodes[last_added_vertex].suffix_link = active_node;
                last_added_vertex = 0;
            } else {
                // there is an edge from active node by current char
                if (suffix_begin_int == letter_int) {
                    // current suffix is already stored in tree
                    nodes[last_added_vertex].suffix_link = active_node;
                    return;
                }

                // there is no suffix
                // create new vertex 'in the middle' of existing edge
                // connect old and new suffixes
                int new_suffix = make_node(nodes[dest_vertex].left_border, active_length - 1);
                nodes[new_suffix].go[letter_int] = make_node(current_char_index - 1, INF);
                nodes[new_suffix].go[suffix_begin_int] = dest_vertex;

                nodes[dest_vertex].length -= active_length - 1;
                nodes[dest_vertex].left_border += active_length - 1;

                dest_vertex = new_suffix;

                nodes[last_added_vertex].suffix_link = new_suffix;
                last_added_vertex = new_suffix;
            }

            if (active_node == 0) {
                active_length--;
            } else {
                active_node = nodes[active_node].suffix_link;
            }
        }
    }

    void init() {
        current_char_index = 0;
        size = 1;
        active_node = 0;
        active_length = 0;
        nodes.push_back(Node(0, 0));
        nodes[0].length = INF;
    }

    void print_dfs(int vertex, int parent) {
        int right_border = nodes[vertex].left_border + nodes[vertex].length;
        if (right_border > text.length()) {
            right_border = text.length();
        }
        printf("%d %d %d\n", parent, nodes[vertex].left_border, right_border);
        int current_dfs = ++counter_dfs;
        for (int to: nodes[vertex].go) {
            if (to != 0) {
                print_dfs(to, current_dfs);
            }
        }
    }

public:
    explicit Suffix_tree(const std::string &text) : text(text) {
        init();
        for (char letter : text) {
            add_letter(letter);
        }
    }

    void print() {
        counter_dfs = 0;
        printf("%d\n", size);
        counter_dfs = 0;
        for (int to: nodes[0].go) {
            if (to != 0) {
                print_dfs(to, 0);
            }
        }
    }
};

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    std::string line;
    std::cin >> line;
    Suffix_tree tree(line);
    tree.print();
}
