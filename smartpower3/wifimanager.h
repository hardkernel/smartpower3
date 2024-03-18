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

#define EMPTY_IPADDRESS "0.0.0.0"
#define EMPTY_PORT 0

#define WIFI_CMD_MENU_CNT   (sizeof(wifi_cmd_menu)/sizeof(wifi_cmd_menu[0]))


const char wifi_cmd_menu[][50] = {
	"[ WIFI Command mode menu ]",
	"1. Connection AP Info",
	"2. Connection UDP server Info",
	"3. Scan & Connection AP",
	"4. Set IP address of UDP server for data logging",
	"5. Forget Connection AP",
	"6. Forget UDP server IP address",
	"7. (Dis)Connect WiFi connection",
	"8. Switch logging ON or OFF",
	"9. Allow toggling channel power via WiFi",
	"A. WiFi Command mode exit"
};

class ScreenManager;

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
	String apInfoConnected(void);
	void udpServerInfo();
	void udpServerForget(void);
	void switchWiFiConnection(void);
	void WiFiMenuMain(char idata);
	bool isCommandMode(void);
	void setCommandMode(void);
	bool isConnected(void);
	bool canConnect(void);
	bool hasSavedConnectionInfo(void);
	bool isWiFiEnabled(void);
	void disableWiFi(void);
	void enableWiFi(void);
	void switchWifiState(bool from_storage);
	void setUdp();
	void switchLoggingOnOff(void);
	void switchPowerToggling(void);
	uint16_t port_udp = 0;
	IPAddress ipaddr_udp;
	bool update_udp_info = true;
	bool update_wifi_info = true;
	void runWiFiLogging(const char *buf0, const char *buf1, const char *buf2, const char *buf3);
	void parseWiFiPacket(ScreenManager &screen_manager);
private:
	WiFiUDP udp;
	WiFiClient client;
	Settings *settings;
	bool commandMode = false;
	void doApForget(void);
	void doUdpServerForget(void);
	void doSwitchWiFiState(void);
	void doSwitchLogging(void);
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
};

#endif
