#include "scpimanager.h"


#define DEFAULT_CHANNEL 1
#define DEFAULT_RESOLUTION 1
#define DEFAULT_EXPECTED_VALUE 1


UserContext::UserContext(ScreenManager *screen_manager, MeasChannels *measuring_channels)
{
	this->screen_manager = screen_manager;
	this->measuring_channels = measuring_channels;
	this->settings = screen_manager->getSettings();
}

SCPIManager::SCPIManager(ScreenManager *screen_manager, MeasChannels *measuring_channels)
{
	this->user_context = new UserContext(screen_manager, measuring_channels);
	mac_address.reserve(18);
}

SCPIManager::~SCPIManager()
{
}

void SCPIManager::init(void)
{
	snprintf(build_date, 34, F("Build date: %s %s"), F(__DATE__), F(__TIME__));
	SCPI_Init(
		&scpi_context,
		scpi_commands,
		&scpi_interface,
		scpi_units_def,
		//SCPI_IDN1, SCPI_IDN2, this->user_context->settings->getMacAddress().c_str(), this->getBuildDate(),
		SCPI_IDN1, SCPI_IDN2, this->getMacAddress(), this->getBuildDate(),
		scpi_input_buffer, SCPI_INPUT_BUFFER_LENGTH,
		scpi_error_queue_data, SCPI_ERROR_QUEUE_SIZE);

	this->scpi_context.user_context = this->user_context;

	Serial.end();
	Serial.begin(115200);
	while (!Serial); // wait for serial to finish initializing
	Serial.println(F("SCPI Interactive demo"));
}

size_t SCPIManager::SCPI_Write(scpi_t *context, const char *data, size_t len)
{
	(void) context;
	Serial.write(data, len);
	return len;
}

scpi_result_t SCPIManager::SCPI_Flush(scpi_t *context)
{
	(void) context;
	Serial.flush();
	return SCPI_RES_OK;
}

int SCPIManager::SCPI_Error(scpi_t *context, int_fast16_t err)
{
	(void) context;
	Serial.print(F("**ERROR: "));
	Serial.print(err);
	Serial.print(F(", \""));
	Serial.print(SCPI_ErrorTranslate (err));
	Serial.println(F("\""));
	return 0;
}

