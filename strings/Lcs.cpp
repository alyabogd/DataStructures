#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

int get_lcs_length(std::string &first, std::string &second) {
    if (first.length() < second.length()) {
        std::string temp = second;
        second = first;
        first = temp;
    }

    std::vector<std::vector<int> > data(2, std::vector<int>(second.length() + 1, 0));

    for (int i = 0; i < first.length(); ++i) {
        data[1][0] = 0;
        for (int j = 1; j <= second.length(); ++j) {
            data[0][j] = data[1][j];
            if (first[i] == second[j - 1]) {
                data[1][j] = data[0][j - 1] + 1;
            } else {
                data[1][j] = std::max(data[0][j], data[1][j - 1]);
            }
        }
    }
    return data[1][second.length()];
}

int main() {

    std::string first, second;
    std::cin >> first >> second;
    std::cout << get_lcs_length(first, second) << std::endl;
}

