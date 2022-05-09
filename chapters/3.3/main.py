import serial.tools.list_ports
from serial import Serial

device = list(serial.tools.list_ports.comports()[0])[0]
for baud in [1200, 1800, 2400, 4800, 9600, 19200, 38400, 57600, 115200]:
    print(f'Trying [{baud:>6d}]: ')
    serial = Serial(device, baud, timeout=2)
    data = serial.read(13)
    if data[:2] == b'\xaa\x55':
        print(''.join(map(lambda ch: f'{ch:0>2X}', data[2:])))
        break
    else:
        print('failed.')
    serial.close()
