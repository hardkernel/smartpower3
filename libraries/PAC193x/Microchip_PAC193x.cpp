/***********************************************************

   This is a library for Microchip PAC193x
	
   © 2020 Microchip Technology Inc. and its subsidiaries.  
 
   Subject to your compliance with these terms, you may use Microchip
   software and any derivatives of this software. You must retain the above
   copyright notice with any redistribution of this software and the 
   following disclaimers. 
   It is your responsibility to comply with third party license terms 
   applicable to your use of third party software (including open source 
   software) that may accompany this Microchip software.
  
   THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
   EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING 
   ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
   FOR A PARTICULAR PURPOSE.  
  
   IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
   INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
   WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
   HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. 
   TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
   CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF
   FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
   
***********************************************************/

/***********************************************************

	Version 1.0.0

***********************************************************/

#include "Microchip_PAC193x.h"

Microchip_PAC193x::Microchip_PAC193x() { 

	rsense = RSENSE;
	errorCode = 0;

}

Microchip_PAC193x::Microchip_PAC193x(uint32_t resistorValue) { 

	rsense = resistorValue;
	errorCode = 0;

}

void Microchip_PAC193x::begin(TwoWire *_wire){ 
	this->_wire = _wire;
		
		Write8(PAC1934_NEG_PWR_ADDR, 0);
		Write8(PAC1934_CTRL_ADDR, 2);
		//Write8(PAC1934_CHANNEL_DIS_ADDR, 0);	// Use defaults
		Write8(PAC1934_SLOW_ADDR, 20); //14h

		// update slow status affecting default sampling speed
		UpdateSlowStatus();

		Refresh();
		delay(125);
}

void Microchip_PAC193x::Read(uint8_t reg_address, int Nbytes, uint8_t *pBuffer) {
	
    int byteCount = Nbytes;
	_wire->beginTransmission (I2C_ADDRESS);
#if (ARDUINO >= 100)
	_wire->write(reg_address); 
#else
	_wire->send(reg_address); 
#endif
	errorCode = _wire->endTransmission(false); 
	if (errorCode != 0){
		errorCode = (-1);
	}
	_wire->requestFrom(I2C_ADDRESS, Nbytes); 
#if (ARDUINO >= 100)
	while(_wire->available() && (byteCount > 0))    // slave may send less than requested
  {  
	*pBuffer = _wire->read();
	pBuffer++;
    byteCount--;
  }
#else
	while(_wire->available() && (byteCount > 0))    // slave may send less than requested
  {  
	*pBuffer = _wire->receive();
	pBuffer++;
    byteCount--;
  }
#endif
}

uint8_t Microchip_PAC193x::Read8(uint8_t reg_address) {
	uint8_t buffer[1];
	uint8_t *ptemp;
	
	ptemp = (uint8_t *)&buffer[0];
	Read(reg_address, 1, buffer);
	
	return *ptemp;
}
	
uint16_t Microchip_PAC193x::Read16(uint8_t reg_address) {
	uint8_t buffer[2];
	uint16_t *ptemp;
	
	ptemp = (uint16_t *)&buffer[0];
	Read(reg_address, 2, buffer);

	return *ptemp;
}	

uint32_t Microchip_PAC193x::Read32(uint8_t reg_address) {
	uint8_t buffer[4];
	uint32_t *ptemp;
	
	ptemp = (uint32_t *)&buffer[0];	
	Read(reg_address, 4, buffer);
	
	return *ptemp;
}

uint64_t Microchip_PAC193x::Read64(uint8_t reg_address) {
	uint8_t buffer[8];
	uint64_t *ptemp;
	
	ptemp = (uint64_t *)&buffer[0];	
	Read(reg_address, 8, buffer);
	
	return *ptemp;
} 

void Microchip_PAC193x::Write8(uint8_t reg_address, uint8_t data) {
	_wire->beginTransmission(I2C_ADDRESS); // start transmission to device 
#if (ARDUINO >= 100)
	_wire->write(reg_address); // sends register address to read from
	_wire->write(data);  // write data
#else
	_wire->send(reg_address); // sends register address to read from
	_wire->send(data);  // write data
#endif
	errorCode = _wire->endTransmission();  // end transmission
	if (errorCode != 0){
		errorCode = (-2);
	}
}

