#include "settings.h"

Settings::Settings()
{
	// reserve space for Strings to minimize fragmentation
	this->wifi_access_point_ssid.reserve(256);
	this->wifi_password.reserve(64);  // the longes password currently possible
	this->mac_address.reserve(17);
}

void Settings::init()
{
	  // "storage" is default ArduinoNVS namespace, default nvs partition (ArduinoNVS used the first suitable partition)
	preferences.begin("storage", false);
	delay(100);  // wait till storage initializes, otherwise the init may block indefinetly

	// Clear old keys
	if (!this->isNvsCleared(true)) {
		Serial.println("About to clear preferences");
		preferences.clear();
		Serial.println("Preferences cleared");
		this->setNvsCleared(true);
	}

	// Backlight level
	this->backlight_level_index = this->getBacklightLevelIndex(true);
	// Serial baud rate
	this->serial_baud_rate = this->getSerialBaudRate(true);
	// Logging related
	this->logging_enabled = this->isLoggingEnabled(true);
	//this->log_interval_index = this->getLogIntervalIndex(true);
	this->log_interval = this->getLogInterval(true);
	// WiFI related
	this->wifi_enabled = this->isWifiEnabled(true);
	this->mac_address = this->getMacAddress();
	//this->wifi_use_ipv4 = true;
	//bool wifi_use_ipv6 = false;
	this->wifi_access_point_ssid = this->getWifiAccessPointSsid(true);
	this->wifi_password = this->getWifiPassword(true);
//TODO: add settings/checking CREDENTIALS_STATE
	/*this->wifi_credentials_checked = this->isWifiCredentialsChecked();
	this->wifi_ipv4_connect_automatically = true;*/
	this->wifi_ipv4_dhcp_enabled = true;
	this->wifi_ipv4_static_ip = this->getWifiIpv4StaticIp(true);
	//IPAddress wifi_ipv4_gateway_address;
	this->wifi_ipv4_gateway_address = this->getWifiIpv4GatewayAddress(true);
	//IPAddress wifi_ipv4_subnet_mask;
	//IPAddress wifi_ipv4_address_dns_1;
	//IPAddress wifi_ipv4_address_dns_2;
	this->wifi_ipv4_udp_logging_server_ip_address = this->getWifiIpv4UdpLoggingServerIpAddress(true);
	this->wifi_ipv4_udp_logging_server_port = this->getWifiIpv4UdpLoggingServerPort(true);
	// Various
	this->first_boot = this->isFirstBoot(true);
	this->wifi_credentials_state = this->getWifiCredentialsState(true);
	// Channel voltage and current limit
	this->channel_0_voltage = this->getChannel0Voltage(true);
	this->channel_0_current_limit = this->getChannel0CurrentLimit(true);
	this->channel_1_voltage = this->getChannel1Voltage(true);
	this->channel_1_current_limit = this->getChannel1CurrentLimit(true);
	this->wifi_ipv4_SCPI_server_port = this->getWifiIpv4SCPIServerPort(true);
}

uint8_t Settings::getBacklightLevelIndex(bool from_storage)
{
	return (from_storage) ? preferences.getUChar("bl_level", backlight_level_index) : backlight_level_index;
}

void Settings::setBacklightLevelIndex(uint8_t backlightLevelIndex, bool force_commit)
{
	backlight_level_index = backlightLevelIndex;
	preferences.putUChar("bl_level", backlight_level_index);
}

const uint8_t* Settings::getBacklightLevelPreset () const
{
	return backlight_level_preset;
}

/*uint8_t Settings::getLogIntervalIndex(bool from_storage)
{
	return log_interval_index;
}

void Settings::setLogIntervalIndex (uint8_t logIntervalIndex)
{
	log_interval_index = logIntervalIndex;
}
*/
/*
const uint16_t* Settings::getLogIntervalPreset () const
{
	return log_interval_preset;
}
*/

