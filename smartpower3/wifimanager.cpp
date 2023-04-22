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

WiFiManager::WiFiManager(SettingScreen *setting_screen, Settings *settings)
{
	WiFi.mode(WIFI_STA);
	WiFi.disconnect();
	delay(100);
	udp.begin(WIFI_UDP_PORT);

	this->udp = udp;
	this->setting_screen = setting_screen;
	this->settings = settings;
}

void WiFiManager::viewMainMenu(void)
{
	Serial.printf("\n\r");
	for(uint8_t i = 0; i < WIFI_CMD_MENU_CNT; i++)
		Serial.printf("%s\n\r", &wifi_cmd_menu[i][0]);

	Serial.printf("%s", MSG_CMD);
}

void WiFiManager::viewApList(int16_t ap_list_cnt)
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

int16_t WiFiManager::apScanning(void)
{
	Serial.printf("%s\n\r", MSG_CMD_SCANNING);
	WiFi.disconnect();  // needs to be disconnected between successive scans
	return WiFi.scanNetworks();
}

bool WiFiManager::apConnect(const char* ssid, const char* passwd, bool show_ctrl_c_info)
{
	char cmd;
	uint8_t i = 0;

	WiFi.disconnect();
	WiFi.begin(ssid, passwd);

	if (show_ctrl_c_info) {
		this->serialPrintCtrlCNotice();
	}
	Serial.printf("Connecting... %s\n\r", ssid);

	while((i++ < CONNECT_RETRY_CNT)) {
		if (WiFi.status() == WL_CONNECTED) {
			Serial.printf("[[[ Connection Okay ]]]\n\r");
			Serial.printf("Connected AP : %s(%d dbm), local IP : %s\n\r",
					WiFi.SSID().c_str(),
					WiFi.RSSI(),
					WiFi.localIP().toString().c_str());
			this->setNVSAPConnectionInfo(ssid, passwd, WIFI_CREDENTIALS_STATE_OK);
			return true;
		}
		Serial.printf("\nConnecting wait (%d)...\n\r", CONNECT_RETRY_CNT - i);

		if (Serial.available()) {
			cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				settings->setWifiCredentialsState(WIFI_CREDENTIALS_STATE_NOT_CHECKED);
				WiFi.disconnect();
				return false;
			}
		}
		delay(1000);
	}

	if (settings->getWifiCredentialsState() == WIFI_CREDENTIALS_STATE_OK) {
		Serial.printf("\nReverting to previous AP\n\r");
	} else {
		Serial.printf("\n%s Connect failed\n\r", ssid);
	}
	settings->setWifiCredentialsState(WIFI_CREDENTIALS_STATE_NOT_CHECKED);

	return false;
}

bool WiFiManager::apConnect(uint8_t ap_number, char *passwd)
{
	String ssid;
	ssid.reserve(256);

	ssid = WiFi.SSID(ap_number);

	Serial.printf("%s\n\r", MSG_CMD_CONNECT);
	Serial.printf("%s (%d dbm) %s\n\r",
		ssid.c_str(),
		WiFi.RSSI(ap_number),
		encryption_str(WiFi.encryptionType(ap_number)));

	return this->apConnect(ssid.c_str(), passwd, true);
}

bool WiFiManager::apConnectFromSettings()
{
	if (
			!settings->getWifiAccessPointSsid().isEmpty()
			&& !settings->getWifiPassword().isEmpty()
	) {
		WiFi.disconnect();
		WiFi.begin(
				settings->getWifiAccessPointSsid().c_str(),
				settings->getWifiPassword().c_str());
		if (WiFi.waitForConnectResult(CONNECT_RETRY_CNT * 1000) == WL_CONNECTED) {
			this->setNVSAPConnectionInfo(
					settings->getWifiAccessPointSsid().c_str(),
					settings->getWifiPassword().c_str(),
					WIFI_CREDENTIALS_STATE_OK);
			return true;
		}
	}
	settings->setWifiCredentialsState(WIFI_CREDENTIALS_STATE_INVALID);
	return false;
}

