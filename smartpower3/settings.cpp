#include "settings.h"

Settings::Settings()
{
	NVS.begin();
	delay(100);  // wait till NVS initializes, otherwise the init may block indefinetly
	// reserve space for Strings to minimize fragmentation
	this->wifi_access_point_ssid.reserve(256);
	this->wifi_password.reserve(64);  // the longes password currently possible
}

void Settings::init()
{
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
	//this->wifi_use_ipv4 = true;
	//bool wifi_use_ipv6 = false;
	this->wifi_access_point_ssid = this->getWifiAccessPointSsid(true);
	this->wifi_password = this->getWifiPassword(true);
//TODO: add settings/checking CREDENTIALS_STATE
	/*this->wifi_credentials_checked = this->isWifiCredentialsChecked();
	this->wifi_ipv4_connect_automatically = true;
	this->wifi_ipv4_dhcp_enabled = true;*/
	//IPAddress wifi_ipv4_static_ip;
	//IPAddress wifi_ipv4_gateway_address;
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
}

uint8_t Settings::getBacklightLevelIndex(bool from_storage)
{
	return (from_storage) ? NVS.getInt("bl_level", backlight_level_index) : backlight_level_index;
}

void Settings::setBacklightLevelIndex(uint8_t backlightLevelIndex, bool force_commit)
{
	backlight_level_index = backlightLevelIndex;
	NVS.setInt("bl_level", backlight_level_index, force_commit);
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
	return (from_storage) ? NVS.getInt("log_interval", log_interval) : log_interval;
}

void Settings::setLogInterval(uint8_t logInterval, bool force_commit)
{
	log_interval = logInterval;
	NVS.setInt("log_interval", logInterval, force_commit);
}

bool Settings::isLoggingEnabled(bool from_storage)
{
	return (from_storage) ? NVS.getInt("logging_enabled", logging_enabled) : logging_enabled;
}

void Settings::setLoggingEnabled(bool loggingEnabled, bool force_commit)
{
	logging_enabled = loggingEnabled;
	NVS.setInt("logging_enabled", logging_enabled, force_commit);
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
	return (from_storage) ? NVS.getInt("serial_baud", serial_baud_rate) : serial_baud_rate;
}

void Settings::setSerialBaudRate(uint32_t serialBaudRate, bool force_commit)
{
	serial_baud_rate = serialBaudRate;
	NVS.setInt("serial_baud", serial_baud_rate, force_commit);
}

String Settings::getWifiAccessPointSsid(bool from_storage)
{
	if (from_storage) {
		NVS.getString("ssid", wifi_access_point_ssid);
	}
	return wifi_access_point_ssid;
}

void Settings::setWifiAccessPointSsid(const char* wifiAccessPointSsid, bool force_commit)
{
	wifi_access_point_ssid = wifiAccessPointSsid;
	NVS.setString("ssid", wifi_access_point_ssid, force_commit);
}

bool Settings::isWifiEnabled(bool from_storage)
{
	return (from_storage) ? NVS.getInt("wifi_enabled", wifi_enabled) : wifi_enabled;
}

