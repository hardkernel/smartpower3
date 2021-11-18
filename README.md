# Build a smartpower3

## Linux(platformio)
### Install platformio
(https://docs.platformio.org/en/latest//core/installation.html)

### Get a smartpower3 source code including submodules
```
git clone --recursive https://github.com/hardkernel/smartpower3
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
pio run --target uploadfs
```
