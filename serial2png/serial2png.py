import asyncio
from serial_asyncio import open_serial_connection
import struct
import aiofiles
import tempfile
import time
import os
import sys

from PIL import ImageFile
ImageFile.LOAD_TRUNCATED_IMAGES = True

from PIL import Image

class Header():
    def __init__(self):
        self.reserved = 0
        self.filesize = 54 + (320*480*3)
        self.fileoffset_to_pixelarray = 54
        self.dibheadersize = 40
        self.width = 480
        self.height = 320
        self.planes = 1
        self.bitsperpixel = 24
        self.compression = 0
        self.imagesize = 320*480*3
        self.ypixelpermeter = 2835
        self.xpixelpermeter = 2835
        self.numcolorspallette = 0
        self.mostimpcolor = 0

        self.header = struct.pack('6I2H6I', self.reserved, self.filesize, self.fileoffset_to_pixelarray,
                self.dibheadersize, self.width, self.height, self.planes, self.bitsperpixel,
                self.compression, self.imagesize, self.ypixelpermeter, self.xpixelpermeter,
                self.numcolorspallette, self.mostimpcolor)


class Serial2BMP():
    def __init__(self, node='/dev/ttyUSB0', baudrate=921600):
        self.serial = {'node':node, 'alive': -1, 'r': None, 'w': None,
                'baudrate': baudrate}
    async def init_serial(self):
        while (self.serial['alive'] != 1):
            if self.serial['node'] == None:
                return None
            try:
                self.serial['r'], self.serial['w'] = await open_serial_connection(
                        url=self.serial['node'], baudrate=self.serial['baudrate'])
            except SerialException as e:
                print(f"serial init error {self.serial['r']}, {self.serial['w']}")

            if (type(self.serial['r']) == asyncio.streams.StreamReader) and \
                    (type(self.serial['w']) == asyncio.streams.StreamWriter):
                        self.serial['alive'] = 1
                        print('Serial initialized');
            await asyncio.sleep(1)

    async def send(self, msg):
        if self.serial['alive'] != 1:
            await asyncio.sleep(1)
        else:
            self.serial['w'].write(str.encode(msg));

    async def recv(self):
        if self.serial['alive'] != 1:
            await asyncio.sleep(1)

async def main():
    temp = tempfile.TemporaryFile()
    if len(sys.argv) == 2:
        serial2bmp = Serial2BMP(sys.argv[1])
    else:
        serial2bmp = Serial2BMP()
    task = await serial2bmp.init_serial()
    hd = Header()
    while True:
        if (input("Press \"S\" for screen capture of SmartPower3 : \n\r") == "S"):
            test = []
            await serial2bmp.send('S')
            while True:
                tmp = await serial2bmp.serial['r'].read(1)
                tmp2 = await serial2bmp.serial['r'].read(1)
                tmp = int.from_bytes(tmp, byteorder='little')
                tmp2 = int.from_bytes(tmp2, byteorder='little')
                tmp = tmp << 8 | tmp2


                b = (tmp & 0x001F) << 3;
                b |= (b >> 5)
                test.append(b.to_bytes(1, 'big'))
                g = (tmp & 0x07E0) >> 3
                g |= (g >> 6)
                test.append(g.to_bytes(1, 'big'))
                r = (tmp & 0xF800) >> 8;
                r |= (r >> 5)
                test.append(r.to_bytes(1, 'big'))


                if len(test) == 460800:
                    test2 = []
                    tmp = []
                    test.reverse()
                    for idx, val in enumerate(test):
                        if idx%1440 == 0 and idx != 0:
                            tmp.reverse()
                            test2 += tmp
                            tmp = []
                        tmp.append(val)
                    file_name = time.strftime("%Y%m%d-%H%M%S")
                    with open(file_name + '.bmp', mode='wb+') as f:
                        f.write(b'B')
                        f.write(b'M')
                        f.write(hd.header)
                        f.writelines(test2)
                    if os.path.exists(file_name + '.bmp'):
                        img = Image.open(file_name + '.bmp')
                        img.save(file_name + '.png', 'png')
                    if os.path.exists(file_name + '.png'):
                        if os.path.exists(file_name + '.bmp'):
                            os.remove(file_name + '.bmp')
                    print(file_name)
                    break

        await asyncio.sleep(1)

if __name__ == "__main__":
    asyncio.run(main())