uint8_t Settings::getLogInterval(bool from_storage)
{
	return (from_storage) ? preferences.getUChar("log_interval", log_interval) : log_interval;
}

void Settings::setLogInterval(uint8_t logInterval, bool force_commit)
{
	log_interval = logInterval;
	preferences.putUChar("log_interval", logInterval);
}

bool Settings::isLoggingEnabled(bool from_storage)
{
	return (from_storage) ? preferences.getBool("logging_enabled", logging_enabled) : logging_enabled;
}

void Settings::setLoggingEnabled(bool loggingEnabled, bool force_commit)
{
	logging_enabled = loggingEnabled;
	preferences.putBool("logging_enabled", logging_enabled);
}

void Settings::switchLogging(bool from_storage)
{
	if (this->isLoggingEnabled(from_storage)) {
		this->setLoggingEnabled(false);
	} else {
		this->setLoggingEnabled(true);
	}
}

uint8_t Settings::getSerialBaudRateIndex(bool from_storage)
{
	uint32_t baud_rate;

	if (from_storage) {
		baud_rate = this->getSerialBaudRate(from_storage);
	} else {
		baud_rate = this->serial_baud_rate;
	}
	for (int i = 0; i < sizeof(serial_baud_rate_preset); i++) {
		if (serial_baud_rate_preset[i] >= baud_rate) {
			return i;
		}
	}
	return 0;
}

/*void Settings::setSerialBaudRateIndex (uint8_t serialBaudRateIndex, bool force_commit)
{
	serial_baud_rate_index = serialBaudRateIndex;
	NVS.setInt("serial_baud", serial_baud_rate_index, force_commit);
}*/

uint32_t Settings::getSerialBaudRate(bool from_storage)
{
	return (from_storage) ? preferences.getUInt("serial_baud", serial_baud_rate) : serial_baud_rate;
}

void Settings::setSerialBaudRate(uint32_t serialBaudRate, bool force_commit)
{
	serial_baud_rate = serialBaudRate;
	preferences.putUInt("serial_baud", serial_baud_rate);
}

String Settings::getWifiAccessPointSsid(bool from_storage)
{
	if (from_storage && preferences.isKey("ssid")) {
		wifi_access_point_ssid = preferences.getString("ssid");
	}
	return wifi_access_point_ssid;
}

void Settings::setWifiAccessPointSsid(const char* wifiAccessPointSsid, bool force_commit)
{
	wifi_access_point_ssid = wifiAccessPointSsid;
	preferences.putString("ssid", wifiAccessPointSsid);
}

bool Settings::isWifiEnabled(bool from_storage)
{
	return (from_storage) ? preferences.getBool("wifi_enabled", wifi_enabled) : wifi_enabled;
}

void Settings::setWifiEnabled(bool wifiEnabled, bool force_commit)
{
	wifi_enabled = wifiEnabled;
	preferences.putBool("wifi_enabled", wifi_enabled);
}

void Settings::switchWifi(bool from_storage)
{
	if (this->isWifiEnabled(from_storage)) {
		this->setWifiEnabled(false);
	} else {
		this->setWifiEnabled(true);
	}
}

/*const IPAddress& Settings::getWifiIpv4AddressDns1 () const
{
	return wifi_ipv4_address_dns_1;
}*/

/*void Settings::setWifiIpv4AddressDns1 (const IPAddress &wifiIpv4AddressDns1)
{
	wifi_ipv4_address_dns_1 = wifiIpv4AddressDns1;
}

const IPAddress& Settings::getWifiIpv4AddressDns2 () const
{
	return wifi_ipv4_address_dns_2;
}

void Settings::setWifiIpv4AddressDns2 (const IPAddress &wifiIpv4AddressDns2)
{
	wifi_ipv4_address_dns_2 = wifiIpv4AddressDns2;
}*/

bool Settings::isWifiIpv4ConnectAutomatically () const
{
	return wifi_ipv4_connect_automatically;
}

