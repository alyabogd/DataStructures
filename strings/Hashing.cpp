#include <iostream>
#include <cstdio>
#include <vector>
#include <string>

static const int BASE_MODULUS_FIRST = 200000001;
static const int BASE_MODULUS_SECOND = 214867047;

struct Number_safe {
    int mod_f = BASE_MODULUS_FIRST;
    int mod_s = BASE_MODULUS_SECOND;

    long long value_f;
    long long value_s;

    Number_safe() {
        mod_f = BASE_MODULUS_FIRST;
        mod_s = BASE_MODULUS_SECOND;
        value_f = 0;
        value_s = 0;
    }

    explicit Number_safe(long long value) : value_f(value), value_s(value) {
        mod_f = BASE_MODULUS_FIRST;
        mod_s = BASE_MODULUS_SECOND;
    }

    Number_safe(long long value_f, long long value_s) :
            value_f(value_f), value_s(value_s) {
        mod_f = BASE_MODULUS_FIRST;
        mod_s = BASE_MODULUS_SECOND;
    }

    bool operator==(const Number_safe &other) {
        return value_s == other.value_s && value_f == other.value_f;
    }

    Number_safe operator+(const Number_safe &other) {
        return Number_safe(
                (value_f + other.value_f) % mod_f,
                (value_s + other.value_s) % mod_s
        );
    }

    Number_safe operator-(const Number_safe &other) {
        return Number_safe(
                (value_f + mod_f - other.value_f) % mod_f,
                (value_s + mod_s - other.value_s) % mod_s
        );
    }

    Number_safe operator*(int mult) {
        return Number_safe(
                (value_f * mult) % mod_f,
                (value_s * mult) % mod_s
        );
    }

    Number_safe operator*(const Number_safe &other) {
        return Number_safe(
                (value_f * other.value_f) % mod_f,
                (value_s * other.value_s) % mod_s
        );
    }
};

class Hashed_string {
    int mult = 31;

    std::vector<Number_safe> powers;
    std::vector<Number_safe> prefix_hashes;

    Number_safe get_hash(int from, int to) {
        return prefix_hashes[to] - prefix_hashes[from - 1];
    }

public:
    // 1-based indexing
    explicit Hashed_string(std::string text) {
        unsigned len = text.length();
        powers.assign(len + 1, Number_safe(0));
        prefix_hashes.assign(len + 1, Number_safe(0));

        powers[0] = Number_safe(1);

        for (int i = 0; i < len; ++i) {
            powers[i + 1] = powers[i] * mult;
            int own_value = text[i] - 'a' + 1;
            prefix_hashes[i + 1] = prefix_hashes[i] + powers[i + 1] * own_value;
        }
    }

    bool is_equal(int from_f, int to_f, int from_s, int to_s) {
        Number_safe hash_f = get_hash(from_f, to_f);
        Number_safe hash_s = get_hash(from_s, to_s);

        if (from_f < from_s) {
            hash_f = hash_f * powers[from_s - from_f];
        }
        if (from_s < from_f) {
            hash_s = hash_s * powers[from_f - from_s];
        }

        return hash_f == hash_s;
    }
};

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    std::string text;
    std::cin >> text;
    Hashed_string hashed_string(text);

    int tests;
    int from_f, to_f, from_s, to_s;
    std::cin >> tests;
    for (int i = 0; i < tests; ++i) {
        scanf("%d %d %d %d", &from_f, &to_f, &from_s, &to_s);

        if (hashed_string.is_equal(from_f, to_f, from_s, to_s)) {
            printf("Yes\n");
        } else {
            printf("No\n");
        }
    }
}
