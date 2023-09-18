#ifndef SMARTPOWER3_SCPIMANAGER_H_
#define SMARTPOWER3_SCPIMANAGER_H_


#include <SCPI_Parser.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WiFi.h>
#include "settings.h"
#include "screenmanager.h"
#include <SCPI_Parser.h>
#include <meas_channel.h>


#define SCPI_INPUT_BUFFER_LENGTH 256
#define SCPI_ERROR_QUEUE_SIZE 17
#define SCPI_IDN1 "Hardkernel Co Ltd"
#define SCPI_IDN2 "SmartPower3"
#define MAXROW 1    //maximum number of rows
#define MAXCOL 3    //maximum number of columns
#define MAXDIM 1    //maximum number of dimensions


class UserContext
{
public:
	UserContext(ScreenManager *screen_manager, MeasChannels *measuring_channels);
	ScreenManager *screen_manager;
	Settings *settings;
	MeasChannels *measuring_channels;
};


class SCPIManager
{
public:
	SCPIManager(ScreenManager *screen_manager, MeasChannels *measuring_channels);
	~SCPIManager();
	void init(void);
	void processInput(Stream &communication_interface);

	static size_t SCPI_Write(scpi_t * context, const char * data, size_t len);
	static int SCPI_Error(scpi_t * context, int_fast16_t err);
	static scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val);
	static scpi_result_t SCPI_Reset(scpi_t * context);
	static scpi_result_t SCPI_Flush(scpi_t * context);
	static scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context);

	static scpi_result_t SCPI_NetworkMACQ(scpi_t * context);
	static scpi_result_t SCPI_NetworkAddress(scpi_t * context);
	static scpi_result_t SCPI_NetworkAddressQ(scpi_t * context);

	static scpi_result_t SCPI_NetworkGate(scpi_t * context);
	static scpi_result_t SCPI_NetworkGateQ(scpi_t * context);

	static scpi_result_t SCPI_NetworkSubnet(scpi_t * context);
	static scpi_result_t SCPI_NetworkSubnetQ(scpi_t * context);

	static scpi_result_t SCPI_NetworkPort(scpi_t * context);
	static scpi_result_t SCPI_NetworkPortQ(scpi_t * context);

	static scpi_result_t SCPI_NetworkHostnameQ(scpi_t * context);

	static scpi_result_t SCPI_NetworkDHCP(scpi_t * context);
	static scpi_result_t SCPI_NetworkDHCPQ(scpi_t * context);

	static scpi_result_t SCPI_SocketPort(scpi_t * context);
	static scpi_result_t SCPI_SocketPortQ(scpi_t * context);
	static scpi_result_t SCPI_SocketIPAddress(scpi_t * context);
	static scpi_result_t SCPI_SocketIPAddressQ(scpi_t * context);

	static scpi_result_t SCPI_SocketConnect(scpi_t * context);
	static scpi_result_t SCPI_SocketDisconnect(scpi_t * context);
	static scpi_result_t SCPI_SocketFeed(scpi_t * context);
	static scpi_result_t SCPI_SocketFeedQ(scpi_t * context);
	static scpi_result_t SCPI_SerialFeed(scpi_t * context);
	static scpi_result_t SCPI_SerialFeedQ(scpi_t * context);
	static scpi_result_t SCPI_RLState(scpi_t * context);
	static scpi_result_t SCPI_RLStateQ(scpi_t * context);

	static scpi_result_t My_CoreTstQ(scpi_t * context);

	static scpi_result_t DeviceCapability(scpi_t * context);
	static scpi_result_t Reset(scpi_t * context);
	static scpi_result_t Output_TurnOnOff(scpi_t *context);
	static scpi_result_t Output_TurnOnOffQ(scpi_t *context);
	static scpi_result_t SCPI_LoggingInterval(scpi_t * context);
	static scpi_result_t SCPI_LoggingIntervalQ(scpi_t * context);
	static scpi_result_t DMM_FetchVoltageDcQ(scpi_t *context);
	static scpi_result_t DMM_FetchCurrentDcQ(scpi_t *context);
	static scpi_result_t DMM_FetchPowerDcQ(scpi_t *context);
	static scpi_result_t DMM_ConfigureVoltage(scpi_t *context);
	static scpi_result_t DMM_ConfigureVoltageQ(scpi_t *context);
	static scpi_result_t DMM_ConfigureCurrent(scpi_t *context);
	static scpi_result_t DMM_ConfigureCurrentQ(scpi_t *context);

	char* getBuildDate(void);
	const char* getMacAddress(void);