void Settings::setWifiIpv4ConnectAutomatically (bool wifiIpv4ConnectAutomatically)
{
	wifi_ipv4_connect_automatically = wifiIpv4ConnectAutomatically;
}

bool Settings::isWifiIpv4DhcpEnabled()
{
	return wifi_ipv4_dhcp_enabled;
}

void Settings::setWifiIpv4DhcpEnabled(bool wifiIpv4DhcpEnabled)
{
	wifi_ipv4_dhcp_enabled = wifiIpv4DhcpEnabled;
	//preferences.putBool("dhcp_enabled", wifi_ipv4_dhcp_enabled);
}

/*const IPAddress& Settings::getWifiIpv4GatewayAddress () const
{
	return wifi_ipv4_gateway_address;
}

void Settings::setWifiIpv4GatewayAddress (const IPAddress &wifiIpv4GatewayAddress)
{
	wifi_ipv4_gateway_address = wifiIpv4GatewayAddress;
}

const IPAddress& Settings::getWifiIpv4StaticIp () const
{
	return wifi_ipv4_static_ip;
}

void Settings::setWifiIpv4StaticIp (const IPAddress &wifiIpv4StaticIp)
{
	wifi_ipv4_static_ip = wifiIpv4StaticIp;
}

const IPAddress& Settings::getWifiIpv4SubnetMask () const
{
	return wifi_ipv4_subnet_mask;
}

void Settings::setWifiIpv4SubnetMask (const IPAddress &wifiIpv4SubnetMask)
{
	wifi_ipv4_subnet_mask = wifiIpv4SubnetMask;
}*/

IPAddress Settings::getWifiIpv4UdpLoggingServerIpAddress(bool from_storage)
{
	return this->getSettingIPAddress(from_storage, "ipaddr_udp", &wifi_ipv4_udp_logging_server_ip_address);
/*	IPAddress ipaddress;
	if (from_storage && preferences.isKey("ipaddr_udp")) {
		ipaddress.fromString(preferences.getString("ipaddr_udp"));
		return ipaddress;
	}
	return wifi_ipv4_udp_logging_server_ip_address;*/
}

