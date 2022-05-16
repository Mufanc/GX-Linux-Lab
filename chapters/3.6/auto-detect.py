import httpx
from serial.tools import list_ports
from serial import Serial


def bytes2string(data):
    return ''.join([f'{ch:0>2X}' for ch in data])


def detect_baud_rate():
    def internal():
        device = list(list_ports.comports()[0])[0]
        for baud in [1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200]:
            print(f'Trying [{baud:>6d}]...', end='\r')
            serial = Serial(device, baud, timeout=1)
            data = serial.read(6)
            if data[:2] == b'\xaa\x55':
                return data[2:], baud
            serial.close()
        return None, None

    while True:
        serial, baud = internal()
        if serial:
            print('')
            return serial, baud
        print('请调节 B 板波特率，然后再试一次！', end='\r')
        input()


def main():
    serial, baud = detect_baud_rate()
    print(f'序列号: {bytes2string(serial)}, 波特率：{baud}')

    student = input('学号：')
    data = b'\xaa\x55' + serial + bytes([ord(ch) - ord('0') for ch in student])

    pipe = Serial(list(list_ports.comports()[0])[0], baud)
    pipe.write(data)

    passwd = bytes2string(pipe.read(6)[2:])

    print(f'密码：{passwd}')

    resp = httpx.get(f'http://132.232.98.70:6363/check485?id={student}&v={bytes2string(serial)}&s={passwd}')
    print(f'已提交至服务器，返回信息：{resp.text}')


if __name__ == '__main__':
    main()