protected:
private:
	UserContext *user_context;
	static Settings* getSettings(scpi_t *context);
	static scpi_result_t saveIpv4Address(scpi_t *context, void (Settings::*func)(IPAddress, bool));
	static scpi_result_t saveNetworkPort(scpi_t *context, void (Settings::*func)(uint16_t, bool));
	static float fetchChannelVoltage(uint8_t channel_number, float resolution, MeasChannels *channels);
	static float fetchChannelCurrent(uint8_t channel_number, float resolution, MeasChannels *channels);
	static float fetchChannelPower(uint8_t channel_number, float resolution, MeasChannels *channels);
	static scpi_result_t DMM_MeasureUnitDcQ(scpi_t *context, scpi_unit_t allowed_unit,
											float (*reading_method)(uint8_t, float, MeasChannels*));
	static scpi_result_t processChannelList(scpi_t *context, scpi_parameter_t &channel_list_param, float resolution,
											float (*reading_method)(uint8_t, float, MeasChannels*));
	static scpi_result_t reprocessNumberParam(scpi_t *context, scpi_parameter_t &original_param,
											  scpi_number_t *number_param, scpi_unit_t allowed_unit);
	static void resetContext(scpi_t *context, char *pos, int_fast16_t input_count);

	char build_date[34];
	String mac_address;
	char incoming_byte = 0; // for incoming serial data
	char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
	scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];
	scpi_t scpi_context;
	scpi_interface_t scpi_interface = {
		/*.error = */this->SCPI_Error,
		/*.write = */this->SCPI_Write,
		/*.control = */this->SCPI_Control,
		/*.flush = */this->SCPI_Flush,
		/*.reset = */this->SCPI_Reset,
	};
	const scpi_command_t scpi_commands[52] = {
		/* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
		{ "*CLS", SCPI_CoreCls, 0 },
		{ "*ESE", SCPI_CoreEse, 0 },
		{ "*ESE?", SCPI_CoreEseQ, 0 },
		{ "*ESR?", SCPI_CoreEsrQ, 0 },
		{ "*IDN?", SCPI_CoreIdnQ, 0 },
		{ "*OPC", SCPI_CoreOpc, 0 },
		{ "*OPC?", SCPI_CoreOpcQ, 0 },
//TODO: Check if reset is complete or needs rework
		{ "*RST", Reset, 0 },
		{ "*SRE", SCPI_CoreSre, 0 },
		{ "*SRE?", SCPI_CoreSreQ, 0 },
		{ "*STB?", SCPI_CoreStbQ, 0 },
		{ "*TST?", My_CoreTstQ, 0 },
		{ "*WAI", SCPI_CoreWai, 0 },

		/* Required SCPI commands (SCPI std V1999.0 4.2.1) */
		{ "SYSTem:ERRor[:NEXT]?", SCPI_SystemErrorNextQ, 0 },
		{ "SYSTem:ERRor:COUNt?", SCPI_SystemErrorCountQ, 0 },
		{ "SYSTem:VERSion?", SCPI_SystemVersionQ, 0 },
		/* Non required SYSTem commands */
		{ "SYSTem:CAPability?", DeviceCapability, 0 },

		{"SYSTem:COMMunicate:NETwork:MAC?", SCPI_NetworkMACQ, 0 },
		// This query returns the MAC address of the Ethernet module. MAC address consist of two number groups:
		// the first three bytes are known as the Organizationally Unique Identifier (OUI),
		// which is distributed by the IEEE, and the last three bytes are the device’s unique serial number.
		// The six bytes are separated by hyphens. The MAC address is unique to the instrument
		// and cannot be altered by the user.
		// Return Param <XX-XX-XX-YY-YY-YY>
		{"SYSTem:COMMunicate:NETwork:ADDRess", SCPI_NetworkAddress, 0 },
		// This command sets the static address of the Ethernet module of the power supply.
		{"SYSTem:COMMunicate:NETwork:ADDRess?",  SCPI_NetworkAddressQ, 0 },
		{"SYSTem:COMMunicate:NETwork:GATE", SCPI_NetworkGate, 0 },
		{"SYSTem:COMMunicate:NETwork:GATE?", SCPI_NetworkGateQ, 0 },
		// This command sets the Gateway IP address of the Ethernet module of the power supply.
		// The Gateway IP defaults to 0.0.0.0 in absence of a DHCP server.
		// Gateway IP address is represented with 4 bytes each having a range of 0-255 separated by dots
		{"SYSTem:COMMunicate:NETwork:SUBNet", SCPI_NetworkSubnet, 0 },
		{"SYSTem:COMMunicate:NETwork:SUBNet?", SCPI_NetworkSubnetQ, 0 },
		// This command sets the subnet IP Mask address of the Ethernet module of the power supply.
		// The factory subnet mask setting is 255.255.255.0.

		// Currently no need for this
		//{"SYSTem:COMMunicate:NETwork:PORT", SCPI_NetworkPort, 0 },
		//{"SYSTem:COMMunicate:NETwork:PORT?", SCPI_NetworkPortQ, 0 },

		// This command sets the Socket (Port) of the Ethernet module of the power supply.
		// The factory default port setting is 50505. The factory recommends port values greater than 49151
		// to avoid conflicts with registered Ethernet port functions.
		//{"[SYSTem][:COMMunicate]:NETwork:HOSTname?", SCPI_NetworkHostnameQ, 0 },
		// This query reads the host name of the Ethernet communications module.
		{"SYSTem:COMMunicate:NETwork:DHCP", SCPI_NetworkDHCP, 0 },
		{"SYSTem:COMMunicate:NETwork:DHCP?", SCPI_NetworkDHCPQ, 0 },
		// This command sets the DHCP operating mode of the Ethernet module. If DHCP is set to 1,
		// the module will allow its IP address to be automatically set by the DHCP server on the network.
		// If DHCP is set to 0, the default IP address is set according to .
		{"SYSTem:COMMunicate:SOCKet:ADDRess", SCPI_SocketIPAddress, 0 },
		{"SYSTem:COMMunicate:SOCKet:ADDRess?", SCPI_SocketIPAddressQ, 0 },
		{"SYSTem:COMMunicate:SOCKet:PORT", SCPI_SocketPort, 0 },
		{"SYSTem:COMMunicate:SOCKet:PORT?", SCPI_SocketPortQ, 0 },
		// This is done as SOCket (as opposed to network, becouse it enables forward compatibility, should there be
		// more sockets used, with different mechanisms.
		{"SYSTem:COMMunicate:SOCKet:CONNect", SCPI_SocketConnect, 0},
		{"SYSTem:COMMunicate:SOCKet:DISConnect", SCPI_SocketDisconnect, 0},

		{"SYSTem:COMMunicate:SOCKet:FEED", SCPI_SocketFeed, 0},
		{"SYSTem:COMMunicate:SOCKet:FEED?", SCPI_SocketFeedQ, 0},

		{"SYSTem:COMMunicate:SERial:FEED", SCPI_SerialFeed, 0},
		{"SYSTem:COMMunicate:SERial:FEED?", SCPI_SerialFeedQ, 0},

		{"SYSTem:COMMunicate:RLSTate", SCPI_RLState, 0}, //LOCal|REMote|RWLock // RWLock is not supported
		{"SYSTem:COMMunicate:RLSTate?", SCPI_RLStateQ, 0},

		{"SENSe:FUNCtion:TINTerval", SCPI_LoggingInterval, 0},
		{"SENSe:FUNCtion:TINTerval?", SCPI_LoggingIntervalQ, 0},
		/* DMM */
		{ "FETCh[:SCALar]:VOLTage[:DC]?", DMM_FetchVoltageDcQ, 0 },
		{ "FETCh[:SCALar]:CURRent[:DC]?", DMM_FetchCurrentDcQ, 0 },
		{ "FETCh[:SCALar]:POWer[:DC]?", DMM_FetchPowerDcQ, 0 },

		{"[SOURce#]:CURRent", DMM_ConfigureCurrent, 0 },
		{"[SOURce#]:CURRent?", DMM_ConfigureCurrentQ, 0 },
		{"[SOURce#]:VOLTage", DMM_ConfigureVoltage, 0 },
		{"[SOURce#]:VOLTage?", DMM_ConfigureVoltageQ, 0 },

		{ "OUTPut#[:STATe]", Output_TurnOnOff, 0 },
		{ "OUTPut#[:STATe]?", Output_TurnOnOffQ, 0 },

		SCPI_CMD_LIST_END
	};
};


#endif /* SMARTPOWER3_SCPIMANAGER_H_ */
