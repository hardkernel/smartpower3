#include <ArduinoTrace.h>




int value = 42;

void setup() {
pinMode(14, OUTPUT);
pinMode(27, OUTPUT);
digitalWrite(14, HIGH);
digitalWrite(27, HIGH);
sleep(5);
ARDUINOTRACE_INIT(115200);
TRACE();
}

void loop() {
TRACE();
	DUMP(42);
	sleep(1);
}
