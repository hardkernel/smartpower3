## arduino-cli

```
arduino-cli config init
cp arduino-cli.yaml ~/.arduino15/

arduino-cli compile --fqbn esp32:esp32:esp32 smartpower3
arduino-cli upload --port /dev/ttyUSB0 --fqbn esp32:esp32:esp32 smartpower3/
```
