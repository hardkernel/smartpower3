#include "wifimanager.h"

// general
const char *MSG_ON = "ON";
const char *MSG_OFF = "OFF";
// menu items
const char *MSG_CMD_MODE_ENTERED = ">>> WiFi command mode entered <<<";
const char *MSG_CMD_MODE_EXITED = ">>> WiFi command mode exited <<<";
const char *MSG_CMD_UNKNOWN = ">>> Unknown command <<<";
const char *MSG_CMD_NO_NETWORK = ">>> No networks found <<<";
const char *MSG_CMD_SCAN_RUNNING = ">>> Network scan in progress <<<";
const char *MSG_CMD_SCAN_FAILED = ">>> Network scan failed <<<";
const char *MSG_CMD_NETWORK_UNKNOWN_ERROR = ">>> Network unknown error <<<";
const char *MSG_CMD_SCANNING = ">>> AP scanning <<<";
const char *MSG_CMD_CONNECT = ">>> AP connecting <<<";
const char *MSG_CMD_SELECT = ">>> AP select <<<";
const char *MSG_CMD_CONNECT_INFO = ">>> AP connection info <<<";
const char *MSG_CMD_NO_CONNECT = ">>> AP no connection <<<";
const char *MSG_CMD_SET_UDP = ">>> Setting UDP <<<";
const char *MSG_CMD = "Command : ";
const char *MSG_CMD_UDP_INFO = ">>> UDP server connection info for logging <<<";
const char *MSG_CMD_NO_UDP_INFO = ">>> No UDP server info <<<";
const char *MSG_CMD_FORGET_CONNECTION = ">>> Forget connection AP <<<";
const char *MSG_CMD_FORGET_UDP_SERVER_INFO = ">>> Forget UDP server <<<";
const char *MSG_CMD_TURN_WIFI_ON_OFF = "[Dis]Connect pre-saved WiFi network. Current state: ";
const char *MSG_CMD_TURN_LOGGING_ON_OFF = "Switch logging ON or OFF (if logging interval is set). Current state: ";
// yes/no selections
// common
const char *MSG_YN_REDO_SELECTION_REQUEST = "\n\rPlease enter \"y\" or \"n\" & Enter or press Ctrl+C: ";
// UDP server forget
const char *MSG_YN_UDP_FORGET_CONFIRMATION = "Do you really want to forget the UDP server settings? (y/n & Enter): ";
const char *MSG_YN_UDP_FORGET_SUCCESS = "\n\rUDP server settings have been erased.";
const char *MSG_YN_UDP_FORGET_FAILURE = "\n\rUDP server settings have NOT been erased.";
// WiFi AP forget
const char *MSG_YN_AP_FORGET_CONFIRMATION = "Do you really want to forget the network settings? (y/n & Enter): ";
const char *MSG_YN_AP_FORGET_SUCCESS = "\n\rWiFi access point data have been erased.";
const char *MSG_YN_AP_FORGET_FAILURE = "\n\rWiFi access point data have NOT been erased.";
// UDP server setting string
const char *MSG_UDP_SERVER_IP_ADDRESS = "Input your IP address of UDP server (for example \"192.168.0.5\"): ";
const char *MSG_UDP_SERVER_IP_ADDRESS_PORT = "Input your port number of UDP server (for example \"6000\"): ";
const char *MSG_UDP_SERVER_UPDATE_SUCCESS = "Successfully set UDP server address and port: %s:%d\r\n";
// Switch WiFi state setting string
const char *MSG_YN_WIFI_SWITCH_STATE_CONFIRMATION = "Do you really want to switch WiFi state? (y/n & Enter): ";
const char *MSG_YN_WIFI_SWITCH_STATE_SUCCESS = "\n\rWiFi state successfully changed.";
const char *MSG_YN_WIFI_SWITCH_STATE_FAILURE = "\n\rWiFi state not changed.";
// Switch logging on/off
const char *MSG_YN_LOGGING_SWITCH_STATE_CONFIRMATION = "Do you really want to switch logging state? (y/n & Enter): ";
const char *MSG_YN_LOGGING_SWITCH_STATE_SUCCESS = "\n\rLogging state successfully changed.";
const char *MSG_YN_LOGGING_SWITCH_STATE_FAILURE = "\n\rLogging state not changed.";
// AP selection
const char *MSG_AP_SELECT = "Select AP Number (0 - %d) & Enter : ";
const char *MSG_AP_SELECT_PASSWORD = "Please enter password & press Enter : ";

