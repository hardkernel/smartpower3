#ifndef Microchip_PAC193x_h
#define Microchip_PAC193x_h

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include "Wire.h"

#define RSENSE 24900000 //microohm
#define I2C_ADDRESS 0x10
#define CHANNEL 1

#define PAC1934_REFRESH_CMD_ADDR            0x00
#define PAC1934_CTRL_ADDR                   0x01
#define PAC1934_ACC_COUNT_ADDR              0x02
#define PAC1934_VPOWER1_ACC_ADDR            0x03
#define PAC1934_VPOWER2_ACC_ADDR            0x04
#define PAC1934_VPOWER3_ACC_ADDR            0X05
#define PAC1934_VPOWER4_ACC_ADDR            0X06
#define PAC1934_VBUS1_ADDR                  0x07
#define PAC1934_VBUS2_ADDR                  0x08
#define PAC1934_VBUS3_ADDR                  0x09
#define PAC1934_VBUS4_ADDR                  0x0A
#define PAC1934_VSENSE1_ADDR                0x0B
#define PAC1934_VSENSE2_ADDR                0x0C
#define PAC1934_VSENSE3_ADDR                0X0D
#define PAC1934_VSENSE4_ADDR                0X0E
#define PAC1934_VBUS1_AVG_ADDR              0X0F
#define PAC1934_VBUS2_AVG_ADDR              0X10
#define PAC1934_VBUS3_AVG_ADDR              0X11
#define PAC1934_VBUS4_AVG_ADDR              0X12
#define PAC1934_VSENSE1_AVG_ADDR            0X13
#define PAC1934_VSENSE2_AVG_ADDR            0X14
#define PAC1934_VSENSE3_AVG_ADDR            0X15
#define PAC1934_VSENSE4_AVG_ADDR            0X16
#define PAC1934_VPOWER1_ADDR                0X17
#define PAC1934_VPOWER2_ADDR                0X18
#define PAC1934_VPOWER3_ADDR                0X19
#define PAC1934_VPOWER4_ADDR                0X1A
#define PAC1934_CHANNEL_DIS_ADDR            0X1C
#define PAC1934_NEG_PWR_ADDR                0X1D
#define PAC1934_REFRESH_G_CMD_ADDR          0x1E
#define PAC1934_REFRESH_V_CMD_ADDR          0x1F
#define PAC1934_SLOW_ADDR                   0X20
#define PAC1934_CTRL_ACT_ADDR               0X21
#define PAC1934_CHANNEL_DIS_ACT_ADDR        0X22 
#define PAC1934_NEG_PWR_ACT_ADDR            0X23
#define PAC1934_CTRL_LAT_ADDR               0X24
#define PAC1934_CHANNEL_DIS_LAT_ADDR        0X25
#define PAC1934_NEG_PWR_LAT_ADDR            0x26

#define PAC1934_PRODUCT_ID_ADDR             0xFD
#define PAC1934_MANUFACTURER_ID_ADDR        0xFE
#define PAC1934_REVISION_ID_ADDR            0xFF

