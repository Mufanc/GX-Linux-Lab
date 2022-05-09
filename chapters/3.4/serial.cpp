#include "serial.h"

#include <cerrno>
#include <cstdio>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <sys/termios.h>
#include <sys/unistd.h>

#define err_check(code) if ((code) < 0) {    \
    printf("Error: %s\n", strerror(errno));  \
    _exit(1);                                \
}


serial::serial(const char *device_path, speed_t baud_rate) {
    /**
     * O_RDWR 表示以读写模式 (read & write) 打开文件
     * 参考：https://man7.org/linux/man-pages/man3/open.3p.html
     */
    err_check(device = open(device_path, O_RDWR | O_NOCTTY))

    termios attrs {};
    tcgetattr(device, &attrs);


    /**
     * 设置波特率
     * 参考：
     * - https://man7.org/linux/man-pages/man3/cfsetispeed.3p.html
     * - https://man7.org/linux/man-pages/man3/cfsetospeed.3p.html
     */
    err_check(cfsetispeed(&attrs, baud_rate))
    err_check(cfsetospeed(&attrs, baud_rate))

    /**
     * （懒得翻译了，将就看吧）
     * Input flags - Turn off input processing
     *
     * convert break to null byte, no CR to NL translation,
     * no NL to CR translation, don't mark parity errors or breaks
     * no input parity check, don't strip high bit off,
     * no XON/XOFF software flow control
     */
    attrs.c_iflag &= ~( BRKINT | ICRNL | INPCK | ISTRIP | IXON | IXOFF );

    /**
     * Output flags - Turn off output processing
     *
     * no CR to NL translation, no NL to CR-NL translation,
     * no NL to CR translation, no column 0 CR suppression,
     * no Ctrl-D suppression, no fill characters, no case mapping,
     * no local output
     */
    attrs.c_oflag &= ~( OPOST | ONLCR | OCRNL );

    /**
     * No line processing
     * echo off, echo newline off, canonical mode off,
     * extended input processing off, signal chars off
     */
    attrs.c_lflag &= ~( ECHO | ICANON | IEXTEN | ISIG );

    /**
     * Turn off character processing

     * clear current char size mask, no parity checking,
     * no output processing, force 8 bit input
     */
    attrs.c_cflag &= ~( CSIZE | PARENB );
    attrs.c_cflag |= CS8;

    /**
     * One input byte is enough to return from read()
     * Inter-character timer off
     */
    attrs.c_cc[VMIN]  = 1;
    attrs.c_cc[VTIME] = 0;

    /**
     * 设置终端参数，所有改动立即生效
     * 参考：https://man7.org/linux/man-pages/man3/tcsetattr.3p.html
     */
    err_check(tcsetattr(device, TCSANOW, &attrs))

    /**
     * 重新打开设备文件以应用新的终端参数
     */
    close(device);
    err_check(device = open(device_path, O_RDWR | O_NOCTTY))

    /**
     * 创建一个新的 epoll 实例，并返回一个用于控制的文件描述符
     * 参考：https://man7.org/linux/man-pages/man7/epoll.7.html
     */
    err_check(epfd = epoll_create(1))

    epoll_event event {
        .events = EPOLLIN | EPOLLET,  // 当对端变为可读时触发事件
        .data = {
            .fd = device
        }
    };

    /**
     * 将这个事件添加到 epoll 的监听列表中
     * 参考：https://man7.org/linux/man-pages/man2/epoll_ctl.2.html
     */
    err_check(epoll_ctl(epfd, EPOLL_CTL_ADD, device, &event))
}

serial::~serial() {
    (~device) && close(device);
    (~epfd) && close(epfd);
}

std::vector<uchar> serial::read(size_t n) const {
    size_t count = 0;
    std::vector<uchar> buffer(n);
    while (count < n) {
        epoll_event event {};
        /**
         * 等待串口对端发来数据
         * 参考：https://man7.org/linux/man-pages/man2/epoll_wait.2.html
         */
        epoll_wait(epfd, &event, 1, -1);

        /**
         * 读取数据，然后根据读取到的数据数量决定是否需要继续读取
         * 参考：https://man7.org/linux/man-pages/man3/read.3p.html
         */
        count += ::read(device, &buffer[count], n - count);
    }
    return buffer;
}

void serial::write(const std::vector<uchar> &data) const {
    size_t count = 0;
    while (count < data.size()) {
        /**
         * 向串口写入数据
         * 参考：https://man7.org/linux/man-pages/man3/write.3p.html
         */
        count += ::write(device, &data[count], data.size() - count);
    }
}