const char *encryption_str(int encryption)
{
	switch(encryption) {
		case    WIFI_AUTH_OPEN:             return  "Open";
		case    WIFI_AUTH_WEP:              return  "WEP";
		case    WIFI_AUTH_WPA_PSK:          return  "WPA PSK";
		case    WIFI_AUTH_WPA2_PSK:         return  "WPA2 PSK";
		case    WIFI_AUTH_WPA_WPA2_PSK:     return  "WPA WPA2 PSK";
		case    WIFI_AUTH_WPA2_ENTERPRISE:  return  "WPA2 Enterprise";
		default:                            return  "Unknown";
	}
}

WiFiManager::WiFiManager(WiFiUDP &udp, WiFiClient &client, Setting *setting)
{
	this->udp = udp;
	this->client = client;
	this->setting = setting;
}

void WiFiManager::view_main_menu(void)
{
	Serial.printf("\n\r");
	for(uint8_t i = 0; i < WIFI_CMD_MENU_CNT; i++)
		Serial.printf("%s\n\r", &WIFI_CMD_MENU[i][0]);

	Serial.printf("%s", MSG_CMD);
}

void WiFiManager::view_ap_list(int16_t ap_list_cnt)
{
	// scanNetworks in ap_scanning can return negative value indicating failure
	// or unfinished scan - see WiFiType.h
	if (ap_list_cnt > 0) {
		Serial.printf("[ Networks found(%d) ]\n\r", ap_list_cnt);
		for (int16_t i = 0; i < ap_list_cnt; i++) {
			Serial.printf("%d - %s (%d dbm), Encryption (%s)\n\r",
							i,
							WiFi.SSID(i).c_str(),
							WiFi.RSSI(i),
							encryption_str(WiFi.encryptionType(i)));
		}
	} else if (!ap_list_cnt) {
		Serial.printf("%s\n\r", MSG_CMD_NO_NETWORK);
	} else if (ap_list_cnt == WIFI_SCAN_RUNNING) {
		Serial.printf("%s\n\r", MSG_CMD_SCAN_RUNNING);
	} else if (ap_list_cnt == WIFI_SCAN_FAILED) {
		Serial.printf("%s\n\r", MSG_CMD_SCAN_FAILED);
	} else {
		Serial.printf("%s\n\r", MSG_CMD_NETWORK_UNKNOWN_ERROR);
	}
}

int16_t WiFiManager::ap_scanning(void)
{
	Serial.printf("%s\n\r", MSG_CMD_SCANNING);
	WiFi.disconnect();  // needs to be disconnected between successive scans
	return WiFi.scanNetworks();
}

bool WiFiManager::ap_connect(String ssid, String passwd, bool show_ctrl_c_info)
{
	char cmd;
	uint8_t i = 0, wifi_conn_ok = 2;

	WiFi.disconnect();
	WiFi.begin(ssid.c_str(), passwd.c_str());

	if (show_ctrl_c_info) {
		this->serialPrintCtrlCNotice();
	}
	Serial.printf("Connecting... %s\n\r", ssid.c_str());

	while((i++ < CONNECT_RETRY_CNT)) {
		if (WiFi.status() == WL_CONNECTED) {
			Serial.printf("[[[ Connection Okay ]]]\n\r");
			Serial.printf("Connected AP : %s(%d dbm), local IP : %s\n\r",
					WiFi.SSID().c_str(), WiFi.RSSI(), WiFi.localIP().toString().c_str());
			this->setNVSAPConnectionInfo(ssid, passwd, "true");
			credentials_state = STATE_CREDENTIALS_OK;
			return true;
		}
		Serial.printf("\nConnecting wait (%d)...\n\r", CONNECT_RETRY_CNT - i);

		if (Serial.available()) {
			cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				NVS.setInt("wifi_conn_ok", false);
				WiFi.disconnect();
				return false;
			}
		}
		delay(1000);
	}

	wifi_conn_ok = NVS.getInt("wifi_conn_ok");
	if (wifi_conn_ok) {
		credentials_state = STATE_CREDENTIALS_OK;
		Serial.printf("\nConnecting to previous AP\n\r");
	} else if (!wifi_conn_ok) {
		credentials_state = STATE_CREDENTIALS_INVALID;
	} else {
		credentials_state = STATE_CREDENTIALS_NOT_CHECKED;
		Serial.printf("\n%s Connect failed\n\r", ssid.c_str());
	}
	passwd = NVS.setInt("wifi_conn_ok", false);

	return false;
}

