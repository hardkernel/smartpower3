#ifndef SMARTPOWER3_SETTINGS_H_
#define SMARTPOWER3_SETTINGS_H_

#include <Arduino.h>
#include <ArduinoNvs.h>
#include <WiFi.h>
#include <WiFiUdp.h>


enum wifi_credentials_state_e {
	WIFI_CREDENTIALS_STATE_OK = 0,
	WIFI_CREDENTIALS_STATE_INVALID = 1,
	WIFI_CREDENTIALS_STATE_NOT_CHECKED = 2,
};


/*
 * All user changeable settings belong here
 *
 */
class Settings
{
public:
	Settings();
	void init();
	uint8_t getBacklightLevelIndex(bool from_storage=false);
	void setBacklightLevelIndex(uint8_t backlightLevelIndex = 3, bool force_commit = true);
	const uint8_t* getBacklightLevelPreset () const;
	/*uint8_t getLogIntervalIndex(bool from_storage=false);
	void setLogIntervalIndex (uint8_t logIntervalIndex = 0);*/
	//const uint16_t* getLogIntervalPreset () const;
	uint8_t getLogInterval(bool from_storage=false);
	void setLogInterval(uint8_t log_interval, bool force_commit = true);
	bool isLoggingEnabled(bool from_storage = false);
	void setLoggingEnabled(bool loggingEnabled = false, bool force_commit = true);
	uint8_t getSerialBaudRateIndex(bool from_storage=false);
//	void setSerialBaudRateIndex (uint8_t serialBaudRateIndex = 0, bool force_commit = true);
	uint32_t getSerialBaudRate(bool from_storage=false);
	void setSerialBaudRate(uint32_t serialBaudRate, bool force_commit=true);
	String getWifiAccessPointSsid(bool from_storage=false);
	void setWifiAccessPointSsid(const char* wifiAccessPointSsid, bool force_commit=true);
	bool isWifiEnabled(bool from_storage = false);
	void setWifiEnabled(bool wifiEnabled = true, bool force_commit = true);
/*	const IPAddress& getWifiIpv4AddressDns1 () const;
	void setWifiIpv4AddressDns1 (const IPAddress &wifiIpv4AddressDns1);
	const IPAddress& getWifiIpv4AddressDns2 () const;
	void setWifiIpv4AddressDns2 (const IPAddress &wifiIpv4AddressDns2);*/
	bool isWifiIpv4ConnectAutomatically () const;
	void setWifiIpv4ConnectAutomatically (bool wifiIpv4ConnectAutomatically = true);
	bool isWifiIpv4DhcpEnabled () const;
	void setWifiIpv4DhcpEnabled (bool wifiIpv4DhcpEnabled = true);
/*	const IPAddress& getWifiIpv4GatewayAddress () const;
	void setWifiIpv4GatewayAddress (const IPAddress &wifiIpv4GatewayAddress);
	const IPAddress& getWifiIpv4StaticIp () const;
	void setWifiIpv4StaticIp (const IPAddress &wifiIpv4StaticIp);
	const IPAddress& getWifiIpv4SubnetMask () const;
	void setWifiIpv4SubnetMask (const IPAddress &wifiIpv4SubnetMask);*/
	IPAddress getWifiIpv4UdpLoggingServerIpAddress(bool from_storage = false);
	void setWifiIpv4UdpLoggingServerIpAddress(IPAddress wifiIpv4UdpLoggingServerIpAddress, bool force_commit = true);
	uint16_t getWifiIpv4UdpLoggingServerPort(bool from_storage);
	void setWifiIpv4UdpLoggingServerPort (uint16_t wifiIpv4UdpLoggingServerPort = 0, bool force_commit = true);
	String getWifiPassword(bool from_storage=false);
	void setWifiPassword(const char* wifiPassword, bool force_commit = true);
	bool isWifiUseIpv4 () const;
	void setWifiUseIpv4 (bool wifiUseIpv4 = true);
	bool isWifiUseIpv6 () const;
	void setWifiUseIpv6 (bool wifiUseIpv6 = false);
	bool isFirstBoot(bool from_storage = false);
	void setFirstBoot(bool firstBoot = false, bool force_commit = true);
	wifi_credentials_state_e getWifiCredentialsState(bool from_storage = false);
	void setWifiCredentialsState(wifi_credentials_state_e wifiCredentialsStateSettings, bool force_commit = false);
	uint16_t getChannel0CurrentLimit(bool from_storage = false);
	void setChannel0CurrentLimit(uint16_t channel0CurrentLimit, bool force_commit = true);
	uint16_t getChannel0Voltage(bool from_storage = false);
	void setChannel0Voltage (uint16_t channel0Voltage, bool force_commit = true);
	uint16_t getChannel1CurrentLimit(bool from_storage = false);
	void setChannel1CurrentLimit (uint16_t channel1CurrentLimit, bool force_commit = true);
	uint16_t getChannel1Voltage(bool from_storage = false);
	void setChannel1Voltage (uint16_t channel1Voltage, bool force_commit = true);

private:
	bool first_boot = false;
	// Power settings
	uint16_t channel_0_voltage = 5000;  // 5.0V in millivolts, default when first boot
	uint16_t channel_0_current_limit = 3000;  // 3.0A in milliamperes, default when first boot
	uint16_t channel_1_voltage = 5000;  // 5.0V in millivolts, default when first boot;
	uint16_t channel_1_current_limit = 3000;  // 3.0A in milliamperes, default when first boot
	// Backlight level
	uint8_t backlight_level_index = 0;
	uint8_t backlight_level_preset[7] = {10, 25, 50, 75, 100, 125, 150};
	// Serial baud rate
	//uint8_t serial_baud_rate_index = 0;
	uint32_t serial_baud_rate_preset[10] = {9600, 19200, 38400, 57600, 115200, 230400, 460800, 500000, 576000, 921600};
	uint32_t serial_baud_rate = 115200;
	// Logging related
	bool logging_enabled = false;
	//uint8_t log_interval_index = 0;
	uint16_t log_interval_preset[7] = {0, 5, 10, 50, 100, 500, 1000};
	uint8_t log_interval = 0;
	// WiFI related
	bool wifi_enabled = true;
/*	bool wifi_use_ipv4 = true;
	bool wifi_use_ipv6 = false;*/
	String wifi_access_point_ssid;
	String wifi_password;
	wifi_credentials_state_e wifi_credentials_state = WIFI_CREDENTIALS_STATE_OK;
	bool wifi_ipv4_connect_automatically = true;
	bool wifi_ipv4_dhcp_enabled = true;
/*	IPAddress wifi_ipv4_static_ip;
	IPAddress wifi_ipv4_gateway_address;
	IPAddress wifi_ipv4_subnet_mask;
	IPAddress wifi_ipv4_address_dns_1;
	IPAddress wifi_ipv4_address_dns_2;*/
	IPAddress wifi_ipv4_udp_logging_server_ip_address;
	uint16_t wifi_ipv4_udp_logging_server_port = 0;
};

#endif /* SMARTPOWER3_SETTINGS_H_ */