int16_t Microchip_PAC193x::UpdateProductID(){
	
	errorCode = 0;
	ProductID = Read8(PAC1934_PRODUCT_ID_ADDR); 
	
	return errorCode;
}

int16_t Microchip_PAC193x::UpdateManufacturerID(){
	
	errorCode = 0;
	ManufacturerID = Read8(PAC1934_MANUFACTURER_ID_ADDR); 
	
	return errorCode;
}

int16_t Microchip_PAC193x::UpdateRevisionID(){
	
	errorCode = 0;
	RevisionID = Read8(PAC1934_REVISION_ID_ADDR); 
	
	return errorCode;
}

void Microchip_PAC193x::Refresh(){

	// Refesh needs only one byte with no data
		
	_wire->beginTransmission(I2C_ADDRESS);
#if (ARDUINO >= 100)
	_wire->write(PAC1934_REFRESH_CMD_ADDR);
#else
	_wire->send(PAC1934_REFRESH_CMD_ADDR);
#endif
	errorCode = _wire->endTransmission();

	if (errorCode != 0){
		errorCode = (-2);
	}

	refresh_timestamp = micros();

}

int16_t Microchip_PAC193x::UpdateSampleRateLat(){
	uint16_t sampleRateVal;
	uint8_t sampleRateBits;

	errorCode = 0;
	sampleRateBits = Read8(PAC1934_CTRL_LAT_ADDR);
	sampleRateBits = ((sampleRateBits & 0xC0) >> 6);
    switch(sampleRateBits){
        case 0:
            sampleRateVal = 1024;
            break;
        case 1:
            sampleRateVal = 256;
            break;
        case 2:
            sampleRateVal = 64;
            break;
        case 3:
            sampleRateVal = 8;
            break;
        // This should not happen but compiler requirements
        default:
            sampleRateVal = (SlowStatus) ? 8 : 1024;
    }
	
	SampleRateLat = sampleRateVal;
	
    return errorCode;
}

int16_t Microchip_PAC193x::setSampleRate(uint16_t value){
	uint8_t temp_read, temp_write;
	
	errorCode = 0;
	temp_read = Read8(PAC1934_CTRL_ADDR);
	
	switch(value){
        case 1024:
            temp_write = 0;
            break;
        case 256:
            temp_write = 1;
            break;
        case 64:
            temp_write = 2;
            break;
        case 8:
            temp_write = 3;
            break;
        default:
            temp_write = 0;
			errorCode = (-4);
            return errorCode;
    }

	temp_write = (uint8_t)((temp_write << 6) + (temp_read & 0x3F));
	Write8(PAC1934_CTRL_ADDR, temp_write);
	Refresh();
	delay(125);
	
	return errorCode;
}

int16_t Microchip_PAC193x::UpdateOverflowAlert(){
	uint8_t temp_read;
	
	errorCode = 0;
	temp_read = Read8(PAC1934_CTRL_ACT_ADDR);
	temp_read = (uint8_t)(temp_read & 0x01); // bit 0
	
	OverflowAlert = temp_read;
	
	return errorCode;
}
int16_t Microchip_PAC193x::UpdateSlowStatus(){
	uint8_t temp_read;
	
	errorCode = 0;
	temp_read = Read8(PAC1934_SLOW_ADDR);
	temp_read = (uint8_t)((temp_read & 0x80) >> 7); // bit 7
	
	SlowStatus = temp_read;
	
	return errorCode;
}
int16_t Microchip_PAC193x::UpdatePowerOnStatus(){
	uint8_t temp_read, temp_write;
	
	errorCode = 0;
	temp_read = Read8(PAC1934_SLOW_ADDR);
	temp_read = (uint8_t)(temp_read & 0x01); // bit 0
	temp_write = (uint8_t)(temp_read & 0xFE);
	Write8(PAC1934_SLOW_ADDR, temp_write);
	
	PowerOnStatus = temp_read;
	
	return errorCode;
}