bool WiFiManager::ap_connect(uint8_t ap_number, char *passwd)
{
	String ssid;

	ssid = WiFi.SSID(ap_number);

	Serial.printf("%s\n\r", MSG_CMD_CONNECT);
	Serial.printf("%s (%d dbm) %s\n\r",
		ssid.c_str(),
		WiFi.RSSI(ap_number),
		encryption_str(WiFi.encryptionType(ap_number)));

	return this->ap_connect(ssid, passwd, true);
}

bool WiFiManager::ap_connect_from_settings()
{
	String ssid, password;
	uint8_t i = 0;

	if (this->getNVSAPConnectionInfo(ssid, password)) {
		WiFi.disconnect();
		WiFi.begin(ssid.c_str(), password.c_str());
		if (WiFi.waitForConnectResult(CONNECT_RETRY_CNT * 1000) == WL_CONNECTED) {
			this->setNVSAPConnectionInfo(ssid, password, "true");
			this->credentials_state = STATE_CREDENTIALS_OK;
			return true;
		}
	}
	this->credentials_state = STATE_CREDENTIALS_INVALID;
	return false;
}

void WiFiManager::ap_set_passwd(uint8_t ap_number)
{
	char passwd[64], cmd; // 64 chars is the longest wifi password currently possible
	uint8_t pos = 0, sizeof_passwd = sizeof(passwd);

	memset(passwd, 0x00, sizeof_passwd);

	Serial.printf("\n\rAP (%s, %d dbm) \r\n",
			WiFi.SSID(ap_number).c_str(),
			WiFi.RSSI(ap_number));
	Serial.printf(MSG_AP_SELECT_PASSWORD);

	if (WiFi.encryptionType(ap_number) == WIFI_AUTH_OPEN) {
		this->ap_connect(ap_number, passwd);
		return;
	}

	while(true) {
		if (Serial.available()) {
			cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				return;
			} else if ( cmd == SERIAL_ENTER ) {
				Serial.printf("\n\r");
				this->ap_connect(ap_number, passwd);
				return;
			}
			else {
				Serial.printf("*");
				passwd[pos++] = cmd;
			}
			this->checkAndResetIndexAndValue(pos, *passwd, sizeof_passwd, "\n\rPassword longer than possible.", MSG_AP_SELECT_PASSWORD);
		}
		delay(100);
	}
}

void WiFiManager::ap_select(int16_t ap_list_cnt)
{
	char cmd, sel_ap_number[5];
	int16_t pos = 0, connect_ap_number = 0, sizeof_sel_ap_number = sizeof(sel_ap_number);

	memset(sel_ap_number, 0x00, sizeof(sel_ap_number));

	this->serialPrintCtrlCNotice();

	Serial.printf("%s\n\r", MSG_CMD_SELECT);
	Serial.printf(MSG_AP_SELECT, ap_list_cnt-1);

	while(true) {
		if (Serial.available()) {
			cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				return;
			} else if (cmd == SERIAL_ENTER) {
				this->ap_set_passwd(connect_ap_number);
				return;
			} else {
				Serial.printf("%c", cmd);
				if (this->isDigitChar(cmd) && (pos < sizeof_sel_ap_number -1)) {
					sel_ap_number[pos++] = cmd;
				} else {
					pos = sizeof_sel_ap_number;
				}
			}
			connect_ap_number = strtoul(&sel_ap_number[0], NULL, 10);
			if ((connect_ap_number > ap_list_cnt-1) || pos > sizeof_sel_ap_number-1) {
				Serial.println(">>> Error selecting AP number <<<");
				Serial.printf(MSG_AP_SELECT, ap_list_cnt-1);
				pos = 0;
				memset(sel_ap_number, 0x00, sizeof_sel_ap_number);
			}
		}
		delay(100);
	}
}

