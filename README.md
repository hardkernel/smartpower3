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

## Notes on building the firmware using Platformio

### Build environment, libraries and their versions
#### Platform
Platform build environment pulls in some more general libraries, for example bus and filesystem libraries (Wire/I2C,
SPI, SPIFFS etc.). As these may change between (major) versions, it is advisable to fix platform version
in ```platformio.ini``` file.

That is not to say that you shouldn't use the latest platform version - on the contrary. But 
it gives you a known working starting point - if you can compile the original firmware code without any changes 
to the codebase and version settings, flash it into the device and it works as expected, than any problem is **likely**
related to any update or changes you may have made. Should you make a change and things went wrong, then you have
a good idea what change exactly caused the problem and work from there.

#### Libraries
Some of the more specific hardware libraries required to build the project can be included into the firmware build 
in two ways. The original way of using ```git submodule``` (using source and version fixed in git) or by linking 
them via ```platformio.ini```.

If you remove ```lib_deps``` information from ```platformio.ini```, the submodules at their checked out versions will be used. 
Or you can use library manager in Platformio (for VSCode or other with similar functionality), download the same library 
at different version and then see which one works best for your use-case. There are more options - see documentation 
on setting up your project and configuration options (https://docs.platformio.org/en/latest/projectconf/index.html) and
notes in ```platformio.ini``` provided with the project.

Again, as is the case with platform settings, having a known version working configuration and updating from there can
help pinpoint any problem you may encounter.