void Settings::setWifiIpv4UdpLoggingServerIpAddress(IPAddress wifiIpv4UdpLoggingServerIpAddress, bool set_through_settings, bool force_commit)
{
	wifi_ipv4_udp_logging_server_ip_address = wifiIpv4UdpLoggingServerIpAddress;
	preferences.putString("ipaddr_udp", wifi_ipv4_udp_logging_server_ip_address.toString());
	if (set_through_settings) {
		esp_event_post_to((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS,
						  SETTINGS_LOGGING_ADDRESS_CHANGED_EVENT, NULL, sizeof(NULL), portMAX_DELAY);
	}
}

uint16_t Settings::getWifiIpv4UdpLoggingServerPort(bool from_storage)
{
	return (from_storage) ?
			preferences.getUShort("port_udp", wifi_ipv4_udp_logging_server_port)
			: wifi_ipv4_udp_logging_server_port;
}

void Settings::setWifiIpv4UdpLoggingServerPort(uint16_t wifiIpv4UdpLoggingServerPort, bool set_through_settings, bool force_commit)
{
	wifi_ipv4_udp_logging_server_port = wifiIpv4UdpLoggingServerPort;
	preferences.putUShort("port_udp", wifi_ipv4_udp_logging_server_port);
	delay(100);
	if (set_through_settings) {
		esp_event_post_to((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS,
						  SETTINGS_LOGGING_PORT_CHANGED_EVENT, NULL, sizeof(NULL), portMAX_DELAY);
	}
}

String Settings::getWifiPassword(bool from_storage)
{
	if (from_storage && preferences.isKey("passwd")) {
		wifi_password = preferences.getString("passwd");
	}
	return wifi_password;
}

void Settings::setWifiPassword(const char* wifiPassword, bool force_commit)
{
	wifi_password = wifiPassword;
	preferences.putString("passwd", wifiPassword);
}

/*bool Settings::isWifiUseIpv4 () const
{
	return wifi_use_ipv4;
}

void Settings::setWifiUseIpv4 (bool wifiUseIpv4)
{
	wifi_use_ipv4 = wifiUseIpv4;
}

bool Settings::isWifiUseIpv6 () const
{
	return wifi_use_ipv6;
}

void Settings::setWifiUseIpv6 (bool wifiUseIpv6)
{
	wifi_use_ipv6 = wifiUseIpv6;
}*/

bool Settings::isFirstBoot(bool from_storage)
{
	return (from_storage) ? preferences.getBool("firstboot", first_boot) : first_boot;
}

void Settings::setFirstBoot(bool firstBoot, bool force_commit)
{
	first_boot = firstBoot;
	preferences.putBool("firstboot", first_boot);
}

wifi_credentials_state_e Settings::getWifiCredentialsState(bool from_storage)
{
	return (from_storage)
			? static_cast<wifi_credentials_state_e>(preferences.getBool("wifi_conn_ok", wifi_credentials_state))
			: wifi_credentials_state;
}

void Settings::setWifiCredentialsState(wifi_credentials_state_e wifiCredentialsStateSettings, bool force_commit)
{
	wifi_credentials_state = wifiCredentialsStateSettings;
	preferences.putBool("wifi_conn_ok", wifi_credentials_state);
}

uint16_t Settings::getChannel0CurrentLimit(bool from_storage)
{
	return (from_storage) ? preferences.getUShort("current_limit0", channel_0_current_limit) : channel_0_current_limit;
}

void Settings::setChannel0CurrentLimit(uint16_t channel0CurrentLimit, bool set_through_settings, bool force_commit)
{
	channel_0_current_limit = channel0CurrentLimit;
	preferences.putUShort("current_limit0", channel_0_current_limit);
	if (set_through_settings) {
		esp_event_post_to((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS, SETTINGS_CURRENT0_CHANGED_EVENT, NULL, sizeof(NULL), portMAX_DELAY);
	}
}

uint16_t Settings::getChannel0Voltage(bool from_storage)
{
	return (from_storage) ? preferences.getUShort("voltage0", channel_0_voltage) : channel_0_voltage;
}

void Settings::setChannel0Voltage(uint16_t channel0Voltage, bool set_through_settings, bool force_commit)
{
	channel_0_voltage = channel0Voltage;
	preferences.putUShort("voltage0", channel_0_voltage);
	delay(100);
	if (set_through_settings) {
		esp_event_post_to((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS, SETTINGS_VOLTAGE0_CHANGED_EVENT, NULL, sizeof(NULL), portMAX_DELAY);
	}
}

uint16_t Settings::getChannel1CurrentLimit(bool from_storage)
{
	return (from_storage) ? preferences.getUShort("current_limit1", channel_1_current_limit) : channel_1_current_limit;
}

void Settings::setChannel1CurrentLimit(uint16_t channel1CurrentLimit, bool set_through_settings, bool force_commit)
{
	channel_1_current_limit = channel1CurrentLimit;
	preferences.putUShort("current_limit1", channel_1_current_limit);
	if (set_through_settings) {
		esp_event_post_to((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS, SETTINGS_CURRENT1_CHANGED_EVENT, NULL, sizeof(NULL), portMAX_DELAY);
	}
}

uint16_t Settings::getChannel1Voltage(bool from_storage)
{
	return (from_storage) ? preferences.getUShort("voltage1", channel_1_voltage) : channel_1_voltage;
}

void Settings::setChannel1Voltage(uint16_t channel1Voltage, bool set_through_settings, bool force_commit)
{
	channel_1_voltage = channel1Voltage;
	preferences.putUShort("voltage1", channel_1_voltage);
	delay(100);
	if (set_through_settings) {
		esp_event_post_to((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS, SETTINGS_VOLTAGE1_CHANGED_EVENT, NULL, sizeof(NULL), portMAX_DELAY);
	}
}

bool Settings::isNvsCleared(bool from_storage)
{
	return (from_storage) ? preferences.getBool("nvs_cleared", false) : nvs_cleared;
}

void Settings::setNvsCleared(bool nvsCleared)
{
	nvs_cleared = nvsCleared;
	preferences.putBool("nvs_cleared", nvsCleared);
}

esp_event_loop_handle_t& Settings::getEventLoopHandleAddress()
{
	return loop_with_task;
}

String Settings::getMacAddress()
{
	mac_address = WiFi.macAddress();
	return this->mac_address;
}

void Settings::setMacAddress(String macAddress)
{
	mac_address = macAddress;
}

IPAddress Settings::getWifiIpv4GatewayAddress(bool from_storage)
{
	IPAddress ipaddress;
	if (from_storage && preferences.isKey("ipaddr_gate")) {
		ipaddress.fromString(preferences.getString("ipaddr_gate"));
		return ipaddress;
	}
	return wifi_ipv4_gateway_address;
}

void Settings::setWifiIpv4GatewayAddress (IPAddress wifiIpv4GatewayAddress, bool set_from_settings, bool force_commit)
{
	wifi_ipv4_gateway_address = wifiIpv4GatewayAddress;
	preferences.putString("ipaddr_gate", wifi_ipv4_gateway_address.toString());
}

IPAddress Settings::getWifiIpv4StaticIp(bool from_storage)
{
	IPAddress ipaddress;
	if (from_storage && preferences.isKey("ipaddr_static")) {
		ipaddress.fromString(preferences.getString("ipaddr_static"));
		return ipaddress;
	}
	return wifi_ipv4_static_ip;
}

void Settings::setWifiIpv4StaticIp (IPAddress wifiIpv4StaticIp, bool set_from_settings, bool force_commit)
{
	wifi_ipv4_static_ip = wifiIpv4StaticIp;
	preferences.putString("ipaddr_static", wifi_ipv4_static_ip.toString());
}

IPAddress Settings::getWifiIpv4SubnetMask (bool from_storage)
{
	return this->getSettingIPAddress(from_storage, "subnet", &wifi_ipv4_subnet_mask);
/*	IPAddress ipaddress;
	if (from_storage && preferences.isKey("subnet")) {
		ipaddress.fromString(preferences.getString("subnet"));
		return ipaddress;
	}
	return wifi_ipv4_subnet_mask;*/
}

void Settings::setWifiIpv4SubnetMask(IPAddress wifiIpv4SubnetMask, bool set_through_settings, bool force_commit)
{
	wifi_ipv4_subnet_mask = wifiIpv4SubnetMask;
	preferences.putString("subnet", wifi_ipv4_subnet_mask.toString());
}

IPAddress Settings::getSettingIPAddress(bool from_storage, const char *setting_key, IPAddress *address_variable)
{
	IPAddress ipaddress;
	if (from_storage && preferences.isKey(setting_key)) {
		ipaddress.fromString(preferences.getString(setting_key));
		return ipaddress;
	}
	return *address_variable;
}

uint16_t Settings::getWifiIpv4SCPIServerPort(bool from_storage)
{
	return (from_storage) ?
			preferences.getUShort("port_scpi", wifi_ipv4_SCPI_server_port)
			: wifi_ipv4_SCPI_server_port;
}

void Settings::setWifiIpv4SCPIServerPort(uint16_t wifiIpv4SCPIServerPort, bool set_through_settings,
											bool force_commit)
{
	wifi_ipv4_udp_logging_server_port = wifiIpv4SCPIServerPort;
	preferences.putUShort("port_scpi", wifi_ipv4_SCPI_server_port);
	delay(100);
	if (set_through_settings) {
		esp_event_post_to((esp_event_loop_handle_t)loop_with_task, SETTINGS_EVENTS,
						  SETTINGS_SCPI_PORT_CHANGED_EVENT, NULL, sizeof(NULL), portMAX_DELAY);
	}
}