void WiFiManager::apSetPassword(uint8_t ap_number)
{
	char passwd[64], cmd; // 64 chars is the longest password currently possible
	uint8_t pos = 0, sizeof_passwd = sizeof(passwd);

	memset(passwd, 0x00, sizeof_passwd);

	Serial.printf("\n\rAP (%s, %d dbm) \r\n",
			WiFi.SSID(ap_number).c_str(),
			WiFi.RSSI(ap_number));
	Serial.printf(MSG_AP_SELECT_PASSWORD);

	if (WiFi.encryptionType(ap_number) == WIFI_AUTH_OPEN) {
		this->apConnect(ap_number, passwd);
		return;
	}

	while(true) {
		if (Serial.available()) {
			cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				return;
			} else if ( cmd == SERIAL_ENTER ) {
				Serial.printf("\n\r");
				this->apConnect(ap_number, passwd);
				return;
			}
			else {
				Serial.printf("*");
				passwd[pos++] = cmd;
			}
			this->checkAndResetIndexAndValue(
					pos,
					*passwd,
					sizeof_passwd,
					"\n\rPassword longer than possible.",
					MSG_AP_SELECT_PASSWORD);
		}
		delay(100);
	}
}

void WiFiManager::apSelect(int16_t ap_list_cnt)
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
				this->apSetPassword(connect_ap_number);
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

void WiFiManager::apSelectAndConnect()
{
	// if Enter is pressed, first AP is the default
	int16_t APListCount = 0;

	APListCount = this->apScanning();
	this->viewApList(APListCount);
	// scanNetworks in ap_scanning can return negative value indicating failure
	// or unfinished scan - see WiFiType.h
	if (APListCount > 0) {
		this->apSelect(APListCount);
	}
}

void WiFiManager::doApForget()
{
	WiFi.disconnect(true, true);
	this->setNVSAPConnectionInfo("", "", WIFI_CREDENTIALS_STATE_INVALID);
	delay(100);
	update_wifi_info = true;
}

void WiFiManager::apForget()
{
	this->doYesNoSelection(
			&WiFiManager::doApForget,
			MSG_YN_AP_FORGET_CONFIRMATION,
			MSG_YN_AP_FORGET_SUCCESS,
			MSG_YN_AP_FORGET_FAILURE
	);
}

void WiFiManager::apDisconnectAndTurnWiFiOff()
{
	WiFi.disconnect(true, false);
}

void WiFiManager::udpServerInfo()
{
	Serial.printf("IP Address [%s]\n\r", ipaddr_udp.toString().c_str());
	Serial.printf("Port [%d]\n\r", port_udp);
}

void WiFiManager::apInfo()
{
	if (WiFi.status() == WL_CONNECTED) {
		Serial.printf("%s\n\r", MSG_CMD_CONNECT_INFO);
		Serial.printf("%s (%d dbm)\n\r", WiFi.SSID().c_str(), WiFi.RSSI());
		Serial.printf("IP Address [%s]\n\r", WiFi.localIP().toString().c_str());
		Serial.printf("MAC Address [%s]\n\r", WiFi.macAddress().c_str());
	} else if (this->apInfoSaved() != "") {
		Serial.printf("%s\n\r", MSG_CMD_CONNECT_INFO);
		Serial.printf("%s [saved connection data]\n\r", this->apInfoSaved().c_str());
	} else {
		Serial.printf("%s\n\r", MSG_CMD_NO_CONNECT);
	}
}

String WiFiManager::apInfoSaved()
{
	return settings->getWifiAccessPointSsid();
}

String WiFiManager::apInfoConnected()
{
	char wifi_info[35];

	sprintf(wifi_info, "%s : %s", WiFi.SSID(), WiFi.localIP().toString());
	return wifi_info;
}

void WiFiManager::setUdp()
{
	IPAddress ipaddr;
	uint16_t port;

	this->serialPrintCtrlCNotice();

	ipaddr = this->serialGetUdpServerAddress();
	if (ipaddr) {
		port = this->serialGetUdpServerPort();
		if (port) {
			this->setUdpServerPortAndAddress(ipaddr.toString(), port);
			Serial.printf(MSG_UDP_SERVER_UPDATE_SUCCESS, ipaddr.toString().c_str(), port);
		}
	}
}

IPAddress WiFiManager::serialGetUdpServerAddress()
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

uint16_t WiFiManager::serialGetUdpServerPort()
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

void WiFiManager::doSwitchWiFiState()
{
	if (this->isConnected()) {
		this->disableWiFi();
	} else if (this->canConnect()) {
		this->enableWiFi();
	} else {
		Serial.println("\n\rPlease set connection settings first!");
	}
}

void WiFiManager::switchWiFiConnection()
{
	this->doYesNoSelection(
			&WiFiManager::doSwitchWiFiState,
			MSG_YN_WIFI_SWITCH_STATE_CONFIRMATION,
			MSG_YN_WIFI_SWITCH_STATE_SUCCESS,
			MSG_YN_WIFI_SWITCH_STATE_FAILURE
	);
}

