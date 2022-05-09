#ifndef SERIAL_H
#define SERIAL_H

#include <cstring>
#include <vector>
#include <sys/termios.h>

typedef unsigned char uchar;

class serial {
private:
    int device = -1, epfd = -1;
public:
    serial(const char *device_path, speed_t baud_rate);
    ~serial();
    std::vector<uchar> read(size_t n) const;
    void write(const std::vector<uchar> &data) const;
};

#endif //SERIAL_H
