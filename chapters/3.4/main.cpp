#include <cstdio>
#include <iostream>
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
    printf("> ");
    std::string student; std::cin >> student;
    const std::vector<uchar> prefix = { 0xAA, 0x55 };

    serial pipe("/dev/ttyUSB0", B1200);

    printf("%s\n", format(pipe.read(13)).c_str());

    std::vector<uchar> data = prefix;
    for (char ch : student) {
        data.push_back(ch - '0');
    }
    pipe.write(data);

    for (int i = 0; i < 260; i++) {
        pipe.read(2);                                    // 读掉 AA 55
        int position = pipe.read(1)[0];                  // 读取起始字节位置
        pipe.read(position - 4);                         // 读掉多余的字节
        data = pipe.read(4);                             // 读取 4 字节的「密码」

        std::string passwd = format(data);               // 将「密码」格式化为易读的格式
        printf("[%3d]: %s\r", i, passwd.c_str());
        fflush(stdout);                                  // 刷新输出缓冲区

        // 将 AA 55 插入到收到的「密码」之前
        data.insert(data.begin(), prefix.begin(), prefix.end());
        pipe.write(data);                                // 将「密码」发送回单片机
    }
    printf("\n");

    return 0;
}
