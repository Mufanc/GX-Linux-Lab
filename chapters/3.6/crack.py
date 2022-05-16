import httpx
from random import randbytes


def bytes2string(data):
    return ''.join([f'{ch:0>2X}' for ch in data])


def encode(data):
    return bytes([ord(ch) - ord('0') for ch in data])


def main():
    serial = randbytes(4)
    print(f'序列号：{bytes2string(serial)}')

    student = input('学号：')
    data = encode(student[-3:])

    passwd = [0] * 4
    passwd[0] = serial[1] ^ data[0] ^ data[2]
    passwd[1] = serial[3] ^ data[1] ^ data[2]
    passwd[2] = serial[0] ^ data[0] ^ data[1]
    passwd[3] = serial[2]

    passwd = bytes2string(passwd)
    print(f'密码：{passwd}')

    resp = httpx.get(
        f'http://132.232.98.70:6363/check485',
        params={
            'id': student,
            'v': bytes2string(serial),
            's': passwd
        }
    )
    print(f'已提交至服务器，返回信息：{resp.text}')


if __name__ == '__main__':
    main()
