#ifndef wifi_manager_h
#define wifi_manager_h

#include <ArduinoNvs.h>
#include <WiFi.h>
#include <WiFiUdp.h>

#define SERIAL_CTRL_C       0x03
#define SERIAL_ENTER        0x0D
#define SERIAL_Y            0x79
#define SERIAL_N            0x6E
#define CONNECT_RETRY_CNT   20


enum {
    eSTATE_WIFI_IDEL,
    eSTATE_WIFI_INIT,
    eSTATE_WIFI_MAIN,
    eSTATE_WIFI_SELECT,
    eSTATE_WIFI_CONNECT,
    eSTATE_WIFI_SCAN,
    eSTATE_WIFI_TEST,
};

/*
const char *MSG_CMD_MODE_ENTERED = ">>> WIFI Command mode entered <<<";
const char *MSG_CMD_MODE_EXITED = ">>> WIFI Command mode exited <<<";
const char *MSG_CMD_UNKNOWN = ">>> Unknown command <<<";
const char *MSG_CMD_NO_NETWORK = ">>> No Networks found <<<";
const char *MSG_CMD_SCANNING = ">>> AP Scanning <<<";
const char *MSG_CMD_CONNECT = ">>> AP Connecting <<<";
const char *MSG_CMD_SELECT = ">>> AP Select <<<";
const char *MSG_CMD_CONNECT_INFO = ">>> AP Connection Info <<<";
const char *MSG_CMD_NO_CONNECT = ">>> AP no connnection <<<";
const char *MSG_CMD = "Command : ";
const char *TestSendData = "01234567890123456789012345678901234567890123456789012345678901234567890";
*/

const char WIFI_CMD_MENU[][50] = {
    "[ WIFI Command mode menu ]",
    "1. Connection AP Info",
    "2. Connection UDP server Info",
    "3. Scan & Connection AP",
    "4. Set IP address of UDP server for data logging",
    "5. Forget Connection AP",
    "6. Forget UDP server IP address",
    "7. WiFi Command mode exit"
};

#define WIFI_CMD_MENU_CNT   (sizeof(WIFI_CMD_MENU)/sizeof(WIFI_CMD_MENU[0]))


class WiFiManager
{
public:
	WiFiManager(WiFiUDP &udp, WiFiClient &client);
	void view_main_menu(void);
	void view_ap_list(int ap_list_cnt);
	void ap_scanning(void);
	bool ap_connect(int ap_number, char *passwd);
	bool ap_connect(String ssid, String passwd);
	void ap_set_passwd(int ap_number);
	void ap_select(int ap_list_cnt);
	void ap_forget(void);
	void ap_info(int ap_number);
	void udp_server_info();
	void udp_server_forget(void);
	void cmd_main(char idata);
	bool isCommandMode(void);
	void setCommandMode(void);
	void set_udp();
	uint8_t state = 0;
	uint8_t nvs_state = 0;
	//void setConnType(CONN_TYPE conn_type);
	uint16_t port_udp = 0;
	IPAddress ipaddr_udp;
	bool update_udp_info = true;
private:
	WiFiUDP udp;
	WiFiClient client;
	uint16_t WiFiState = eSTATE_WIFI_IDEL;

	int ConnectAP_Number = 0, ConnectAP_RSSI = 0, APListCount = 0, TCPTestCount = 0;
	char ConnectAP_Passwd[64] = {0,};
	bool isConnectedAP = false, isTCPTest = false;
	bool commandMode = false;
	String nvs_ssid = "";
	String nvs_passwd = "";
	void do_ap_forget();
	void do_udp_server_forget();
	void do_yes_no_selection(
			void (WiFiManager::*func)(),
			const char *confirmation_string,
			const char *approval_string,
			const char *denial_string
	);
	//uint8_t wifi_conn_type = CONN_NONE;
};

#endif
