#include <cstdio>
#include <iostream>
#include <string>
#include <thread>

#include "serial.h"

std::string format(const std::vector<uchar> &data) {
    std::string str(2 * data.size() + 1, '\x00');
    for (int i = 0; i < data.size(); i++) {
        sprintf(&str[i * 2], "%02X", data[i]);
    }
    return str;
}

int main() {
    printf("> ");
    std::string student; std::cin >> student;
    const std::vector<uchar> prefix = { 0xAA, 0x55 };

    serial pipe("/dev/ttyUSB0", B1200);

    printf("%s\n", format(pipe.read(6)).substr(4).c_str());
    fflush(stdout);

    std::vector<uchar> data = prefix;
    for (char ch : student) {
        data.push_back(ch - '0');
    }
    pipe.write(data);

    std::vector<uchar> buffer;
    for (int count = 0; count < 270; ) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        data = pipe.read_all();

        if (!data.empty()) {
            buffer.insert(buffer.end(), data.begin(), data.end());
            continue;
        }

        if (buffer.size() >= 6 && buffer[0] == prefix[0] && buffer[1] == prefix[1]) {
            std::vector<uchar> passwd(&buffer[buffer.size() - 4], &buffer[buffer.size()]);

            count++;
            printf("[%3d]: %s\r", count, format(passwd).c_str());

            passwd.insert(passwd.begin(), prefix.begin(), prefix.end());
            pipe.write(passwd);
        }

        buffer.clear();
        fflush(stdout);
    }
    printf("\n");

    return 0;
}
