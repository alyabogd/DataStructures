#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

struct Node {
    bool is_terminated;
    std::vector<int> children;
    int parent;
    char char_to_parent;
    int suffix_link;
    int compressed_suffix_link;
    std::vector<int> step;

    Node(int parent, char char_to_parent) :
            parent(parent), char_to_parent(char_to_parent) {
        is_terminated = false;
        suffix_link = -1;
        compressed_suffix_link = -1;
        children.assign(2, -1);
        step.assign(2, -1);
    }

    bool contains_key_children(char letter) {
        int int_letter = letter - '0';
        return children[int_letter] != -1;
    }

    bool contains_link(char letter) {
        int int_letter = letter - '0';
        return step[int_letter] != -1;
    }

    void add_child(char letter, int index) {
        children[letter - '0'] = index;
    }
};

class Trie {

    std::vector<Node> nodes;
    std::vector<bool> tupic;
    std::vector<bool> used_in_dfs;

public:
    Trie() {
        // keep root node
        nodes.assign(1, Node(0, 0));
    }

    void put_string(std::string &pattern) {
        int currentIndex = 0;
        for (int i = 0; i < pattern.length(); ++i) {
            char currentChar = pattern[i];
            if (nodes[currentIndex].contains_key_children(currentChar)) {
                currentIndex = nodes[currentIndex].children[currentChar - '0'];
            } else {
                nodes.push_back(Node(currentIndex, currentChar));
                nodes[currentIndex].add_child(currentChar, nodes.size() - 1);
                currentIndex = nodes.size() - 1;
            }
        }
        nodes[currentIndex].is_terminated = true;
    }

    int get_suffix_link(int from) {
        if (nodes[from].suffix_link != -1) {
            return nodes[from].suffix_link;
        }
        if (from == 0 || nodes[from].parent == 0) {
            nodes[from].suffix_link = 0;
            return 0;
        }
        nodes[from].suffix_link =
                next_step(get_suffix_link(nodes[from].parent), nodes[from].char_to_parent);
        return nodes[from].suffix_link;
    }

    int get_compressed_suffix_link(int from) {
        if (nodes[from].compressed_suffix_link != -1) {
            return nodes[from].compressed_suffix_link;
        }
        int suffix_link = get_suffix_link(from);
        if (nodes[suffix_link].is_terminated || suffix_link == 0) {
            nodes[from].compressed_suffix_link = suffix_link;
            return suffix_link;
        }
        nodes[from].compressed_suffix_link = get_compressed_suffix_link(suffix_link);
        return nodes[from].compressed_suffix_link;
    }


    int next_step(int from, char letter) {
        int int_letter = letter - '0';
        if (nodes[from].contains_link(letter)) {
            return nodes[from].step[int_letter];
        }

        if (nodes[from].contains_key_children(letter)) {
            int child_index = nodes[from].children[int_letter];
            nodes[from].step[int_letter] = child_index;
            return child_index;
        }

        if (from == 0) {
            nodes[from].step[int_letter] = 0;
            return 0;
        }

        nodes[from].step[int_letter] = next_step(get_suffix_link(from), letter);
        return nodes[from].step[int_letter];
    }

    // can't go in terminated vertexes
    // can't go in vertexes, which has terminated ones due suffix links
    bool helper(int from) {
        if (tupic[from]) {
            return false;
        }
        if (used_in_dfs[from]) {
            return true;
        }

        used_in_dfs[from] = true;

        int node_to_zero = next_step(from, '0');
        bool zero = false;

        if (!nodes[node_to_zero].is_terminated &&
            get_compressed_suffix_link(node_to_zero) == 0) {
            zero = helper(node_to_zero);
        }

        if (zero) {
            return true;
        }

        int node_to_one = next_step(from, '1');
        bool one = false;

        if (!nodes[node_to_one].is_terminated &&
            get_compressed_suffix_link(node_to_one) == 0) {
            one = helper(node_to_one);
        }

        if (one) {
            return true;
        }

        tupic[from] = true;
        return false;
    }

    bool endlessStringExists() {
        used_in_dfs.assign(nodes.size(), false);
        tupic.assign(nodes.size(), false);
        return helper(0);
    }
};

int main() {

    std::cin.sync_with_stdio(0);
    std::cin.tie();

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    Trie trie;
    int tests;
    std::cin >> tests;
    std::string number;
    for (int i = 0; i < tests; ++i) {
        std::cin >> number;
        trie.put_string(number);
    }

    if (trie.endlessStringExists()) {
        std::cout << "YES" << std::endl;
    } else {
        std::cout << "NO" << std::endl;
    }

    return 0;
}
