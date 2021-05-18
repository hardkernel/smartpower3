# Build a smartpower3 with arduino-cli
## Linux
### Install arduino-cli
```
sudo apt install curl git
curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
```

### Get a smartpower3 source code including submodules
```
git clone --recursive https://github.com/leeseungcheol/smartpower3
cd smartpower3
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
