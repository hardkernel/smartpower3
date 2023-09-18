#ifndef SMARTPOWER3_SETTINGS_H_
#define SMARTPOWER3_SETTINGS_H_

#include <Arduino.h>
#include <Preferences.h>
#include <WiFi.h>
#include <WiFiUdp.h>


ESP_EVENT_DECLARE_BASE(SETTINGS_EVENTS);

enum {
    SETTINGS_VOLTAGE0_CHANGED_EVENT,
	SETTINGS_CURRENT0_CHANGED_EVENT,
	SETTINGS_VOLTAGE1_CHANGED_EVENT,
	SETTINGS_CURRENT1_CHANGED_EVENT,
	SETTINGS_LOGGING_PORT_CHANGED_EVENT,
	SETTINGS_LOGGING_ADDRESS_CHANGED_EVENT,
	SETTINGS_LOGGING_INTERVAL_CHANGED_EVENT,
	SETTINGS_SCPI_PORT_CHANGED_EVENT,
	SETTINGS_OPERATION_MODE_CHANGED_EVENT,
};

enum wifi_credentials_state_e {
	WIFI_CREDENTIALS_STATE_OK = 0,
	WIFI_CREDENTIALS_STATE_INVALID = 1,
	WIFI_CREDENTIALS_STATE_NOT_CHECKED = 2,
};

enum device_operation_mode {
	OPERATION_MODE_DEFAULT,
	OPERATION_MODE_SCPI,
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
	uint8_t getBacklightLevelIndex(bool from_storage = false);
	void setBacklightLevelIndex(uint8_t backlightLevelIndex = 3);
	uint8_t getLogInterval(bool from_storage = false);
	void setLogInterval(uint8_t log_interval, bool set_through_settings = false);
	uint16_t getLogIntervalPreset(bool from_storage = false);
	uint16_t getLogIntervalPreset(uint8_t log_interval);
	uint8_t checkLogLevel(uint8_t log_interval, uint32_t serial_baud);
	int8_t getLogIntervalIndexFromPreset(uint16_t preset);
	bool isNotLoggingPresetValue(uint16_t value);
	bool isLoggingEnabled(bool from_storage = false);
	void setLoggingEnabled(bool loggingEnabled = false);
	void switchLogging(bool from_storage = false);
	uint8_t getSerialBaudRateIndex(bool from_storage = false);
	uint32_t getSerialBaudRate(bool from_storage = false);
	void setSerialBaudRate(uint32_t serialBaudRate);
	String getWifiAccessPointSsid(bool from_storage = false);
	void setWifiAccessPointSsid(const char *wifiAccessPointSsid);
	bool isWifiEnabled(bool from_storage = false);
	void setWifiEnabled(bool wifiEnabled = true);
	void switchWifi(bool from_storage = false);

	IPAddress getWifiIpv4AddressDns1(bool from_storage = false);
	void setWifiIpv4AddressDns1(IPAddress wifiIpv4AddressDns1, bool set_through_settings = false);
	IPAddress getWifiIpv4AddressDns2(bool from_storage = false);
	void setWifiIpv4AddressDns2(IPAddress wifiIpv4AddressDns2, bool set_through_settings = false);

