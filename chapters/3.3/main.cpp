#include <cstdio>
#include <string>

#include "serial.h"

std::string format(const std::vector<uchar> &data) {
    std::string str(2 * data.size() + 1, '\x00');
    for (int i = 0; i < data.size(); i++) {
        sprintf(&str[i * 2], "%02X", data[i]);
    }
    return str;
}

int main() {
    serial pipe("/dev/ttyUSB0", B4800);

    std::vector<uchar> response = pipe.read(13);
    for (auto ch : response) {
        printf("%02X", ch);
    }

    return 0;
}