void WiFiManager::setUdpServerPortAndAddress(String ipaddr, uint16_t port)
{
	ipaddr_udp.fromString(ipaddr);
	settings->setWifiIpv4UdpLoggingServerIpAddress(ipaddr_udp);
	settings->setWifiIpv4UdpLoggingServerPort(port);
	port_udp = port;
	update_udp_info = true;
}

void WiFiManager::doUdpServerForget()
{
	this->setUdpServerPortAndAddress((String)EMPTY_IPADDRESS, (uint16_t)EMPTY_PORT);
}

void WiFiManager::udpServerForget()
{
	this->doYesNoSelection(
			&WiFiManager::doUdpServerForget,
			MSG_YN_UDP_FORGET_CONFIRMATION,
			MSG_YN_UDP_FORGET_SUCCESS,
			MSG_YN_UDP_FORGET_FAILURE
	);
}

void WiFiManager::doSwitchLogging()
{
	if (setting_screen->isLoggingEnabled()) {
		setting_screen->disableLogging();
	} else {
		setting_screen->enableLogging();
	}
}

void WiFiManager::switchLoggingOnOff()
{
	this->doYesNoSelection(
			&WiFiManager::doSwitchLogging,
			MSG_YN_LOGGING_SWITCH_STATE_CONFIRMATION,
			MSG_YN_LOGGING_SWITCH_STATE_SUCCESS,
			MSG_YN_LOGGING_SWITCH_STATE_FAILURE
	);
}

void WiFiManager::WiFiMenuMain(char idata)
{
	Serial.println(idata);
	Serial.println();

	switch(idata) {
		case    '1':
			apInfo();
			break;
		case    '2':
			Serial.printf("%s\n\r", MSG_CMD_UDP_INFO);
			udpServerInfo();
			break;
		case    '3':
			apSelectAndConnect();
			break;
		case    '4':
			Serial.printf("%s\n\r", MSG_CMD_SET_UDP);
			setUdp();
			break;
		case    '5':
			Serial.printf("%s\n\r", MSG_CMD_FORGET_CONNECTION);
			apForget();
			break;
		case    '6':
			Serial.printf("%s\n\r", MSG_CMD_FORGET_UDP_SERVER_INFO);
			udpServerForget();
			break;
		case    '7':
			Serial.printf(
					"%s%s\n\r",
					MSG_CMD_TURN_WIFI_ON_OFF,
					this->isConnected() ? MSG_ON : MSG_OFF);
			switchWiFiConnection();
			break;
		case    '8':
			Serial.printf(
					"%s%s\n\r",
					MSG_CMD_TURN_LOGGING_ON_OFF,
					setting_screen->isLoggingEnabled() ? MSG_ON : MSG_OFF);
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

	viewMainMenu();
	while (Serial.available())
		Serial.read();
}

bool WiFiManager::isConnected(void)
{
	return (WiFi.status() == WL_CONNECTED);
}

bool WiFiManager::canConnect(void)
{
	return (
			!this->isConnected()
			&& settings->getWifiCredentialsState() == WIFI_CREDENTIALS_STATE_OK
			&& this->hasSavedConnectionInfo());
}

bool WiFiManager::hasSavedConnectionInfo()
{
	return (!apInfoSaved().isEmpty());
}

bool WiFiManager::isWiFiEnabled()
{
	return settings->isWifiEnabled();
}

void WiFiManager::disableWiFi()
{
	settings->setWifiEnabled(false);
}

void WiFiManager::enableWiFi()
{
	settings->setWifiEnabled(true);
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

void WiFiManager::doYesNoSelection(void (WiFiManager::*func)(), const char *confirmation_string, const char *approval_string, const char *denial_string)
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

void WiFiManager::setNVSAPConnectionInfo(const char *ssid, const char *password,
										 wifi_credentials_state_e credentials_state)
{
	settings->setWifiAccessPointSsid(ssid);
	settings->setWifiPassword(password);
	settings->setWifiCredentialsState(credentials_state);
}

void WiFiManager::runWiFiLogging(const char *buf0, const char *buf1, const char *buf2, const char *buf3)
{
	udp.beginPacket(this->ipaddr_udp, this->port_udp);
	udp.write((uint8_t *)buf0, SIZE_LOG_BUFFER0-1);
	udp.write((uint8_t *)buf1, SIZE_LOG_BUFFER1-1);
	udp.write((uint8_t *)buf2, SIZE_LOG_BUFFER2-1);
	udp.write((uint8_t *)buf3, SIZE_CHECKSUM_BUFFER-1);
	udp.endPacket();
}