	void setWifiIpv4ConnectAutomatically(bool wifiIpv4ConnectAutomatically = true);
	bool isWifiIpv4DhcpEnabled(bool from_storage = false);
	void setWifiIpv4DhcpEnabled(bool wifiIpv4DhcpEnabled = true);
	IPAddress getWifiIpv4GatewayAddress(bool from_storage = false);
	void setWifiIpv4GatewayAddress(IPAddress wifiIpv4GatewayAddress, bool set_through_settings = false);
	IPAddress getWifiIpv4StaticIp(bool from_storage = false);
	void setWifiIpv4StaticIp(IPAddress wifiIpv4StaticIp, bool set_through_settings = false);
	IPAddress getWifiIpv4SubnetMask(bool from_storage = false);
	void setWifiIpv4SubnetMask(IPAddress wifiIpv4SubnetMask, bool set_through_settings = false);
	IPAddress getWifiIpv4UdpLoggingServerIpAddress(bool from_storage = false);
	void setWifiIpv4UdpLoggingServerIpAddress(IPAddress wifiIpv4UdpLoggingServerIpAddress,
											  bool set_through_settings = false);
	uint16_t getWifiIpv4UdpLoggingServerPort(bool from_storage = false);
	void setWifiIpv4UdpLoggingServerPort(uint16_t wifiIpv4UdpLoggingServerPort = 0, bool set_through_settings = false);
	String getWifiPassword(bool from_storage = false);
	void setWifiPassword(const char *wifiPassword);
	bool isFirstBoot(bool from_storage = false);
	void setFirstBoot(bool firstBoot = false);
	wifi_credentials_state_e getWifiCredentialsState(bool from_storage = false);
	void setWifiCredentialsState(wifi_credentials_state_e wifiCredentialsStateSettings);
	uint16_t getChannel0CurrentLimit(bool from_storage = false);
	void setChannel0CurrentLimit(uint16_t channel0CurrentLimit, bool set_through_settings = false);
	uint16_t getChannel0Voltage(bool from_storage = false);
	void setChannel0Voltage(uint16_t channel0Voltage, bool set_through_settings = false);
	uint16_t getChannel1CurrentLimit(bool from_storage = false);
	void setChannel1CurrentLimit(uint16_t channel1CurrentLimit, bool set_through_settings = false);
	uint16_t getChannel1Voltage(bool from_storage = false);
	void setChannel1Voltage(uint16_t channel1Voltage, bool set_through_settings = false);
	//helper methods
	bool isNvsCleared(bool from_storage = false);
	void setNvsCleared(bool nvsCleared = false);
	esp_event_loop_handle_t& getEventLoopHandleAddress(void);
	String getMacAddress(void);
	void setMacAddress(String macAddress);
	device_operation_mode getOperationMode(bool from_storage = false);
	void setOperationMode(device_operation_mode operationMode = OPERATION_MODE_DEFAULT,
						  bool set_through_settings = false);
	bool isScpiSerialLoggingEnabled(void);
	void setScpiSerialLoggingEnabled(bool scpiSerialLoggingEnabled = false);
	bool isScpiSocketLoggingEnabled(void);
	void setScpiSocketLoggingEnabled(bool scpiSocketLoggingEnabled = false);

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
	uint32_t serial_baud_rate_preset[10] = {9600, 19200, 38400, 57600, 115200, 230400, 460800, 500000, 576000, 921600};
	uint32_t serial_baud_rate = 115200;
	// Logging related
	bool logging_enabled = false;
	uint16_t log_interval_preset[7] = {0, 5, 10, 50, 100, 500, 1000};
	uint8_t log_interval = 0;
	// WiFI related
	String mac_address;
	bool wifi_enabled = true;
	String wifi_access_point_ssid = "";
	String wifi_password = "";
	wifi_credentials_state_e wifi_credentials_state = WIFI_CREDENTIALS_STATE_OK;
	bool wifi_ipv4_connect_automatically = true;
	bool wifi_ipv4_dhcp_enabled = true;
	IPAddress wifi_ipv4_static_ip;
	IPAddress wifi_ipv4_gateway_address;
	IPAddress wifi_ipv4_subnet_mask;
	IPAddress wifi_ipv4_address_dns_1;
	IPAddress wifi_ipv4_address_dns_2;
	IPAddress wifi_ipv4_udp_logging_server_ip_address = IPAddress(0, 0, 0, 0);
	uint16_t wifi_ipv4_udp_logging_server_port = 0;
	uint16_t wifi_ipv4_SCPI_server_port = 0;
	device_operation_mode operation_mode = OPERATION_MODE_DEFAULT;
	Preferences preferences;
	bool nvs_cleared = false;
	bool scpi_socket_logging_enabled = false;
	bool scpi_serial_logging_enabled = false;
	IPAddress getSettingIPAddress(bool from_storage, const char* setting_key, IPAddress* address_variable);
};

#endif /* SMARTPOWER3_SETTINGS_H_ */
