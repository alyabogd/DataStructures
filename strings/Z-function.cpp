#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct Compressed_decimal {
    int preperiod_length;
    int period_length;

    Compressed_decimal(int preperiod_length, int period_length) :
            preperiod_length(preperiod_length),
            period_length(period_length) {}
};

class Decimal_processor {
    std::vector<int> z_func;
    std::string data;

    void assign_z_function() {
        unsigned len = data.length();
        z_func.assign(len, 0);

        int left = 0, right = 0;
        for (int index = 1; index < len; ++index) {
            if (index <= right) {
                // inside processed z-block
                int symbols_till_end = right - index + 1;
                z_func[index] = std::min(z_func[index - left], symbols_till_end);
            }
            while (index + z_func[index] < len) {
                if (data[z_func[index]] == data[index + z_func[index]]) {
                    z_func[index]++;
                } else {
                    break;
                }
            }
            if (index + z_func[index] - 1 > right) {
                left = index;
                right = z_func[index] + index - 1;
            }
        }
    }

public:
    explicit Decimal_processor(const std::string &data) : data(data) {
        // remove first character and dot at the beginning
        this->data = data.substr(2);
        std::reverse(this->data.begin(), this->data.end());
        assign_z_function();
    }

    Compressed_decimal get_compressed() {
        int max_z = -1;
        int index = 0;
        int len = z_func.size();

        for (int i = 1; i < len; ++i) {
            if (z_func[i] > max_z) {
                index = i;
                max_z = z_func[i];
            }
        }

        if (max_z == -1) {
            index = 1;
            max_z = 0;
        }
        int periodic_part = max_z + index;
        int preperiod = len - periodic_part;
        int period = index;
        return Compressed_decimal(preperiod, period);
    }
};

std::string compress(const std::string &data) {
    Decimal_processor processor(data);
    Compressed_decimal compressed = processor.get_compressed();
    std::string integer_part = data.substr(0, 2);
    std::string preperiod = data.substr(2, compressed.preperiod_length);
    std::string period = data.substr(
            2 + compressed.preperiod_length,
            compressed.period_length);
    return integer_part + preperiod + "(" + period + ")";
}

void read_input(std::istream &in, std::string &number) {
    in >> number;
}

void write_output(std::ostream &out, const std::string &number) {
    out << number;
}

int main() {

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    std::string number;
    read_input(std::cin, number);

    const std::string compressed_number = compress(number);
    write_output(std::cout, compressed_number);
}