void WiFiManager::ap_select_and_connect()
{
	// if Enter is pressed, first AP is the default
	int16_t APListCount = 0;

	APListCount = this->ap_scanning();
	this->view_ap_list(APListCount);
	// scanNetworks in ap_scanning can return negative value indicating failure
	// or unfinished scan - see WiFiType.h
	if (APListCount > 0) {
		this->ap_select(APListCount);
	}
}

void WiFiManager::do_ap_forget()
{
	WiFi.disconnect(true, true);
	this->setNVSAPConnectionInfo("", "", "false");
	credentials_state = STATE_CREDENTIALS_INVALID;
	delay(100);
	update_wifi_info = true;
}

void WiFiManager::ap_forget()
{
	this->do_yes_no_selection(
			&WiFiManager::do_ap_forget,
			MSG_YN_AP_FORGET_CONFIRMATION,
			MSG_YN_AP_FORGET_SUCCESS,
			MSG_YN_AP_FORGET_FAILURE
	);
}

void WiFiManager::ap_disconnect_and_turn_wifi_off()
{
	WiFi.disconnect(true, false);
}

void WiFiManager::udp_server_info()
{
	Serial.printf("IP Address [%s]\n\r", ipaddr_udp.toString().c_str());
	Serial.printf("Port [%d]\n\r", port_udp);
}

void WiFiManager::ap_info()
{
	if (WiFi.status() == WL_CONNECTED) {
		Serial.printf("%s\n\r", MSG_CMD_CONNECT_INFO);
		Serial.printf("%s (%d dbm)\n\r", WiFi.SSID().c_str(), WiFi.RSSI());
		Serial.printf("IP Address [%s]\n\r", WiFi.localIP().toString().c_str());
		Serial.printf("MAC Address [%s]\n\r", WiFi.macAddress().c_str());
	} else if (this->ap_info_saved() != "") {
		Serial.printf("%s\n\r", MSG_CMD_CONNECT_INFO);
		Serial.printf("%s [saved connection data]\n\r", this->ap_info_saved().c_str());
	} else {
		Serial.printf("%s\n\r", MSG_CMD_NO_CONNECT);
	}
}

String WiFiManager::ap_info_saved()
{
	return NVS.getString("ssid");
}

String WiFiManager::ap_info_connected()
{
	char wifi_info[35];

	sprintf(wifi_info, "%s : %s", WiFi.SSID(), WiFi.localIP().toString());
	return wifi_info;
}

void WiFiManager::set_udp()
{
	IPAddress ipaddr;
	uint16_t port;

	this->serialPrintCtrlCNotice();

	ipaddr = this->serial_get_udp_server_address();
	if (ipaddr) {
		port = this->serial_get_udp_server_port();
		if (port) {
			this->set_udp_server_port_and_address(ipaddr.toString(), port);
			Serial.printf(MSG_UDP_SERVER_UPDATE_SUCCESS, ipaddr.toString().c_str(), port);
		}
	}
}

IPAddress WiFiManager::serial_get_udp_server_address()
{
	char ipaddr[16], cmd;
	uint8_t pos = 0, sizeof_ipaddr = sizeof(ipaddr);
	IPAddress result_ipaddress;

	Serial.printf(MSG_UDP_SERVER_IP_ADDRESS);

	memset(ipaddr, 0x00, sizeof_ipaddr);

	while(true) {
		if (Serial.available()) {
			cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				return false;
			} else if ( cmd == SERIAL_ENTER ) {
				Serial.println();
				if (result_ipaddress.fromString(ipaddr)) {
					return result_ipaddress;
				} else {
					pos = sizeof_ipaddr;
				}
			} else {
				Serial.printf("%c", cmd);
				if (this->isDigitChar(cmd) || cmd == 46) // digit or dot
					ipaddr[pos++] = cmd;
				else
					pos = sizeof_ipaddr;
			}
			this->checkAndResetIndexAndValue(pos, *ipaddr, sizeof_ipaddr, "\n\rWrong IP address.", MSG_UDP_SERVER_IP_ADDRESS);
		}
		delay(100);
	}
}

