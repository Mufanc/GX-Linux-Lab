import httpx
import serial.tools.list_ports
from serial import Serial

student = input('学号：')


def bytes2string(_data):
    return ''.join([f'{ch:0>2x}' for ch in _data]).upper()


data = b'\xaa\x55' + bytes([ord(ch) - ord('0') for ch in student])
pipe = Serial(list(serial.tools.list_ports.comports()[0])[0], 1200)

serial, passwd = bytes2string(pipe.read(13)), None
print(f'序列号：{serial}')

pipe.write(data)
for i in range(260):
    pipe.read(2)                            # 读掉 AA 55
    distance = list(pipe.read(1))[0]        # 读取起始字节位置
    pipe.read(distance - 4)                 # 读掉多余的字节
    passwd = pipe.read(4)                   # 读取 4 字节的「密码」
    data = bytes2string(passwd)             # 将「密码」格式化为易读的格式
    print(f'[{i:>3d}]: {data}', end='\r')
    pipe.write(b'\xaa\x55' + passwd)        # 将「密码」发送回单片机

print('\n')
resp = httpx.get(f'http://132.232.98.70:6363/checkSecret?id={student}&s={data}&v={serial}')
print(f'已提交至服务器，有效成绩：{resp.text}')
