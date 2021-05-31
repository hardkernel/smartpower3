# Build a smartpower3
## Linux(arduino-cli)
### Install arduino-cli
```
sudo apt install curl git
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
echo "export PATH=\"\$HOME/bin:\$PATH\"" >> ~/.bashrc
```

### Get a smartpower3 source code including submodules
```
git clone --recursive https://github.com/leeseungcheol/smartpower3
```

### Check again the submodule repository
```
cd smartpower3
git submodule init
git submodule update
```

### Configure arduino-cli
```
arduino-cli config init
arduino-cli config set board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
arduino-cli core update-index
arduino-cli config set directories.user $PWD
arduino-cli core install esp32:esp32
```

### Compile & Upload
```
arduino-cli compile --fqbn esp32:esp32:esp32 smartpower3
arduino-cli upload --port /dev/ttyUSB0 --fqbn esp32:esp32:esp32 smartpower3/
```

## Linux(platformio)
### Install platformio
(https://docs.platformio.org/en/latest//core/installation.html)

### Get a smartpower3 source code including submodules
```
git clone --recursive https://github.com/leeseungcheol/smartpower3
```

### Check again the submodule repository
```
cd smartpower3
git submodule init
git submodule update
```

### Compile & Upload
```
pio run --target upload
```

### Serial monitor
```
pio device monitor -b115200
```