void Settings::setWifiEnabled(bool wifiEnabled, bool force_commit)
{
	wifi_enabled = wifiEnabled;
	NVS.setInt("wifi_enabled", wifi_enabled, force_commit);
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

bool Settings::isWifiIpv4DhcpEnabled () const
{
	return wifi_ipv4_dhcp_enabled;
}

void Settings::setWifiIpv4DhcpEnabled (bool wifiIpv4DhcpEnabled)
{
	wifi_ipv4_dhcp_enabled = wifiIpv4DhcpEnabled;
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
	IPAddress ipaddress;
	if (from_storage) {
		ipaddress.fromString(NVS.getString("ipaddr_udp"));
		return ipaddress;
	}
	return wifi_ipv4_udp_logging_server_ip_address;
}

void Settings::setWifiIpv4UdpLoggingServerIpAddress(IPAddress wifiIpv4UdpLoggingServerIpAddress, bool force_commit)
{
	wifi_ipv4_udp_logging_server_ip_address = wifiIpv4UdpLoggingServerIpAddress;
	NVS.setString("ipaddr_udp", wifi_ipv4_udp_logging_server_ip_address.toString(), force_commit);
}

uint16_t Settings::getWifiIpv4UdpLoggingServerPort(bool from_storage)
{
	return (from_storage) ?
			NVS.getInt("port_udp", wifi_ipv4_udp_logging_server_port)
			: wifi_ipv4_udp_logging_server_port;
}

void Settings::setWifiIpv4UdpLoggingServerPort (uint16_t wifiIpv4UdpLoggingServerPort, bool force_commit)
{
	wifi_ipv4_udp_logging_server_port = wifiIpv4UdpLoggingServerPort;
	NVS.setInt("port_udp", wifi_ipv4_udp_logging_server_port, force_commit);
}

String Settings::getWifiPassword(bool from_storage)
{
	if (from_storage) {
		NVS.getString("passwd", wifi_password);
	}
	return wifi_password;
}

void Settings::setWifiPassword(const char* wifiPassword, bool force_commit)
{
	wifi_password = wifiPassword;
	NVS.setString("passwd", wifi_password);
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
	return (from_storage) ? NVS.getInt("firstboot", first_boot) : first_boot;
}

void Settings::setFirstBoot(bool firstBoot, bool force_commit)
{
	first_boot = firstBoot;
	NVS.setInt("firstboot", first_boot, force_commit);
}

wifi_credentials_state_e Settings::getWifiCredentialsState(bool from_storage)
{
	return (from_storage)
			? (wifi_credentials_state_e) NVS.getInt("wifi_conn_ok", wifi_credentials_state)
			: wifi_credentials_state;
}

void Settings::setWifiCredentialsState(wifi_credentials_state_e wifiCredentialsStateSettings, bool force_commit)
{
	wifi_credentials_state = wifiCredentialsStateSettings;
	NVS.setInt("wifi_conn_ok", wifi_credentials_state, force_commit);
}

uint16_t Settings::getChannel0CurrentLimit(bool from_storage)
{
	return (from_storage) ? NVS.getInt("current_limit0", channel_0_current_limit) : channel_0_current_limit;
}

void Settings::setChannel0CurrentLimit(uint16_t channel0CurrentLimit, bool force_commit)
{
	channel_0_current_limit = channel0CurrentLimit;
	NVS.setInt("current_limit0", channel_0_current_limit, force_commit);
}

uint16_t Settings::getChannel0Voltage(bool from_storage)
{
	return (from_storage) ? NVS.getInt("voltage0", channel_0_voltage) : channel_0_voltage;
}

void Settings::setChannel0Voltage(uint16_t channel0Voltage, bool force_commit)
{
	channel_0_voltage = channel0Voltage;
	NVS.setInt("voltage0", channel_0_voltage, force_commit);
}

uint16_t Settings::getChannel1CurrentLimit(bool from_storage)
{
	return (from_storage) ? NVS.getInt("current_limit1", channel_1_current_limit) : channel_1_current_limit;
}

void Settings::setChannel1CurrentLimit(uint16_t channel1CurrentLimit, bool force_commit)
{
	channel_1_current_limit = channel1CurrentLimit;
	NVS.setInt("current_limit1", channel_1_current_limit, force_commit);
}

uint16_t Settings::getChannel1Voltage(bool from_storage)
{
	return (from_storage) ? NVS.getInt("voltage1", channel_1_voltage) : channel_1_voltage;
}

void Settings::setChannel1Voltage(uint16_t channel1Voltage, bool force_commit)
{
	channel_1_voltage = channel1Voltage;
	NVS.setInt("voltage1", channel_1_voltage, force_commit);
}
