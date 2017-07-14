#include <iostream>
#include <vector>
#include <map>
#include <string>

struct Node {
    int number_of_terminals_below;
    bool is_terminated;
    std::map<char, int> children;

    Node() {
        is_terminated = false;
        number_of_terminals_below = 0;
    }

    bool contains_key(char letter) {
        return children.find(letter) != children.end();
    }

    void add_child(char letter, int index) {
        children[letter] = index;
    }
};

class Trie {
    std::vector<Node> data;

public:
    Trie() {
        data.assign(0, Node());
        // root node
        data.push_back(Node());
        data[0].number_of_terminals_below = 0;
    }

    void put_string(std::string &line) {
        int arrayIndex = 0;
        for (int i = 0; i < line.length(); ++i) {
            data[arrayIndex].number_of_terminals_below++;
            if (data[arrayIndex].contains_key(line[i])) {
                arrayIndex = data[arrayIndex].children[line[i]];
            } else {
                data.push_back(Node());
                data[arrayIndex].add_child(line[i], data.size() - 1);
                arrayIndex = data.size() - 1;
            }
        }
        // count itself as a terminated node too
        data[arrayIndex].number_of_terminals_below++;
        data[arrayIndex].is_terminated = true;
    }

    std::string get_string(int lexic_index) {
        std::string word = "";
        int lexic_index_copy = lexic_index;
        int arrayIndex = 0;
        while (lexic_index_copy != 0) {
            if (data[arrayIndex].is_terminated) {
                lexic_index_copy--;
            }
            if (lexic_index_copy == 0) {
                break;
            }
            std::map<char, int>::iterator it = data[arrayIndex].children.begin();
            while (it != data[arrayIndex].children.end() &&
                   data[(*it).second].number_of_terminals_below < lexic_index_copy) {
                // skip this child and reduce index
                lexic_index_copy -= data[(*it).second].number_of_terminals_below;
                ++it;
            }
            if (it == data[arrayIndex].children.end()) {
                return "";
            }
            word += (*it).first;
            arrayIndex = (*it).second;
        }
        return word;
    }
};

int main() {

    std::cin.sync_with_stdio(0);
    std::cin.tie();

    //freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);

    int requests;
    std::cin >> requests;
    std::string command;
    Trie trie;
    for (int i = 0; i < requests; ++i) {
        std::cin >> command;
        if (isdigit(command[0])) {
            std::cout << trie.get_string(std::stoi(command)) << std::endl;
        } else {
            trie.put_string(command);
        }
    }
}
