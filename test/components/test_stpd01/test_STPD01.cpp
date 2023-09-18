#include <test_STPD01.h>
#include <unity.h>


TwoWire I2CB = TwoWire(1);
Test_STPD01 *test_stpd01 = new Test_STPD01(0x5, &I2CB);


Test_STPD01::Test_STPD01(uint8_t addr, TwoWire *theWire) : STPD01(addr, theWire)
{
	onoff = 1;
	_i2caddr = addr;
	_wire = theWire;
}

Test_STPD01::~Test_STPD01()
{
}

uint8_t Test_STPD01::read8(uint8_t reg)
{
	byte error;  // Arduino.h defines byte as uint8_t
	error = reg; // Fake the responce //_wire->endTransmission();
	if (error == -1)
		return -1;
	delay(5);
	return static_cast<int>(reg); // read response is int
}

void Test_STPD01::write8(uint8_t reg, uint8_t val)
{
	delay(5);
}

void setUp(void) {
}

void tearDown(void) {
}

typedef struct {
	uint8_t register_value;
	float volt_float;
} testing_sample;

// this data comes from SRPD01 manual
testing_sample testing_data[248] = {
{0x00, 3}, {0x30, 3.96}, {0x66, 5.04}, {0x9C, 7}, {0xD2, 13.8}, {0x01, 3.02}, {0x31, 3.98}, {0x67, 5.06}, {0x9D, 7.1},
{0xD3, 14}, {0x02, 3.04}, {0x32, 4}, {0x68, 5.08}, {0x9E, 7.2}, {0xD4, 14.2}, {0x03, 3.06}, {0x33, 4.02}, {0x69, 5.1},
{0x9F, 7.3}, {0xD5, 14.4}, {0x04, 3.08}, {0x34, 4.04}, {0x6A, 5.12}, {0xA0, 7.4}, {0xD6, 14.6}, {0x05, 3.1},
{0x35, 4.06}, {0x6B, 5.14}, {0xA1, 7.5}, {0xD7, 14.8}, {0x06, 3.12}, {0x36, 4.08}, {0x6C, 5.16}, {0xA2, 7.6},
{0xD8, 15}, {0x07, 3.14}, {0x37, 4.1}, {0x6D, 5.18}, {0xA3, 7.7}, {0xD9, 15.2}, {0x08, 3.16}, {0x38, 4.12}, {0x6E, 5.2},
{0xA4, 7.8}, {0xDA, 15.4}, {0x09, 3.18}, {0x39, 4.14}, {0x6F, 5.22}, {0xA5, 7.9}, {0xDB, 15.6}, {0x0A, 3.2},
{0x3A, 4.16}, {0x70, 5.24}, {0xA6, 8}, {0xDC, 15.8}, {0x0B, 3.22}, {0x3B, 4.18}, {0x71, 5.26}, {0xA7, 8.1}, {0xDD, 16},
{0x0C, 3.24}, {0x3C, 4.2}, {0x72, 5.28}, {0xA8, 8.2}, {0xDE, 16.2}, {0x0D, 3.26}, {0x3D, 4.22}, {0x73, 5.3},
{0xA9, 8.3}, {0xDF, 16.4}, {0x0E, 3.28}, {0x3E, 4.24}, {0x74, 5.32}, {0xAA, 8.4}, {0xE0, 16.6}, {0x0F, 3.3},
{0x3F, 4.26}, {0x75, 5.34}, {0xAB, 8.5}, {0xE1, 16.8}, {0x10, 3.32}, {0x40, 4.28}, {0x76, 5.36}, {0xAC, 8.6},
{0xE2, 17}, {0x11, 3.34}, {0x41, 4.3}, {0x77, 5.38}, {0xAD, 8.7}, {0xE3, 17.2}, {0x12, 3.36}, {0x42, 4.32}, {0x78, 5.4},
{0xAE, 8.8}, {0xE4, 17.4}, {0x13, 3.38}, {0x43, 4.34}, {0x79, 5.42}, {0xAF, 8.9}, {0xE5, 17.6}, {0x14, 3.4},
{0x44, 4.36}, {0x7A, 5.44}, {0xB0, 9}, {0xE6, 17.8}, {0x15, 3.42}, {0x45, 4.38}, {0x7B, 5.46}, {0xB1, 9.1}, {0xE7, 18},
{0x16, 3.44}, {0x46, 4.4}, {0x7C, 5.48}, {0xB2, 9.2}, {0xE8, 18.2}, {0x17, 3.46}, {0x47, 4.42}, {0x7D, 5.5},
{0xB3, 9.3}, {0xE9, 18.4}, {0x18, 3.48}, {0x48, 4.44}, {0x7E, 5.52}, {0xB4, 9.4}, {0xEA, 18.6}, {0x19, 3.5},
{0x49, 4.46}, {0x7F, 5.54}, {0xB5, 9.5}, {0xEB, 18.8}, {0x1A, 3.52}, {0x4A, 4.48}, {0x80, 5.56}, {0xB6, 9.6},
{0xEC, 19}, {0x1B, 3.54}, {0x4B, 4.5}, {0x81, 5.58}, {0xB7, 9.7}, {0xED, 19.2}, {0x1C, 3.56}, {0x4C, 4.52}, {0x82, 5.6},
{0xB8, 9.8}, {0xEE, 19.4}, {0x1D, 3.58}, {0x4D, 4.54}, {0x83, 5.62}, {0xB9, 9.9}, {0xEF, 19.6}, {0x1E, 3.6},
{0x4E, 4.56}, {0x84, 5.64}, {0xBA, 10}, {0xF0, 19.8}, {0x1F, 3.62}, {0x4F, 4.58}, {0x85, 5.66}, {0xBB, 10.1},
{0xF1, 20}, {0x20, 3.64}, {0x50, 4.6}, {0x86, 5.68}, {0xBC, 10.2}, {0x21, 3.66}, {0x51, 4.62}, {0x87, 5.7},
{0xBD, 10.3}, {0x22, 3.68}, {0x52, 4.64}, {0x88, 5.72}, {0xBE, 10.4}, {0x23, 3.7}, {0x53, 4.66}, {0x89, 5.74},
{0xBF, 10.5}, {0x24, 3.72}, {0x54, 4.68}, {0x8A, 5.76}, {0xC0, 10.6}, {0x25, 3.74}, {0x55, 4.7}, {0x8B, 5.78},
{0xC1, 10.7}, {0x26, 3.76}, {0x56, 4.72}, {0x8C, 5.8}, {0xC2, 10.8}, {0x27, 3.78}, {0x57, 4.74}, {0x8D, 5.82},
{0xC3, 10.9}, {0x28, 3.8}, {0x58, 4.76}, {0x8E, 5.84}, {0xC4, 11}, {0x29, 3.82}, {0x59, 4.78}, {0x8F, 5.86},
{0xC5, 11.2}, {0x2A, 3.84}, {0x5A, 4.8}, {0x90, 5.88}, {0xC6, 11.4}, {0x2B, 3.86}, {0x5B, 4.82}, {0x91, 5.9},
{0xC7, 11.6}, {0x2C, 3.88}, {0x5C, 4.84}, {0x92, 6}, {0xC8, 11.8}, {0x2D, 3.9}, {0x5D, 4.86}, {0x93, 6.1}, {0xC9, 12},
{0x2E, 3.92}, {0x5E, 4.88}, {0x94, 6.2}, {0xCA, 12.2}, {0x2F, 3.94}, {0x5F, 4.9}, {0x95, 6.3}, {0xCB, 12.4},
{0x30, 3.96}, {0x60, 4.92}, {0x96, 6.4}, {0xCC, 12.6}, {0x31, 3.98}, {0x61, 4.94}, {0x97, 6.5}, {0xCD, 12.8}, {0x32, 4},
{0x62, 4.96}, {0x98, 6.6}, {0xCE, 13}, {0x33, 4.02}, {0x63, 4.98}, {0x99, 6.7}, {0xCF, 13.2}, {0x34, 4.04}, {0x64, 5},
{0x9A, 6.8}, {0xD0, 13.4}, {0x35, 4.06}, {0x65, 5.02}, {0x9B, 6.9}, {0xD1, 13.6}
};

void test_convertVoltToRegisterValue_should_return_correct_value_for_alltesting_data(void) {
	for (testing_sample sample : testing_data) {
		TEST_ASSERT_EQUAL(
				sample.register_value,
				test_stpd01->convertVoltToRegisterValue(static_cast<uint16_t>(sample.volt_float*1000))
		);
	}
}

void test_convertRegisterValueToVolt_should_return_correct_value_for_alltesting_data(void) {
    for (testing_sample sample : testing_data) {
        TEST_ASSERT_EQUAL(
                static_cast<uint16_t>(sample.volt_float*1000),
                test_stpd01->convertRegisterValueToVolt(static_cast<uint8_t>(sample.register_value))
        );
    }
}

int runUnityTests(void) {

	UNITY_BEGIN();
	RUN_TEST(test_convertVoltToRegisterValue_should_return_correct_value_for_alltesting_data);
	RUN_TEST(test_convertRegisterValueToVolt_should_return_correct_value_for_alltesting_data);
	test_stpd01->~Test_STPD01();
	return UNITY_END();
}

void setup(void) {
	delay(2000);
	runUnityTests();
}

void loop(void) {
	delay(1);
}