uint16_t WiFiManager::serial_get_udp_server_port()
{
	char port[5], cmd;
	uint8_t pos = 0, sizeof_port = sizeof(port);
	uint16_t resulting_port;

	Serial.printf(MSG_UDP_SERVER_IP_ADDRESS_PORT);

	memset(port, 0x00, sizeof_port);

	while(true) {
		if (Serial.available()) {
			cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				return false;
			} else if ( cmd == SERIAL_ENTER ) {
				resulting_port = atoi(port);
				if (resulting_port > 0 and resulting_port < 10000) {
					Serial.println();
					return resulting_port;
				} else {
					pos = sizeof_port;
				}
			} else {
				Serial.printf("%c", cmd);
				if (this->isDigitChar(cmd)) {
					port[pos++] = cmd;
				} else {
					pos = sizeof_port;
				}
			}
			this->checkAndResetIndexAndValue(pos, *port, sizeof_port, "\n\rWrong port number.", MSG_UDP_SERVER_IP_ADDRESS_PORT);
		}
		delay(100);
	}
}

void WiFiManager::do_switch_wifi_state()
{
	if (this->is_connected()) {
		this->disableWiFi();
	} else if (this->can_connect()) {
		this->enableWiFi();
	} else {
		Serial.println("\n\rPlease set connection settings first!");
	}
}

void WiFiManager::switch_wifi_connection()
{
	this->do_yes_no_selection(
			&WiFiManager::do_switch_wifi_state,
			MSG_YN_WIFI_SWITCH_STATE_CONFIRMATION,
			MSG_YN_WIFI_SWITCH_STATE_SUCCESS,
			MSG_YN_WIFI_SWITCH_STATE_FAILURE
	);
}

void WiFiManager::set_udp_server_port_and_address(String ipaddr, uint16_t port)
{
	NVS.setString("ipaddr_udp", ipaddr);
	NVS.setInt("port_udp", port);
	port_udp = port;
	ipaddr_udp.fromString(ipaddr);
	update_udp_info = true;
}

void WiFiManager::do_udp_server_forget()
{
	this->set_udp_server_port_and_address((String)EMPTY_IPADDRESS, (uint16_t)EMPTY_PORT);
}

void WiFiManager::udp_server_forget()
{
	this->do_yes_no_selection(
			&WiFiManager::do_udp_server_forget,
			MSG_YN_UDP_FORGET_CONFIRMATION,
			MSG_YN_UDP_FORGET_SUCCESS,
			MSG_YN_UDP_FORGET_FAILURE
	);
}

void WiFiManager::do_switch_logging()
{
	if (setting->isLoggingEnabled()) {
		setting->disableLogging();
	} else {
		setting->enableLogging();
	}
}

void WiFiManager::switchLoggingOnOff()
{
	this->do_yes_no_selection(
			&WiFiManager::do_switch_logging,
			MSG_YN_LOGGING_SWITCH_STATE_CONFIRMATION,
			MSG_YN_LOGGING_SWITCH_STATE_SUCCESS,
			MSG_YN_LOGGING_SWITCH_STATE_FAILURE
	);
}

