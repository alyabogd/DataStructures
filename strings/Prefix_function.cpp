#include <iostream>
#include <vector>
#include <string>

int main() {

    int test;
    std::cin >> test;
    std::string s;
    for (int j = 0; j < test; ++j) {
        std::cin >> s;
        unsigned len = s.length();
        std::vector<int> pi(len, 0);
        int max_border = 0;
        for (int i = 1; i < len; ++i) {
            int k = pi[i - 1];
            while (k > 0 && s[i] != s[k]) {
                k = pi[k - 1];
            }
            if (s[i] == s[k]) {
                k++;
            }
            pi[i] = k;
            if (k > max_border) {
                max_border = k;
            }
        }
        int p = len - max_border;
        if (len % p == 0) {
            std::cout << len - max_border << "\n" << std::endl;
        } else {
            std::cout << len << "\n" << std::endl;
        }
    }


}

