#include <iostream>
#include <vector>
#include <string>

/**
 * Sorted array of suffixes of given string
 * Build time O(nlogn) - using Karp-Miller-Rozenberg's algorithm
 */
class Suffix_array {

    std::string text;

    // array[i] = it's lexicographic order
    std::vector<int> suffix_array;
    std::vector<int> marks;

    int max_mark;

    void build_suffix_array() {
        // sort by 1st letter
        initial_sort();

        // iteratively increase length
        for (int length = 1; length < text.size(); length *= 2) {
            suffix_array = sort_suffixes(length);
            marks = mark_suffixes(length);
        }
    }

    std::vector<int> mark_suffixes(int length) {
        std::vector<int> buffer(text.length(), 0);
        buffer[suffix_array[0]] = 0;
        for (int iterator = 1; iterator < text.length(); ++iterator) {
            int index = suffix_array[iterator];
            int prev_index = suffix_array[iterator - 1];

            buffer[index] = buffer[prev_index];
            if (marks[index] != marks[prev_index] ||
                marks[(index + length) % text.length()] !=
                marks[(prev_index + length) % text.length()]) {
                buffer[index]++;
            }
        }

        max_mark = buffer[suffix_array[suffix_array.size() - 1]];
        return buffer;
    }

    std::vector<int> sort_suffixes(int length) const {
        std::vector<int> buckets(max_mark + 1, 0);
        for (int i = 0; i < marks.size(); ++i) {
            buckets[marks[i]]++;
        }

        int sum = 0;
        for (int i = 0; i < buckets.size(); ++i) {
            sum += buckets[i];
            buckets[i] = sum - buckets[i];
        }

        std::vector<int> buffer(text.size(), 0);
        for (int i = 0; i < suffix_array.size(); ++i) {
            int pair_position = (suffix_array[i] - length + text.size()) % text.size();
            int new_position = buckets[marks[pair_position]];
            buckets[marks[pair_position]]++;
            buffer[new_position] = pair_position;
        }

        return buffer;
    }

    void initial_sort() {
        std::vector<int> buckets(max_mark + 1, 0);
        for (int index = 0; index < text.size(); ++index) {
            int key = text[index];
            buckets[key]++;
        }

        int sum = 0;
        for (int i = 0; i < buckets.size(); ++i) {
            sum += buckets[i];
            buckets[i] = sum - buckets[i];
        }

        // assign suffix array
        for (int index = 0; index < text.size(); ++index) {
            int key = text[index];
            int position = buckets[key];
            // because there might be another equal elements
            buckets[key]++;
            suffix_array[position] = index;
        }

        // assign marks.
        marks[suffix_array[0]] = 0;
        for (int i = 1; i < suffix_array.size(); ++i) {
            marks[suffix_array[i]] = marks[suffix_array[i - 1]];
            if (text[suffix_array[i]] != text[suffix_array[i - 1]]) {
                marks[suffix_array[i]]++;
            }
        }

        max_mark = marks[suffix_array[suffix_array.size() - 1]];
    }

public:
    explicit Suffix_array(const std::string &text) : text(text) {

        std::string end;
        end = static_cast<char> (0);

        this->text.append(end);

        suffix_array.assign(this->text.size(), 0);
        marks.assign(this->text.size(), 0);

        max_mark = 127 + 1;

        build_suffix_array();
    }

    void print() {
        for (int i = 1; i < suffix_array.size(); ++i) {
            std::cout << suffix_array[i] + 1 << " ";
        }
        std::cout << std::endl;
    }
};

int main() {

    std::cin.sync_with_stdio(0);
    std::cin.tie();

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    std::string line;
    std::getline(std::cin, line);
    Suffix_array suffix_array(line);
    suffix_array.print();
}