void WiFiManager::cmd_main(char idata)
{
	Serial.println(idata);
	Serial.println();

	switch(idata) {
		case    '1':
			ap_info();
			break;
		case    '2':
			Serial.printf("%s\n\r", MSG_CMD_UDP_INFO);
			udp_server_info();
			break;
		case    '3':
			ap_select_and_connect();
			break;
		case    '4':
			Serial.printf("%s\n\r", MSG_CMD_SET_UDP);
			set_udp();
			break;
		case    '5':
			Serial.printf("%s\n\r", MSG_CMD_FORGET_CONNECTION);
			ap_forget();
			break;
		case    '6':
			Serial.printf("%s\n\r", MSG_CMD_FORGET_UDP_SERVER_INFO);
			udp_server_forget();
			break;
		case    '7':
			Serial.printf(
					"%s%s\n\r",
					MSG_CMD_TURN_WIFI_ON_OFF,
					this->is_connected() ? MSG_ON : MSG_OFF);
			switch_wifi_connection();
			break;
		case    '8':
			Serial.printf(
					"%s%s\n\r",
					MSG_CMD_TURN_LOGGING_ON_OFF,
					setting->isLoggingEnabled() ? MSG_ON : MSG_OFF);
			switchLoggingOnOff();
			break;
		case    '9':
		case    SERIAL_CTRL_C:
			commandMode = false;
			Serial.printf("%s\n\r", MSG_CMD_MODE_EXITED);
			return;
		default :
			Serial.printf("%s\n\r", MSG_CMD_UNKNOWN);
			break;
	}

	view_main_menu();
	while (Serial.available())
		Serial.read();
}

bool WiFiManager::is_connected(void)
{
	return (WiFi.status() == WL_CONNECTED);
}

bool WiFiManager::can_connect(void)
{
	return (
			!this->is_connected()
			&& this->credentials_state == STATE_CREDENTIALS_OK
			&& this->hasSavedConnectionInfo());
}

bool WiFiManager::hasSavedConnectionInfo()
{
	return (ap_info_saved() != "");
}

bool WiFiManager::isWiFiEnabled()
{
	return this->enabled;
}

void WiFiManager::disableWiFi()
{
	this->enabled = false;
}

void WiFiManager::enableWiFi()
{
	this->enabled = true;
}

bool WiFiManager::isCommandMode(void)
{
	return commandMode;
}

void WiFiManager::setCommandMode(void)
{
	commandMode = true;
	Serial.printf("\n\r%s\n\r", MSG_CMD_MODE_ENTERED);
}

void WiFiManager::serialPrintCtrlCNotice()
{
	Serial.println("**************************************************");
	Serial.println("*                                                *");
	Serial.println("*           For exit press [Ctrl+c]...           *");
	Serial.println("*                                                *");
	Serial.println("**************************************************");
}

void WiFiManager::checkAndResetIndexAndValue(uint8_t& index_variable, char& indexed_variable, int indexed_variable_length, const char *error_message, const char *prompt_message)
{
	if (index_variable > indexed_variable_length-1) {
		Serial.println(error_message);
		Serial.printf(prompt_message);
		index_variable = 0;
		memset(&indexed_variable, 0x00, indexed_variable_length);
	}
}

bool WiFiManager::isDigitChar(char input_char)
{
	return (input_char > 47) && (input_char < 58);
}

void WiFiManager::do_yes_no_selection(void (WiFiManager::*func)(), const char *confirmation_string, const char *approval_string, const char *denial_string)
{
	char buf, cmd;

	Serial.printf("%s", confirmation_string);

	while (true) {
		if (Serial.available()) {
			cmd = Serial.read();
			switch (cmd) {
				case SERIAL_CTRL_C:
					return;
				case SERIAL_ENTER:
					switch (buf) {
						case SERIAL_Y:
							(this->* func) ();
							Serial.println(approval_string);
							return;
						case SERIAL_N:
							Serial.println(denial_string);
							return;
						default:
							Serial.printf(MSG_YN_REDO_SELECTION_REQUEST);
					}
					break;
				default:
					Serial.printf("%c", cmd);
					buf = cmd;
			}
		}
	}
}

void WiFiManager::setNVSAPConnectionInfo(String ssid, String password, bool wifi_conn_ok)
{
	NVS.setString("ssid", ssid);
	NVS.setString("passwd", password);
	NVS.setInt("wifi_conn_ok", wifi_conn_ok);
}

bool WiFiManager::getNVSAPConnectionInfo(String &ssid, String &password)
{
	return NVS.getString("ssid", ssid) && NVS.getString("passwd", password);
}
