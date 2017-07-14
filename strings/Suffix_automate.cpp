#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>

struct State {
    int max_length;
    int suffix_link;
    std::vector<int> go;

    bool is_terminal;

    State() {
        max_length = 0;
        suffix_link = -1;
        go.assign(26, -1);
        is_terminal = false;
    }

    bool contains_jump(char letter) {
        return go[letter - 'a'] != -1;
    }

    bool contains_jump(int letter) {
        return go[letter] != -1;
    }

    void add_jump(char letter, int index) {
        go[letter - 'a'] = index;
    }

    void add_jump(int letter, int index) {
        go[letter] = index;
    }
};

class Suffix_automata {
    std::vector<State> states;
    int last;
    int size;

    int states_counter;
    int jumps_counter;

    std::vector<int> terminals;

    // adds new state to states array
    // returns it's position
    int add_state() {
        states.push_back(State());
        states_counter++;
        size++;
        return size - 1;
    }

    /**
     * Pushes full copy of old state and returns it's position
     */
    int add_copy(int old_state) {
        int new_state = add_state();
        states[new_state].max_length = states[old_state].max_length;
        states[new_state].suffix_link = states[old_state].suffix_link;
        states[new_state].go = std::vector<int>(states[old_state].go);
        for (int index : states[new_state].go) {
            if (index != -1) {
                jumps_counter++;
            }
        }
        return new_state;
    }

    // add first 'empty' state
    void init() {
        size = 0;
        last = 0;

        add_state();
        states[0].max_length = 0;
        states[0].suffix_link = -1;
    }

    /**
     * Method adds next letter to automate and update all necessary states
     */
    void add_letter(char letter) {
        int cur = add_state();
        states[cur].max_length = states[last].max_length + 1;

        int fail_index = check_suffix_links(letter, cur);
        if (fail_index == -1) {
            states[cur].suffix_link = 0;
            last = cur;
            return;
        }

        int old_state = states[fail_index].go[letter - 'a'];
        if (states[old_state].max_length == states[fail_index].max_length + 1) {
            states[cur].suffix_link = old_state;
        } else {
            int clone = add_copy(old_state);
            states[clone].max_length = states[fail_index].max_length + 1;

            states[old_state].suffix_link = clone;
            states[cur].suffix_link = clone;

            update_jumps(letter, fail_index, old_state, clone);
        }

        last = cur;
    }

    /**
     * Change jump from old_state to clone while necessary
     */
    void update_jumps(char letter, int fail_index, int old_state, int clone) {
        int index = fail_index;
        while (index >= 0 && states[index].go[letter - 'a'] == old_state) {
            states[index].go[letter - 'a'] = clone;
            index = states[index].suffix_link;
        }
    }

    /**
     * Checks, if there is a jump from state 'cur' by letter 'letter'
     * If there is no jump, method adds one to the last state and moves by suffix link
     * Otherwise, returns index of state, where there is already a jump
     */
    int check_suffix_links(char letter, int cur) {
        int fail_index = last;
        while (fail_index >= 0 && !states[fail_index].contains_jump(letter)) {
            states[fail_index].add_jump(letter, cur);
            jumps_counter++;
            fail_index = states[fail_index].suffix_link;
        }
        return fail_index;
    }

    void assign_terminal_states() {
        terminals.clear();
        int index = last;
        while (index != -1) {
            states[index].is_terminal = true;
            terminals.push_back(index);
            index = states[index].suffix_link;
        }
    }

public:
    explicit Suffix_automata(std::string &text) {
        states_counter = 0;
        jumps_counter = 0;

        last = -1;
        size = -1;
        states.clear();

        init();
        for (char letter : text) {
            add_letter(letter);
        }

        assign_terminal_states();
    }

    void print() {
        printf("%d %d\n", states_counter, jumps_counter);

        for (int from = 0; from < states.size(); ++from) {
            for (int letter = 0; letter < states[from].go.size(); ++letter) {
                if (states[from].contains_jump(letter)) {
                    printf("%d %d %c\n", from + 1,
                           states[from].go[letter] + 1,
                           static_cast<char>(letter + 'a'));
                }
            }
        }

        printf("%d\n", terminals.size());

        for (int index: terminals) {
            printf("%d ", index + 1);
        }
        printf("\n");
    }
};

int main() {

    std::cin.sync_with_stdio(0);
    std::cin.tie();

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    std::string text;
    std::cin >> text;

    Suffix_automata automata(text);

    automata.print();
}
