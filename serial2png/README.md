# How to capture screen of SmartPower3 via serial port

## Ubuntu

### Update TFT_eSPI library to enable TFT_SDA_READ
```
git submodule init
git submodule update
```

### Define SCREEN_CAPTURE in platform.ini
```
...
build_flags =   -DSCREEN_CAPTURE
...
```

### Upload firmware
```
pio run upload
```

### Set SmartPower3's baud rate to 921600 using its dial.


### Install python3 package
```
sudo apt install python3-pip
python3 -m pip install pyserial_asyncio
```

### Run serial2png.py and input 'S' character with enter
```
odroid@odroid:~/work/smartpower3/serial2png$ python3 serial2png.py /dev/ttyUSB0
Serial initialized
Press "S" for screen capture of SmartPower3 : 
S
20220214-151205
```
