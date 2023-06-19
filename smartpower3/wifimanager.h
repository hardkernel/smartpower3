#ifndef wifi_manager_h
#define wifi_manager_h

#include <WiFi.h>
#include <WiFiUdp.h>
#include "settings.h"

#define SIZE_LOG_BUFFER0 31
#define SIZE_LOG_BUFFER1 23
#define SIZE_LOG_BUFFER2 23
#define SIZE_CHECKSUM_BUFFER 8

#define WIFI_UDP_PORT 3333

#define SERIAL_CTRL_C       0x03
#define SERIAL_ENTER        0x0D
#define SERIAL_Y            0x79
#define SERIAL_N            0x6E
#define CONNECT_RETRY_CNT   20

#define EMPTY_IPADDRESS F("0.0.0.0")
#define EMPTY_PORT 0


class WiFiManager;  // forward declaration to be able to use WiFiManager in the following struct


typedef void (WiFiManager::*wifiman_member_function)();

struct menu_item {
	const char* label;
	wifiman_member_function callback;
};


class WiFiManager
{
public:
	WiFiManager(Settings *settings);
	void viewMainMenu(void);
	void viewApList(int16_t ap_list_cnt);
	int16_t apScanning(void);
	bool apConnect(uint8_t ap_number, char *passwd);
	bool apConnect(const char* ssid, const char* passwd, bool show_ctrl_c_info = false);
	bool apConnectFromSettings(void);
	void apSetPassword(uint8_t ap_number);
	void apSelect(int16_t ap_list_cnt);
	void apSelectAndConnect(void);
	void apForget(void);
	void apDisconnectAndTurnWiFiOff(void);
	void apInfo(void);
	String apInfoSaved(void);
	//char* apInfoSaved(void);
	String apInfoConnected(void);
	//char apInfoConnected(void);
	void udpServerInfo();
	void udpServerForget(void);
	void switchWiFiConnection(void);
	void WiFiMenuMain(char idata);
	bool isCommandMode(void);
	void enterCommandMode(void);
	bool isConnected(void);
	bool canConnect(void);
	bool hasSavedConnectionInfo(void);
	bool isWiFiEnabled(void);
	void disableWiFi(void);
	void enableWiFi(void);
	void switchWifiState(bool from_storage);
	void setUdp();
	void switchLoggingOnOff(void);
	void switchDhcpOnOff(void);
	void setStaticIPAddress(void);
	void setGatewayAddress(void);
	void setSubnetMask(void);
	void setDnsServers(void);
	void switchMode(void);
	uint16_t port_udp = 0;
	IPAddress ipaddr_udp;
	bool update_udp_info = true;
	bool update_wifi_info = true;

	bool update_dhcp_info = true;
	bool update_static_ip_info = true;
	bool update_subnet_info = true;
	bool update_gateway_address_info = true;
	bool update_dns_info = true;
	bool update_mode_info = true;

	void runWiFiLogging(const char *buf0, const char *buf1, const char *buf2, const char *buf3);
	device_operation_mode getOperationMode(bool from_storage = false);
private:
	WiFiUDP udp;
	WiFiClient client;
	Settings *settings;
	bool commandMode = false;
	void doApForget(void);
	void doUdpServerForget(void);
	void doSwitchWiFiState(void);
	void doSwitchLogging(void);
	void doSwitchDhcp(void);
	void serialPrintCtrlCNotice(void);
	IPAddress serialGetUdpServerAddress(void);
	uint16_t serialGetUdpServerPort(void);
	void setUdpServerPortAndAddress(String ipaddr_udp, uint16_t port);
	void checkAndResetIndexAndValue(
			uint8_t& index_variable,
			char& indexed_variable,
			int indexed_variable_length,
			const char *error_message,
			const char *prompt_message
	);
	void doYesNoSelection(
			void (WiFiManager::*func)(),
			const char *confirmation_string,
			const char *approval_string,
			const char *denial_string
	);
	bool isDigitChar(char input_char);
	void setStorageAPConnectionInfo(const char* ssid, const char* password,
								wifi_credentials_state_e credentials_state);
	IPAddress serialGetIPAddress(const char* serial_message);
	void goToMainMenu(void);
	void exitCommandMode(void);
	uint8_t serialGetInt(void);
	void checkAndSetDhcpSettings(void);
	uint8_t current_menu_level = 0;
	uint8_t selected_submenu = 0;
	uint8_t current_submenu_item_count = 0;
	menu_item main_cmd_menu[5][7] PROGMEM = {
		{{F("1. Network Settings"), nullptr},
			{F("	1. Go back to Main Menu"), &WiFiManager::goToMainMenu},
			{F("	2. Switch DHCP ON or OFF"), &WiFiManager::switchDhcpOnOff},
			{F("	3. Set Static IP Address"), &WiFiManager::setStaticIPAddress},
			{F("	4. Set Gateway IP Address"), &WiFiManager::setGatewayAddress},
			{F("	5. Set Subnet Mask"), &WiFiManager::setSubnetMask},
			{F("	6. Set DNS Servers"), &WiFiManager::setDnsServers},
		},
		{{F("2. WiFi Settings"), nullptr},
			{F("	1. Go back to Main Menu"), &WiFiManager::goToMainMenu},
			{F("	2. Connection AP Info"), &WiFiManager::apInfo},
			{F("	3. Scan & Connection AP"), &WiFiManager::apSelectAndConnect},
			{F("	4. Switch WiFi connection ON or OFF"), &WiFiManager::switchWiFiConnection},
			{F("	5. Forget Connection AP"), &WiFiManager::apForget}
		},
		{{F("3. Logging Settings"), nullptr},
			{F("	1. Go back to Main Menu"), &WiFiManager::goToMainMenu},
			{F("	2. Connection UDP server Info"), &WiFiManager::udpServerInfo},
			{F("	3. Set IP address of UDP server for data logging"), &WiFiManager::setUdp},
			{F("	4. Switch logging ON or OFF"), &WiFiManager::switchLoggingOnOff},
			{F("	5. Forget UDP server IP address"), &WiFiManager::udpServerForget}
		},
		{{F("4. Mode settings"), nullptr},
			{F("	1. Go back to Main Menu"), &WiFiManager::goToMainMenu},
			{F("	2. Switch operation mode (default/SCPI)"), &WiFiManager::switchMode}
		},
		{{F("5. Command Mode exit"), &WiFiManager::exitCommandMode}
		}
	};
};


#endif