class Microchip_PAC193x {  
		public:
			Microchip_PAC193x();
			Microchip_PAC193x(uint32_t resistorValue);
			void begin(TwoWire *_wire = &Wire);
/*
    Function
		Refresh()
	Summary
        Executes a 'REFRESH' command.
    Description
        This method executes a 'REFRESH' command. In this case, the accumulator data, 
        accumulator count, Vbus, Vsense measurements are all refreshed 
        and the accumulators are reset. 
        The updated data is stable and can be read after 1ms.
	Input
		None.
	Output
		None.
    Returns
		None.
*/	
			void Refresh();

/*
    Function
		UpdateOverflowAlert()
	Summary
        Obtains the 'Overflow' bit value.
    Description
        This method obtains the 'Overflow' indication status bit value. This bit will be set to '1' if
		any of the accumulators or the accumulator counter overflows. This bit is cleared by Refresh.
	Input
		None.
	Output
		The 'Overflow' bit value is assigned to OverflowAlert property of Microchip_PAC193x class.
		Possible values are:
            0 - no accumulator or accumulator counter overflow has occured
            1 - accumulator or accumulator counter has overflowed
    Returns
		In case of execution error, the returned value is an error code.
*/
			int16_t UpdateOverflowAlert();


/*
    Function
		UpdateSlowStatus()
	Summary
        Obtains the 'Slow' bit value.
    Description
        This method obtains the 'Slow' bit value of the Slow register.
	Input
		None.
	Output
		The 'Slow' bit value is assigned to SlowStatus property of Microchip_PAC193x class.
		Possible values are:
            0 - The Slow pin is pulled low externally
            1 - The Slow pin is pulled high externally
    Returns
		In case of execution error, the returned value is an error code.
*/
			int16_t UpdateSlowStatus();


/*
    Function
		UpdatePowerOnStatus()
	Summary
        Obtains the 'POR' bit value.
    Description
        This method obtains the 'POR' bit value.
	Input
		None.
	Output
		The 'POR' bit value is assigned to PowerOnStatus property of Microchip_PAC193x class.
		Possible values are:
            0 - This bit has been creared over i2C since the last Power On occured
            1 - This bit has the POR default value of 1 and it hasn't been cleared since the last reset occured.
    Returns
		In case of execution error, the returned value is an error code.
*/
			int16_t UpdatePowerOnStatus();


/*
    Function
		UpdateSampleRateLat()
	Summary
        Obtains the 'SAMPLE_RATE_LAT' value
    Description
        This method obtains the sample rate value that was active before the
        most recent REFRESH command.
	Input
		None.
	Output
		The sample rate value that was active before the most recent REFRESH command is assigned to SampleRateLat property of Microchip_PAC193x class.
		Possible values are:
                1024 samples/s
                 256 samples/s
                  64 samples/s
                   8 samples/s
    Returns
		In case of execution error, the returned value is an error code.
*/
			int16_t UpdateSampleRateLat();


/*
    Function
		setSampleRate()
	Summary
        Sets the sample rate value.
    Description
        This method sets the current value of the sample rate.
	Input
		value - the sample rate value to be set. Accepted values are: 
                                                        1024 samples/s
                                                         256 samples/s
                                                          64 samples/s
                                                           8 samples/s
	Output
		None.
    Returns
		In case of execution error, the returned value is an error code.
*/
			int16_t setSampleRate(uint16_t value);


/*
	Function
		UpdateProductID()
    Summary
        Obtains the Product ID value for the PAC193x.
    Description
        This method obtains the Product ID value for the PAC193x. This register is read-only.
	Input
		None
	Output
		The Product ID register value is assigned to ProductID property of Microchip_PAC193x class.
    Returns
		In case of execution error, the returned value is an error code.
*/
			int16_t UpdateProductID();


/*
    Function
		UpdateManufacturerID()
	Summary
        Obtains the Manufacturer ID value for the PAC193x.
    Description
        This method obtains the Manufacturer ID that identifies Microchip as 
        the manufacturer of the PAC193x. This register is read-only.
	Input
		None.
	Output
		The Manufacturer ID register value is assigned to ManufacturerID property of Microchip_PAC193x class.
    Returns
		In case of execution error, the returned value is an error code.
*/
			int16_t UpdateManufacturerID();


/*
    Function
		UpdateRevisionID()
	Summary
        Obtains the Revision ID value for the PAC193x.
    Description
        This method Obtains the die revision value. This register is read-only.
	Input
		None.
	Output
		The Revision ID register value is assigned to RevisionID property of Microchip_PAC193x class.
    Returns
		In case of execution error, the returned value is an error code.
*/
			int16_t UpdateRevisionID();

//class public properties:		
			uint8_t OverflowAlert;
			uint8_t SlowStatus;
			uint8_t PowerOnStatus;
			uint16_t SampleRateLat;
			uint8_t ProductID;
			uint8_t ManufacturerID;
			uint8_t RevisionID;

			uint32_t refresh_timestamp;
			
			void Read(uint8_t reg_address, int Nbytes, uint8_t *pBuffer);

		private:
//class private functions:		
			TwoWire *_wire;
			uint8_t Read8(uint8_t reg_address);
			uint16_t Read16(uint8_t reg_address);
			uint32_t Read32(uint8_t reg_address);
			uint64_t Read64(uint8_t reg_address);
			void Write8(uint8_t reg_address, uint8_t data);
//class private properties:
			uint32_t rsense;
			
/*	Property
		errorCode
	Description
		In case of execution error of one of the functions called,
		the value of errorCode indicates wich type of error has occured.
		Possible values:	 0 - No error
							-1 - Read from device i2C communication error
							-2 - Write to device i2C communication error
							-3 - rsense resitor value choice error (0 or negative value)
							-4 - Sample Rate choice error (only the following values are possible: 8, 64, 256, 1024)
*/			
			int16_t errorCode;
			
};

#endif
