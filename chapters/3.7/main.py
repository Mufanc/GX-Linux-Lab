import httpx
import serial.tools.list_ports
from serial import Serial
from time import sleep

prefix, student = b'\xAA\x55', input('学号：')


def bytes2string(_data):
    return ''.join([f'{ch:0>2X}' for ch in _data])


pipe = Serial(list(serial.tools.list_ports.comports()[0])[0], 1200)  # 打开串口

data, passwd = pipe.read(6), None  # 读「序列号」
assert data.startswith(prefix)

serial = bytes2string(data[2:])
print(f'序列号：{serial}')