scpi_result_t SCPIManager::SCPI_Control(scpi_t *context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val)
{
	(void) context;

	if (SCPI_CTRL_SRQ == ctrl) {
		Serial.print(F("**SRQ: 0x"));
		Serial.print(val, HEX);
		Serial.print(F("("));
		Serial.print(val, DEC);
		Serial.println(F(")"));
	} else {
		Serial.print(F("**CTRL: "));
		Serial.print(val, HEX);
		Serial.print(F("("));
		Serial.print(val, DEC);
		Serial.println(F(")"));
	}
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::SCPI_Reset(scpi_t * context)
{
	(void) context;
	Serial.println(F("**Reset"));
	return SCPI_RES_OK;
}

void SCPIManager::processInput(Stream &communication_interface)
{
	if (communication_interface.available () > 0) {
		incoming_byte = communication_interface.read ();
		SCPI_Input(&scpi_context, &incoming_byte, 1);
	}
}

scpi_result_t SCPIManager::SCPI_SystemCommTcpipControlQ(scpi_t *context)
{
	(void) context;
	return SCPI_RES_ERR;
}

scpi_result_t SCPIManager::My_CoreTstQ(scpi_t * context)
{
//TODO: test low_power for connected power supply
	SCPI_ResultInt32(context, 0);
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::DeviceCapability(scpi_t * context)
{
	SCPI_ResultMnemonic(context, "DCPSUPPLY&MEASURE&MULTIPLE");
	//Serial.print ("**SRQ: 0x");
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::Reset(scpi_t * context)
{
	// Return device to defined known state
	// Should be a safe state in the sense that connected devices are not damaged or destroyed
	// for example by setting high output voltage
	uint8_t *onoff = (static_cast<UserContext *>(context->user_context))->screen_manager->getOnOff();

	for (int idx = 0; idx < 2; idx++) {
		if (onoff[idx] == 1) {
			onoff[idx] = 2;
		}
	}
	return SCPI_CoreRst(context);

//	return SCPI_RES_OK;
}

struct _scpi_channel_value_t {
    int32_t row;
    int32_t col;
};
typedef struct _scpi_channel_value_t scpi_channel_value_t;

void SCPIManager::resetContext(scpi_t *context, char *pos, int_fast16_t input_count)
{
	context->param_list.lex_state.pos = pos;
	context->input_count = input_count;
}

scpi_result_t SCPIManager::reprocessNumberParam(scpi_t *context, scpi_parameter_t &original_param,
											 scpi_number_t *number_param, scpi_unit_t allowed_unit)
{
	scpi_t *working_context = context;
	char *saved_position = context->param_list.lex_state.pos;
	int_fast16_t saved_input_count = context->input_count;

	working_context->param_list.lex_state.pos = original_param.ptr;
	working_context->input_count = 0;

	if (SCPI_ParamNumber(working_context, scpi_special_numbers_def, number_param, FALSE)) {
		if (
				number_param->unit == allowed_unit
				|| (number_param->unit == SCPI_UNIT_NONE && number_param->special == false)
		) {
			resetContext(context, saved_position, saved_input_count);
			return SCPI_RES_OK;
		} else if (number_param->unit == SCPI_UNIT_NONE && number_param->special == true) {
			switch (number_param->content.tag) {
				case SCPI_NUM_DEF:
				case SCPI_NUM_AUTO:
					number_param->content.value = 1;
					resetContext(context, saved_position, saved_input_count);
					return SCPI_RES_OK;
				default:
					SCPI_ErrorPush (context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
					resetContext(context, saved_position, saved_input_count);
					return SCPI_RES_ERR;
			}
		} else {
			SCPI_ErrorPush (context, SCPI_ERROR_INVALID_SUFFIX);
			resetContext(context, saved_position, saved_input_count);
			return SCPI_RES_ERR;
		}
	}
	//SCPI_ErrorPush(context, SCPI_ERROR_INVALID_CHARACTER);
	resetContext(context, saved_position, saved_input_count);
	return SCPI_RES_ERR;
}

scpi_result_t SCPIManager::processChannelList(scpi_t *context, scpi_parameter_t &channel_list_param, float resolution,
											  float (*reading_method)(uint8_t, float, MeasChannels*))
{
	UserContext *user_ctx = static_cast<UserContext *>(context->user_context);
	MeasChannels *channels = user_ctx->measuring_channels;

	scpi_bool_t is_range;
	int32_t values_from[MAXDIM];
	int32_t values_to[MAXDIM];
	size_t dimensions;
	size_t chanlst_idx;
	size_t arr_idx = 0;
	size_t n = 1;

	bool for_stop_row = false; //true if iteration for rows has to stop
	int32_t dir_row = 1; //direction of counter for rows, +/-1

	// the next statement is valid usage and it gets only real number of dimensions for the first item (index 0)
	if (!SCPI_ExprChannelListEntry (context, &channel_list_param, 0, &is_range, NULL, NULL, 0, &dimensions)) {
		chanlst_idx = 0;
		arr_idx = 0;
		do { //if valid, iterate over channel_list_param index while res == valid (do-while cause we have to do it once)
			SCPI_ExprChannelListEntry (context, &channel_list_param, chanlst_idx, &is_range, values_from, values_to,
									   4, &dimensions);
			if (is_range == false) {
				if (dimensions == 1) {
					//call a function or something
					SCPI_ResultFloat(context, reading_method(values_from[0], resolution, channels));
				} else {
					return SCPI_RES_ERR;
					break;
				}
				arr_idx++; //inkrement array where we want to save our values to, not neccessary otherwise
			} else if (is_range == true) {
				if (values_from[0] > values_to[0]) {
					dir_row = -1; //we have to decrement from values_from
				} else { //if (values_from[0] < values_to[0])
					dir_row = +1; //default, we increment from values_from
				}
				//iterating over rows, do it once -> set for_stop_row = false
				//needed if there is channel list index isn't at end yet
				for_stop_row = false;
				for (n = values_from[0]; for_stop_row == false; n += dir_row) {
					//special case for range, example: (@2!1)
					if (dimensions == 1) {
						//here we have values
						//call function or sth.
						SCPI_ResultFloat(context, reading_method(n, resolution, channels));
						arr_idx++;
					}
					if (n == (size_t) values_to[0]) {
						for_stop_row = true;
					}
				}
			} else {
				return SCPI_RES_ERR;
				break;
			}
			chanlst_idx++;
		}
		while (SCPI_EXPR_OK
				== SCPI_ExprChannelListEntry (context, &channel_list_param, chanlst_idx, &is_range, values_from,
												values_to, 4, &dimensions));
	}
	//do something at the end if needed
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::DMM_MeasureUnitDcQ(scpi_t *context, scpi_unit_t allowed_unit,
											  float (*reading_method)(uint8_t, float, MeasChannels*))
{
	const uint8_t max_param_count = 3;
	UserContext *user_ctx = static_cast<UserContext *>(context->user_context);
	//Settings *settings = user_ctx->settings;
	MeasChannels *channels = user_ctx->measuring_channels;
	//ScreenManager *sm = user_ctx->screen_manager;
	scpi_number_t expected_value;
	scpi_number_t resolution;
	scpi_parameter_t working_param;
	uint8_t count = 0;
	scpi_parameter_t params[max_param_count];

	while (SCPI_RES_OK == SCPI_Parameter(context, &working_param, FALSE) && count < max_param_count) {
		if (!SCPI_ParamErrorOccurred(context)) {
			params[count] = working_param;
		}
		count++;
	}

	switch (count) {
		// no arguments at command line at all - use all defaults
		case 0:
			if (SCPI_ResultFloat(context, reading_method(DEFAULT_CHANNEL, DEFAULT_RESOLUTION, channels))) {
				return SCPI_RES_OK;
			}
			break;
		// channel list
		case 1:
			return processChannelList(context, params[0], DEFAULT_RESOLUTION, *reading_method);
		// expected_value && channel list
		case 2:
			if (
					// expected_value
					SCPI_RES_OK == reprocessNumberParam(context, params[0], &expected_value, allowed_unit)
					// channel list
					&& SCPI_RES_OK == processChannelList(context, params[1], DEFAULT_RESOLUTION, *reading_method)
			) {
				return SCPI_RES_OK;
			}
			break;
		// expected_value && resolution && channel_list
		case 3:
			if (
					// expected value
					SCPI_RES_OK == reprocessNumberParam(context, params[0], &expected_value, allowed_unit)
					// resolution
					&& SCPI_RES_OK == reprocessNumberParam(context, params[1], &resolution, allowed_unit)
					// channel list
					&& SCPI_RES_OK == processChannelList(context, params[2], resolution.content.value,
														 *reading_method)
			) {
				return SCPI_RES_OK;
			}
			break;
		// other number
		default:
			break;
	}
	return SCPI_RES_ERR;
}

float SCPIManager::fetchChannelVoltage(uint8_t channel_number, float resolution, MeasChannels *channels)
{
	// channel_number is one-based, but getChannel expects zero-based values
	return ((static_cast<float>(channels->getChannel(channel_number-1)->V()))/1000)/resolution;
}

scpi_result_t SCPIManager::DMM_FetchVoltageDcQ(scpi_t *context)
{
	return DMM_MeasureUnitDcQ(context, SCPI_UNIT_VOLT, *fetchChannelVoltage);
}

float SCPIManager::fetchChannelCurrent(uint8_t channel_number, float resolution, MeasChannels *channels)
{
	// channel_number is one-based, but getChannel expects zero-based values
	return ((static_cast<float>(channels->getChannel(channel_number-1)->A(2)))/1000)/resolution;
}

scpi_result_t SCPIManager::DMM_FetchCurrentDcQ (scpi_t *context)
{
	return DMM_MeasureUnitDcQ(context, SCPI_UNIT_AMPER, *fetchChannelCurrent);
}

float SCPIManager::fetchChannelPower(uint8_t channel_number, float resolution, MeasChannels *channels)
{
	// channel_number is one-based, but getChannel expects zero-based values
	return ((static_cast<float>(channels->getChannel(channel_number-1)->W(2)))/1000)/resolution;
}

scpi_result_t SCPIManager::DMM_FetchPowerDcQ (scpi_t *context)
{
	return DMM_MeasureUnitDcQ(context, SCPI_UNIT_WATT, *fetchChannelPower);
}

scpi_result_t SCPIManager::Output_TurnOnOff(scpi_t *context)
{
	scpi_bool_t onoff_parameter;
	uint8_t channel_number = 0;
	int32_t output_number[1];

	if (!SCPI_CommandNumbers(context, output_number, 1, 1)) {
		return SCPI_RES_ERR;
	}
	// this parameter is mandatory, so error out if not present
	if (!SCPI_ParamBool(context, &onoff_parameter, TRUE)) {
		return SCPI_RES_ERR;
	}
	if (output_number[0] < 1 || output_number[0] > 2) {
		SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
		return SCPI_RES_ERR;
	}

	channel_number = output_number[0] - 1;
	uint8_t *onoff = (static_cast<UserContext *>(context->user_context))->screen_manager->getOnOff();

	if (onoff[channel_number] == 1 && !onoff_parameter) {
		onoff[channel_number] = 2;
	} else if (onoff[channel_number] == 0 && onoff_parameter) {
		onoff[channel_number] = 3;
	}

	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::Output_TurnOnOffQ(scpi_t *context)
{
	uint8_t channel_number = 0;
	scpi_bool_t channel_on;
	int32_t output_number[1];

	if (!SCPI_CommandNumbers(context, output_number, 1, 1)) {
		return SCPI_RES_ERR;
	}
	if (output_number[0] < 1 || output_number[0] > 2) {
		SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
		return SCPI_RES_ERR;
	}

	channel_number = output_number[0] - 1;
	uint8_t *onoff = (static_cast<UserContext *>(context->user_context))->screen_manager->getOnOff();

	if (onoff[channel_number] == 1 || onoff[channel_number] == 3) {
		channel_on = 1;
	} else if (onoff[channel_number] == 0  || onoff[channel_number] == 2) {
		channel_on = 0;
	} else {
		return SCPI_RES_ERR;
	}

	SCPI_ResultBool(context, channel_on);
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::DMM_ConfigureVoltage(scpi_t *context)
{
	UserContext *user_ctx = static_cast<UserContext *>(context->user_context);
	Settings *settings = user_ctx->settings;
	scpi_number_t target_volts;
	int32_t output_number[1];

	if (!SCPI_CommandNumbers(context, output_number, 1, 1)) {
		return SCPI_RES_ERR;
	}
	// required to accept MIN and MAX
	if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &target_volts, TRUE)) {
		return SCPI_RES_ERR;
	}
	if (output_number[0] < 1 || output_number[0] > 2) {
		SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
		return SCPI_RES_ERR;
	}

	if (target_volts.unit == SCPI_UNIT_NONE
			|| target_volts.unit == SCPI_UNIT_UNITLESS
			|| target_volts.unit == SCPI_UNIT_VOLT
	) {
		if (target_volts.special && target_volts.content.tag == SCPI_NUM_MIN) {
			if (output_number[0] == 1) {
				settings->setChannel0Voltage(3000, true);  // hardware allowed minimum
			} else if (output_number[0] == 2) {
				settings->setChannel1Voltage(3000, true);  // hardware allowed minimu
			}
			return SCPI_RES_OK;
		} else if (target_volts.special && target_volts.content.tag == SCPI_NUM_MAX) {
			if (output_number[0] == 1) {
				settings->setChannel0Voltage(20000, true);  // hardware allowed maximum
			} else if (output_number[0] == 2) {
				settings->setChannel1Voltage(20000, true);  // hardware allowed maximum
			}
			return SCPI_RES_OK;
		} else if (target_volts.content.value < 3.0 || target_volts.content.value > 20) {
			//SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);  // full error list define
			SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
			return SCPI_RES_ERR;
		} else {
			if (output_number[0] == 1) {
				settings->setChannel0Voltage(static_cast<uint16_t>((target_volts.content.value)*1000), true);
			} else if (output_number[0] == 2) {
				settings->setChannel1Voltage(static_cast<uint16_t>((target_volts.content.value)*1000), true);
			}
			return SCPI_RES_OK;
		}
	} else {
		SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
		return SCPI_RES_ERR;
	}
	return SCPI_RES_ERR;
}

scpi_result_t SCPIManager::DMM_ConfigureVoltageQ(scpi_t *context)
{
	UserContext *user_ctx = static_cast<UserContext *>(context->user_context);
	Settings *settings = user_ctx->settings;
	int32_t output_number[1];

	if (!SCPI_CommandNumbers(context, output_number, 1, 1)) {
		return SCPI_RES_ERR;
	}
	if (output_number[0] < 1 || output_number[0] > 2) {
		SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
		return SCPI_RES_ERR;
	}
	if (output_number[0] == 1) {
		SCPI_ResultFloat(context, static_cast<float>(settings->getChannel0Voltage(true))/1000);
	} else if (output_number[0] == 2) {
		SCPI_ResultFloat(context, static_cast<float>(settings->getChannel1Voltage(true))/1000);
	}
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::DMM_ConfigureCurrent(scpi_t *context)
{
	UserContext *user_ctx = static_cast<UserContext *>(context->user_context);
	Settings *settings = user_ctx->settings;
	scpi_number_t target_amps;
	int32_t output_number[1];

	if (!SCPI_CommandNumbers(context, output_number, 1, 1)) {
		return SCPI_RES_ERR;
	}
	// required to accept MIN and MAX
	if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &target_amps, TRUE)) {
		return SCPI_RES_ERR;
	}
	if (output_number[0] < 1 || output_number[0] > 2) {
		SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
		return SCPI_RES_ERR;
	}

	if (target_amps.unit == SCPI_UNIT_NONE
			|| target_amps.unit == SCPI_UNIT_UNITLESS
			|| target_amps.unit == SCPI_UNIT_AMPER
	) {
		if (target_amps.special && target_amps.content.tag == SCPI_NUM_MIN) {
			if (output_number[0] == 1) {
				settings->setChannel0CurrentLimit(500, true);  // hardware allowed minimum
			} else if (output_number[0] == 2) {
				settings->setChannel1CurrentLimit(500, true);  // hardware allowed minimum
			}
			return SCPI_RES_OK;
		} else if (target_amps.special && target_amps.content.tag == SCPI_NUM_MAX) {
			if (output_number[0] == 1) {
				settings->setChannel0CurrentLimit(3000, true);  // hardware allowed maximum
			} else if (output_number[0] == 2) {
				settings->setChannel1CurrentLimit(3000, true);  // hardware allowed maximum
			}
			return SCPI_RES_OK;
		} else if (target_amps.content.value < 0.5 || target_amps.content.value > 3) {
			//SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);  // full error list define
			SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);  // full error list define
			return SCPI_RES_ERR;
		} else {
			Serial.println(output_number[0]);
			if (output_number[0] == 1) {
				settings->setChannel0CurrentLimit(static_cast<uint16_t>((target_amps.content.value)*1000), true);
			} else if (output_number[0] == 2) {
				settings->setChannel1CurrentLimit(static_cast<uint16_t>((target_amps.content.value)*1000), true);
			}
			return SCPI_RES_OK;
		}
	} else {
		SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
		return SCPI_RES_ERR;
	}
	return SCPI_RES_ERR;
}

scpi_result_t SCPIManager::DMM_ConfigureCurrentQ(scpi_t *context)
{
	UserContext *user_ctx = static_cast<UserContext *>(context->user_context);
	Settings *settings = user_ctx->settings;
	int32_t output_number[1];

	if (!SCPI_CommandNumbers(context, output_number, 1, 1)) {
		return SCPI_RES_ERR;
	}
	if (output_number[0] < 1 || output_number[0] > 2) {
		SCPI_ErrorPush(context, SCPI_ERROR_ILLEGAL_PARAMETER_VALUE);
		return SCPI_RES_ERR;
	}
	if (output_number[0] == 1) {
		SCPI_ResultFloat(context, static_cast<float>(settings->getChannel0CurrentLimit(true))/1000);
	} else if (output_number[0] == 2) {
		SCPI_ResultFloat(context, static_cast<float>(settings->getChannel1CurrentLimit(true))/1000);
	}
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::SCPI_NetworkMACQ(scpi_t *context)
{
	SCPI_ResultMnemonic(context, getSettings(context)->getMacAddress().c_str());
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::SCPI_NetworkDHCP (scpi_t *context)
{
	scpi_bool_t dhcp_enabled;
	// this parameter is mandatory, so error out if not present
	if (!SCPI_ParamBool(context, &dhcp_enabled, TRUE)) {
		return SCPI_RES_ERR;
	}

	getSettings(context)->setWifiIpv4DhcpEnabled(dhcp_enabled);
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::SCPI_NetworkDHCPQ (scpi_t *context)
{
	SCPI_ResultBool(context, getSettings(context)->isWifiIpv4DhcpEnabled());
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::SCPI_NetworkAddress (scpi_t *context)
{
	return SCPIManager::saveIpv4Address(context, &Settings::setWifiIpv4StaticIp);
}

scpi_result_t SCPIManager::SCPI_NetworkAddressQ (scpi_t *context)
{
	SCPI_ResultMnemonic(context, getSettings(context)->getWifiIpv4StaticIp(true).toString().c_str());
	return SCPI_RES_OK;

	//SCPI_ResultMnemonic(context, WiFi.localIP().toString().c_str());
	//return SCPI_RES_OK;
}

scpi_result_t SCPIManager::SCPI_NetworkGate (scpi_t *context)
{
	return SCPIManager::saveIpv4Address(context, &Settings::setWifiIpv4GatewayAddress);
}

scpi_result_t SCPIManager::SCPI_NetworkGateQ (scpi_t *context)
{
	SCPI_ResultMnemonic(context, getSettings(context)->getWifiIpv4GatewayAddress(true).toString().c_str());
	return SCPI_RES_OK;

	//SCPI_ResultMnemonic(context, WiFi.gatewayIP().toString().c_str());
	//return SCPI_RES_OK;
}

scpi_result_t SCPIManager::SCPI_NetworkSubnet (scpi_t *context)
{
	return SCPIManager::saveIpv4Address(context, &Settings::setWifiIpv4SubnetMask);
}

scpi_result_t SCPIManager::SCPI_NetworkSubnetQ (scpi_t *context)
{
	SCPI_ResultMnemonic(context, getSettings(context)->getWifiIpv4SubnetMask(true).toString().c_str());
	return SCPI_RES_OK;

	//SCPI_ResultMnemonic(context, WiFi.subnetMask().toString().c_str());
	//return SCPI_RES_OK;
}

scpi_result_t SCPIManager::SCPI_NetworkPort (scpi_t *context)
{
	return SCPIManager::saveNetworkPort(context, &Settings::setWifiIpv4SCPIServerPort);
}

scpi_result_t SCPIManager::SCPI_NetworkPortQ (scpi_t *context)
{
	SCPI_ResultUInt16(context, getSettings(context)->getWifiIpv4SCPIServerPort());
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::SCPI_NetworkHostnameQ (scpi_t *context)
{
	return SCPI_RES_ERR;
}

const char* SCPIManager::getMacAddress()
{
	mac_address = WiFi.macAddress();
	return mac_address.c_str();
}

char* SCPIManager::getBuildDate()
{
	return this->build_date;
}

scpi_result_t SCPIManager::SCPI_SocketPort (scpi_t *context)
{
	return SCPIManager::saveNetworkPort(context, &Settings::setWifiIpv4UdpLoggingServerPort);
}

scpi_result_t SCPIManager::SCPI_SocketPortQ (scpi_t *context)
{
	SCPI_ResultUInt16(context, getSettings(context)->getWifiIpv4UdpLoggingServerPort());
	return SCPI_RES_OK;
}

scpi_result_t SCPIManager::SCPI_SocketIPAddress(scpi_t *context)
{
	return SCPIManager::saveIpv4Address(context, &Settings::setWifiIpv4UdpLoggingServerIpAddress);
}

scpi_result_t SCPIManager::SCPI_SocketIPAddressQ(scpi_t *context)
{
	SCPI_ResultMnemonic(context, getSettings(context)->getWifiIpv4UdpLoggingServerIpAddress().toString().c_str());
	return SCPI_RES_OK;
}

Settings* SCPIManager::getSettings(scpi_t *context)
{
	return static_cast<UserContext *>(context->user_context)->settings;
}

scpi_result_t SCPIManager::saveIpv4Address(scpi_t *context, void (Settings::*func)(IPAddress, bool, bool))
{
	const char* value;
	uint len;
	IPAddress ipaddr_obj;
	char ipaddr[15];
	memset(ipaddr, 0x00, sizeof(ipaddr));

	if (!SCPI_ParamCharacters(context, &value, &len, TRUE)) {
		return SCPI_RES_ERR;
	}
	strncpy(ipaddr, value, len);
	if (ipaddr_obj.fromString(ipaddr)) {
		(getSettings(context)->* func)(ipaddr_obj, true, false);
		return SCPI_RES_OK;
	} else {
		//SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);  // full error list
		SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);  // full error list
		return SCPI_RES_ERR;
	}
	return SCPI_RES_ERR;
}

scpi_result_t SCPIManager::saveNetworkPort(scpi_t *context, void (Settings::*func)(uint16_t, bool, bool))
{
	scpi_number_t port;

	if (!SCPI_ParamNumber(context, scpi_special_numbers_def, &port, TRUE)) {
		return SCPI_RES_ERR;
	}

	if (0 <= port.content.value && port.content.value < 10000) {
		(getSettings(context)->* func)(port.content.value, true, false);
		return SCPI_RES_OK;
	} else {
		//SCPI_ErrorPush(context, SCPI_ERROR_DATA_OUT_OF_RANGE);  // full error list
		SCPI_ErrorPush(context, SCPI_ERROR_DATA_TYPE_ERROR);
		return SCPI_RES_ERR;
	}
}
