#ifndef wifi_manager_h
#define wifi_manager_h

#include <ArduinoNvs.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "setting.h"

#define SERIAL_CTRL_C       0x03
#define SERIAL_ENTER        0x0D
#define SERIAL_Y            0x79
#define SERIAL_N            0x6E
#define CONNECT_RETRY_CNT   20

#define EMPTY_IPADDRESS "0.0.0.0"
#define EMPTY_PORT 0

#define WIFI_CMD_MENU_CNT   (sizeof(WIFI_CMD_MENU)/sizeof(WIFI_CMD_MENU[0]))

enum wifi_credentials_state {
	STATE_CREDENTIALS_OK = 0,
	STATE_CREDENTIALS_INVALID = 1,
	STATE_CREDENTIALS_NOT_CHECKED = 2,
};

const char WIFI_CMD_MENU[][50] = {
	"[ WIFI Command mode menu ]",
	"1. Connection AP Info",
	"2. Connection UDP server Info",
	"3. Scan & Connection AP",
	"4. Set IP address of UDP server for data logging",
	"5. Forget Connection AP",
	"6. Forget UDP server IP address",
	"7. (Dis)Connect WiFi connection",
	"8. Switch logging ON or OFF",
	"9. WiFi Command mode exit"
};

class WiFiManager
{
public:
	WiFiManager(WiFiUDP &udp, WiFiClient &client, Setting *setting);
	void view_main_menu(void);
	void view_ap_list(int16_t ap_list_cnt);
	int16_t ap_scanning(void);
	bool ap_connect(uint8_t ap_number, char *passwd);
	bool ap_connect(String ssid, String passwd, bool show_ctrl_c_info = false);
	bool ap_connect_from_settings(void);
	void ap_set_passwd(uint8_t ap_number);
	void ap_select(int16_t ap_list_cnt);
	void ap_select_and_connect(void);
	void ap_forget(void);
	void ap_disconnect_and_turn_wifi_off(void);
	void ap_info(void);
	String ap_info_saved(void);
	String ap_info_connected(void);
	void udp_server_info();
	void udp_server_forget(void);
	void switch_wifi_connection(void);
	void cmd_main(char idata);
	bool isCommandMode(void);
	void setCommandMode(void);
	bool is_connected(void);
	bool can_connect(void);
	bool hasSavedConnectionInfo(void);
	bool isWiFiEnabled(void);
	void disableWiFi(void);
	void enableWiFi(void);
	void set_udp();
	wifi_credentials_state credentials_state = STATE_CREDENTIALS_OK;
	uint16_t port_udp = 0;
	IPAddress ipaddr_udp;
	bool update_udp_info = true;
	bool update_wifi_info = true;
	void switchLoggingOnOff(void);
private:
	WiFiUDP udp;
	WiFiClient client;
	Setting *setting;
	bool commandMode = false;
	void do_ap_forget(void);
	void do_udp_server_forget(void);
	void do_switch_wifi_state(void);
	void do_switch_logging(void);
	void serialPrintCtrlCNotice(void);
	IPAddress serial_get_udp_server_address(void);
	uint16_t serial_get_udp_server_port(void);
	void set_udp_server_port_and_address(String ipaddr, uint16_t port);
	bool enabled = true;
protected:
	void checkAndResetIndexAndValue(
			uint8_t& index_variable,
			char& indexed_variable,
			int indexed_variable_length,
			const char *error_message,
			const char *prompt_message
	);
	void do_yes_no_selection(
			void (WiFiManager::*func)(),
			const char *confirmation_string,
			const char *approval_string,
			const char *denial_string
	);
	bool isDigitChar(char input_char);
	void setNVSAPConnectionInfo(String ssid, String password, bool wifi_conn_ok);
	bool getNVSAPConnectionInfo(String& ssid, String& password);
};

#endif
