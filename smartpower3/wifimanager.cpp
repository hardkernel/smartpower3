#include "wifimanager.h"

// general
const char *MSG_ON = F("ON");
const char *MSG_OFF = F("OFF");
// menu items
const char *MSG_CMD_MODE_ENTERED = F(">>> WiFi command mode entered <<<");
const char *MSG_CMD_MODE_EXITED = F(">>> WiFi command mode exited <<<");
const char *MSG_CMD_UNKNOWN = F(">>> Unknown command <<<");
const char *MSG_CMD_NO_NETWORK = F(">>> No networks found <<<");
const char *MSG_CMD_SCAN_RUNNING = F(">>> Network scan in progress <<<");
const char *MSG_CMD_SCAN_FAILED = F(">>> Network scan failed <<<");
const char *MSG_CMD_NETWORK_UNKNOWN_ERROR = F(">>> Network unknown error <<<");
const char *MSG_CMD_SCANNING = F(">>> AP scanning <<<");
const char *MSG_CMD_CONNECT = F(">>> AP connecting <<<");
const char *MSG_CMD_SELECT = F(">>> AP select <<<");
const char *MSG_CMD_CONNECT_INFO = F(">>> AP connection info <<<");
const char *MSG_CMD_NO_CONNECT = F(">>> AP no connection <<<");
const char *MSG_CMD_SET_UDP = F(">>> Setting UDP <<<");
const char *MSG_CMD = F("Command : ");
const char *MSG_CMD_UDP_INFO = F(">>> UDP server connection info for logging <<<");
const char *MSG_CMD_NO_UDP_INFO = F(">>> No UDP server info <<<");
const char *MSG_CMD_FORGET_CONNECTION = F(">>> Forget connection AP <<<");
const char *MSG_CMD_FORGET_UDP_SERVER_INFO = F(">>> Forget UDP server <<<");
const char *MSG_CMD_TURN_WIFI_ON_OFF = F("[Dis]Connect pre-saved WiFi network. Current state: ");
const char *MSG_CMD_TURN_LOGGING_ON_OFF = F("Switch logging ON or OFF (if logging interval is set). Current state: ");
const char *MSG_CMD_TURN_DHCP_ON_OFF = F("Switch DHCP ON or OFF. Current state: ");
const char *MSG_CMD_SET_STATIC_IP = F(">>> Setting Static IP Address <<<");
const char *MSG_CMD_SET_GATEWAY_IP = F(">>> Setting Gateway IP Address <<<");
const char *MSG_CMD_SET_SUBNET_MASK = F(">>> Setting Subnet Mask <<<");
const char *MSG_CMD_SET_DNS_SERVERS = F(">>> Settings DNS Servers <<<");
const char *MSG_CMD_SET_OPERAION_MODE = F("Switch Operation Mode. [Default=0, SCPI=1], Your selection? :");
// common
const char *MSG_YN_REDO_SELECTION_REQUEST = F("\n\rPlease enter \"y\" or \"n\" & Enter or press Ctrl+C: ");
// UDP server forget
const char *MSG_YN_UDP_FORGET_CONFIRMATION = F("Do you really want to forget the UDP server settings? (y/n & Enter): ");
//const char *MSG_YN_UDP_FORGET_SUCCESS = F("\n\rUDP server settings have been erased.");
//const char *MSG_YN_UDP_FORGET_FAILURE = F("\n\rUDP server settings have NOT been erased.");
// WiFi AP forget
const char *MSG_YN_AP_FORGET_CONFIRMATION = F("Do you really want to forget the network settings? (y/n & Enter): ");
//const char *MSG_YN_AP_FORGET_SUCCESS = F("\n\rWiFi access point data have been erased.");
//const char *MSG_YN_AP_FORGET_FAILURE = F("\n\rWiFi access point data have NOT been erased.");
// UDP server setting string
const char *MSG_UDP_SERVER_IP_ADDRESS = F("Input your IP address of UDP server (for example \"192.168.0.5\"): ");
const char *MSG_UDP_SERVER_IP_ADDRESS_PORT = F("Input your port number of UDP server (for example \"6000\"): ");
const char *MSG_UDP_SERVER_UPDATE_SUCCESS = F("Successfully set UDP server address and port: %s:%d\r\n");
// Switch WiFi state setting string
const char *MSG_YN_WIFI_SWITCH_STATE_CONFIRMATION = F("Do you really want to switch WiFi state? (y/n & Enter): ");
//const char *MSG_YN_WIFI_SWITCH_STATE_SUCCESS = F("\n\rWiFi state successfully changed.");
//const char *MSG_YN_WIFI_SWITCH_STATE_FAILURE = F("\n\rWiFi state not changed.");
// Switch logging on/off
const char *MSG_YN_LOGGING_SWITCH_STATE_CONFIRMATION = F("Do you really want to switch logging state? (y/n & Enter): ");
//const char *MSG_YN_LOGGING_SWITCH_STATE_SUCCESS = F("\n\rLogging state successfully changed.");
//const char *MSG_YN_LOGGING_SWITCH_STATE_FAILURE = F("\n\rLogging state not changed.");
// Switch DHCP on/off
const char *MSG_YN_DHCP_SWITCH_STATE_CONFIRMATION = F("Do you really want to switch DHCP state? (y/n & Enter): ");
//const char *MSG_YN_DHCP_SWITCH_STATE_SUCCESS = F("\n\rDHCP state successfully changed.");
//const char *MSG_YN_DHCP_SWITCH_STATE_FAILURE = F("\n\rDHCP state not changed.");
// Switch Operation mode
const char *MSG_OPERATION_MODE_UPDATE_SUCCESS = F("Device operation mode successfully set to %s.\r\n");
// Set Static IP
const char *MSG_STATIC_IP_ADDRESS = F("Input static IP address of your device (for example \"192.168.0.5\"): ");
const char *MSG_STATIC_IP_ADDRESS_UPDATE_SUCCESS = F("Successfully set up Static IP Address: %s\r\n");
// Set Gateway IP
const char *MSG_GATEWAY_IP_ADDRESS = F("Input Gateway IP address (for example \"192.168.1.1\"): ");
const char *MSG_GATEWAY_IP_ADDRESS_UPDATE_SUCCESS = F("Successfully set up Gateway IP Address: %s\r\n");
// Subnet mask
const char *MSG_SUBNET_MASK_IP_ADDRESS = F("Input Subnet mask (for example \"255.255.255.0\"): ");
const char *MSG_SUBNET_MASK_IP_ADDRESS_UPDATE_SUCCESS = F("Successfully set up Subnet Mask: %s\r\n");
// DNS servers
const char *MSG_DNS_SERVER_IP_ADDRESS = F("Input %s DNS Server IP Address (for example \"192.168.1.1\"): ");
const char *MSG_DNS_SERVER_IP_ADDRESS_SUCCESS = F("Successfully set DNS server addresses: %s\r\n");
// AP selection
const char *MSG_AP_SELECT = F("Select AP Number (0 - %d) & Enter : ");
const char *MSG_AP_SELECT_PASSWORD = F("Please enter password & press Enter : ");

const char *MSG_OPERATION_SUCCEEDED = F("Operation succeeded.");
const char *MSG_OPERATION_ABORTED = F("Operation aborted.");

const char *encryption_str(int encryption)
{
	switch(encryption) {
		case    WIFI_AUTH_OPEN:             return  F("Open");
		case    WIFI_AUTH_WEP:              return  F("WEP");
		case    WIFI_AUTH_WPA_PSK:          return  F("WPA PSK");
		case    WIFI_AUTH_WPA2_PSK:         return  F("WPA2 PSK");
		case    WIFI_AUTH_WPA_WPA2_PSK:     return  F("WPA WPA2 PSK");
		case    WIFI_AUTH_WPA2_ENTERPRISE:  return  F("WPA2 Enterprise");
		default:                            return  F("Unknown");
	}
}

WiFiManager::WiFiManager(Settings *settings)
{
	WiFi.mode(WIFI_STA);
	WiFi.disconnect(true, true);
	delay(100);
	udp.begin(WIFI_UDP_PORT);

	this->udp = udp;
	this->settings = settings;
}

void WiFiManager::viewMainMenu(void)
{
	Serial.println();

	if (selected_submenu == 0) {
		for (menu_item *mi : main_cmd_menu) {
			Serial.println(mi->label);
			current_submenu_item_count = 0;
		}
	} else {
		current_submenu_item_count = 0;
		for (menu_item mi : main_cmd_menu[selected_submenu-1]) {
			if (mi.label != NULL) {
				Serial.println(mi.label);
				current_submenu_item_count++;
			}
		}
	}

	if (this->isCommandMode()) {
		Serial.printf(F("%s"), MSG_CMD);
	}
}

void WiFiManager::viewApList(int16_t ap_list_cnt)
{
	// scanNetworks in ap_scanning can return negative value indicating failure
	// or unfinished scan - see WiFiType.h
	if (ap_list_cnt > 0) {
		Serial.printf(F("[ Networks found(%d) ]\n\r"), ap_list_cnt);
		for (int16_t i = 0; i < ap_list_cnt; i++) {
			Serial.printf(F("%d - %s (%d dbm), Encryption (%s)\n\r"),
							i,
							WiFi.SSID(i).c_str(),
							WiFi.RSSI(i),
							encryption_str(WiFi.encryptionType(i)));
		}
	} else if (!ap_list_cnt) {
		Serial.println(MSG_CMD_NO_NETWORK);
	} else if (ap_list_cnt == WIFI_SCAN_RUNNING) {
		Serial.println(MSG_CMD_SCAN_RUNNING);
	} else if (ap_list_cnt == WIFI_SCAN_FAILED) {
		Serial.println(MSG_CMD_SCAN_FAILED);
	} else {
		Serial.println(MSG_CMD_NETWORK_UNKNOWN_ERROR);
	}
}

int16_t WiFiManager::apScanning(void)
{
	Serial.println(MSG_CMD_SCANNING);
	WiFi.disconnect(true, true);  // needs to be disconnected between successive scans
	return WiFi.scanNetworks();
}

bool WiFiManager::apConnect(const char* ssid, const char* passwd, bool show_ctrl_c_info)
{
	char cmd;
	uint8_t i = 0;

	WiFi.disconnect(true, true);
	this->checkAndSetDhcpSettings();
	WiFi.begin(ssid, passwd);

	if (show_ctrl_c_info) {
		this->serialPrintCtrlCNotice();
	}
	Serial.printf(F("Connecting... %s\n\r"), ssid);

	while((i++ < CONNECT_RETRY_CNT)) {
		if (WiFi.status() == WL_CONNECTED) {
			Serial.printf(F("[[[ Connection Okay ]]]\n\r"));
			Serial.printf(F("Connected AP : %s(%d dbm), local IP : %s\n\r"),
					WiFi.SSID().c_str(),
					WiFi.RSSI(),
					WiFi.localIP().toString().c_str());
			this->setStorageAPConnectionInfo(ssid, passwd, WIFI_CREDENTIALS_STATE_OK);
			return true;
		}
		Serial.printf(F("\nConnecting wait (%d)...\n\r"), CONNECT_RETRY_CNT - i);

		if (Serial.available()) {
			cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				settings->setWifiCredentialsState(WIFI_CREDENTIALS_STATE_NOT_CHECKED);
				WiFi.disconnect(true, true);
				return false;
			}
		}
		delay(1000);
	}

	if (settings->getWifiCredentialsState() == WIFI_CREDENTIALS_STATE_OK) {
		Serial.printf(F("\nReverting to previous AP\n\r"));
	} else {
		Serial.printf(F("\n%s Connect failed\n\r"), ssid);
	}
	settings->setWifiCredentialsState(WIFI_CREDENTIALS_STATE_NOT_CHECKED);

	return false;
}

bool WiFiManager::apConnect(uint8_t ap_number, char *passwd)
{
	String ssid;
	ssid.reserve(256);

	ssid = WiFi.SSID(ap_number);

	Serial.println(MSG_CMD_CONNECT);
	Serial.printf(F("%s (%d dbm) %s\n\r"),
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
		WiFi.disconnect(true, true);
		this->checkAndSetDhcpSettings();
		WiFi.begin(
				settings->getWifiAccessPointSsid().c_str(),
				settings->getWifiPassword().c_str());
		if (WiFi.waitForConnectResult(CONNECT_RETRY_CNT * 1000) == WL_CONNECTED) {
			this->setStorageAPConnectionInfo(
					settings->getWifiAccessPointSsid().c_str(),
					settings->getWifiPassword().c_str(),
					WIFI_CREDENTIALS_STATE_OK);
			return true;
		}
	}
	settings->setWifiCredentialsState(WIFI_CREDENTIALS_STATE_INVALID);
	return false;
}

void WiFiManager::checkAndSetDhcpSettings(void)
{
	if (settings->isWifiIpv4DhcpEnabled()) {
		WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
	} else {
		WiFi.config(
				settings->getWifiIpv4StaticIp(),
				settings->getWifiIpv4GatewayAddress(),
				settings->getWifiIpv4SubnetMask(),
				settings->getWifiIpv4AddressDns1() or INADDR_NONE,
				settings->getWifiIpv4AddressDns2() or INADDR_NONE);
	}
}

void WiFiManager::apSetPassword(uint8_t ap_number)
{
	char passwd[64], cmd; // 64 chars is the longest password currently possible
	uint8_t pos = 0, sizeof_passwd = sizeof(passwd);

	memset(passwd, 0x00, sizeof_passwd);

	Serial.printf(F("\n\rAP (%s, %d dbm) \n\r"),
			WiFi.SSID(ap_number).c_str(),
			WiFi.RSSI(ap_number));

	if (WiFi.encryptionType(ap_number) == WIFI_AUTH_OPEN) {
		this->apConnect(ap_number, passwd);
		return;
	}

	Serial.printf(MSG_AP_SELECT_PASSWORD);
	Serial.read();  // In some terminals, there is already one character in the pipeline that needs to be removed.

	while(true) {
		if (Serial.available()) {
			cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				return;
			} else if ( cmd == SERIAL_ENTER ) {
				Serial.print(F("\n\r"));
				this->apConnect(ap_number, passwd);
				return;
			} else {
				Serial.print(F("*"));
				passwd[pos++] = cmd;
			}
			this->checkAndResetIndexAndValue(
					pos,
					*passwd,
					sizeof_passwd,
					F("\n\rPassword longer than possible."),
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

	Serial.println(MSG_CMD_SELECT);
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
				Serial.printf(F("%c"), cmd);
				if (this->isDigitChar(cmd) && (pos < sizeof_sel_ap_number -1)) {
					sel_ap_number[pos++] = cmd;
				} else {
					pos = sizeof_sel_ap_number;
				}
			}
			connect_ap_number = strtoul(&sel_ap_number[0], NULL, 10);
			if ((connect_ap_number > ap_list_cnt-1) || pos > sizeof_sel_ap_number-1) {
				Serial.println(F(">>> Error selecting AP number <<<"));
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
	this->setStorageAPConnectionInfo("", "", WIFI_CREDENTIALS_STATE_INVALID);
	delay(100);
	update_wifi_info = true;
}

void WiFiManager::apForget()
{
	Serial.println(MSG_CMD_FORGET_CONNECTION);
	this->doYesNoSelection(
			&WiFiManager::doApForget,
			MSG_YN_AP_FORGET_CONFIRMATION,
			MSG_OPERATION_SUCCEEDED,
			MSG_OPERATION_ABORTED
	);
}

void WiFiManager::apDisconnectAndTurnWiFiOff()
{
	WiFi.disconnect(true, true);
}

void WiFiManager::udpServerInfo()
{
	Serial.println(MSG_CMD_UDP_INFO);
	Serial.printf(F("IP Address [%s]\n\r"), ipaddr_udp.toString().c_str());
	Serial.printf(F("Port [%d]\n\r"), port_udp);
}

void WiFiManager::apInfo()
{
	if (WiFi.status() == WL_CONNECTED) {
		Serial.printf(F("%s\n\r"), MSG_CMD_CONNECT_INFO);
		Serial.printf(F("%s (%d dbm)\n\r"), WiFi.SSID().c_str(), WiFi.RSSI());
		Serial.printf(F("IP Address [%s]\n\r"), WiFi.localIP().toString().c_str());
		Serial.printf(F("MAC Address [%s]\n\r"), WiFi.macAddress().c_str());
	} else if (!this->apInfoSaved().isEmpty()) {
		Serial.println(MSG_CMD_CONNECT_INFO);
		Serial.printf(F("%s [saved connection data]\n\r"), this->apInfoSaved().c_str());
	} else {
		Serial.println(MSG_CMD_NO_CONNECT);
	}
}

String WiFiManager::apInfoSaved()
{
	return settings->getWifiAccessPointSsid();
}

String WiFiManager::apInfoConnected()
{
	char wifi_info[35];
	snprintf(wifi_info, 35, F("%s : %s"), WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
	return String(wifi_info);
}

void WiFiManager::setUdp()
{
	IPAddress ipaddr;
	uint16_t port;

	Serial.println(MSG_CMD_SET_UDP);
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
	return this->serialGetIPAddress(MSG_UDP_SERVER_IP_ADDRESS);
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
				Serial.printf(F("%c"), cmd);
				if (this->isDigitChar(cmd)) {
					port[pos++] = cmd;
				} else {
					pos = sizeof_port;
				}
			}
			this->checkAndResetIndexAndValue(pos, *port, sizeof_port, F("\n\rWrong port number."), MSG_UDP_SERVER_IP_ADDRESS_PORT);
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
		Serial.println(F("\n\rPlease set connection settings first!"));
	}
}

void WiFiManager::switchWiFiConnection()
{
	Serial.printf(F("%s%s\n\r"), MSG_CMD_TURN_WIFI_ON_OFF, this->isConnected() ? MSG_ON : MSG_OFF);
	this->doYesNoSelection(
			&WiFiManager::doSwitchWiFiState,
			MSG_YN_WIFI_SWITCH_STATE_CONFIRMATION,
			MSG_OPERATION_SUCCEEDED,
			MSG_OPERATION_ABORTED
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
	Serial.printf(F("%s\n\r"), MSG_CMD_FORGET_UDP_SERVER_INFO);
	this->doYesNoSelection(
			&WiFiManager::doUdpServerForget,
			MSG_YN_UDP_FORGET_CONFIRMATION,
			MSG_OPERATION_SUCCEEDED,
			MSG_OPERATION_ABORTED
	);
}

void WiFiManager::doSwitchLogging()
{
	if (settings->isLoggingEnabled()) {
		settings->setLoggingEnabled(false);
	} else {
		settings->setLoggingEnabled(true);
	}
}

void WiFiManager::switchLoggingOnOff()
{
	Serial.printf(F("%s%s\n\r"), MSG_CMD_TURN_LOGGING_ON_OFF, settings->isLoggingEnabled() ? MSG_ON : MSG_OFF);
	this->doYesNoSelection(
			&WiFiManager::doSwitchLogging,
			MSG_YN_LOGGING_SWITCH_STATE_CONFIRMATION,
			MSG_OPERATION_SUCCEEDED,
			MSG_OPERATION_ABORTED
	);
}

void WiFiManager::WiFiMenuMain(char idata)
{
	Serial.println(idata);
	Serial.println();

	if (this->isDigitChar(idata)) {
		uint8_t num = idata-'0';

		if (0 < num && num < 6 && current_menu_level == 0) {
			selected_submenu = num;
			current_menu_level = 1;
			if (((main_cmd_menu[selected_submenu-1][0]).callback) != nullptr) {
				(this->*((main_cmd_menu[selected_submenu-1][0]).callback))();
			}
		} else if (
				current_menu_level > 0
				&& current_submenu_item_count > num
				&& ((main_cmd_menu[selected_submenu-1][num]).callback) != nullptr)
		{
			(this->*((main_cmd_menu[selected_submenu-1][num]).callback))();
		} else {
			Serial.println(MSG_CMD_UNKNOWN);
		}
	} else {
		Serial.println(MSG_CMD_UNKNOWN);
	}

	if (isCommandMode()) {
		viewMainMenu();
		while (Serial.available())
			Serial.read();
	}
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

void WiFiManager::enterCommandMode(void)
{
	commandMode = true;
	Serial.printf(F("\n\r%s\n\r"), MSG_CMD_MODE_ENTERED);
}

void WiFiManager::serialPrintCtrlCNotice()
{
	Serial.println(F("**************************************************"));
	Serial.println(F("*                                                *"));
	Serial.println(F("*           For exit press [Ctrl+c]...           *"));
	Serial.println(F("*                                                *"));
	Serial.println(F("**************************************************"));
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
	char buf = 0x00;
	char cmd;

	Serial.print(confirmation_string);

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
							Serial.println();
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
					Serial.print(cmd);
					buf = cmd;
			}
		}
	}
}

void WiFiManager::setStorageAPConnectionInfo(const char *ssid, const char *password,
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

void WiFiManager::goToMainMenu(void)
{
	this->current_menu_level = 0;
	this->selected_submenu = 0;
}

void WiFiManager::switchDhcpOnOff(void)
{
	Serial.printf(F("%s%s\n\r"), MSG_CMD_TURN_DHCP_ON_OFF, settings->isWifiIpv4DhcpEnabled() ? MSG_ON : MSG_OFF);
	this->doYesNoSelection(
			&WiFiManager::doSwitchDhcp,
			MSG_YN_DHCP_SWITCH_STATE_CONFIRMATION,
			MSG_OPERATION_SUCCEEDED,
			MSG_OPERATION_ABORTED
	);
	update_dhcp_info = true;
}

void WiFiManager::doSwitchDhcp(void)
{
	if (settings->isWifiIpv4DhcpEnabled()) {
		settings->setWifiIpv4DhcpEnabled(false);
	} else {
		settings->setWifiIpv4DhcpEnabled(true);
	}
}

void WiFiManager::setStaticIPAddress(void)
{
	Serial.println(MSG_CMD_SET_STATIC_IP);
	this->serialPrintCtrlCNotice();

	IPAddress ipaddr = this->serialGetIPAddress(MSG_STATIC_IP_ADDRESS);

	settings->setWifiIpv4StaticIp(ipaddr);
	update_static_ip_info = true;

	Serial.printf(MSG_STATIC_IP_ADDRESS_UPDATE_SUCCESS, ipaddr.toString().c_str());
}

void WiFiManager::setGatewayAddress(void)
{
	Serial.println(MSG_CMD_SET_GATEWAY_IP);
	this->serialPrintCtrlCNotice();

	IPAddress ipaddr = this->serialGetIPAddress(MSG_GATEWAY_IP_ADDRESS);

	settings->setWifiIpv4GatewayAddress(ipaddr);
	update_gateway_address_info = true;

	Serial.printf(MSG_GATEWAY_IP_ADDRESS_UPDATE_SUCCESS, ipaddr.toString().c_str());
}

void WiFiManager::setSubnetMask(void)
{
	Serial.println(MSG_CMD_SET_SUBNET_MASK);
	this->serialPrintCtrlCNotice();

	IPAddress ipaddr = this->serialGetIPAddress(MSG_SUBNET_MASK_IP_ADDRESS);

	settings->setWifiIpv4SubnetMask(ipaddr, true);
	update_subnet_info = true;

	Serial.printf(MSG_SUBNET_MASK_IP_ADDRESS_UPDATE_SUCCESS, ipaddr.toString().c_str());
}

void WiFiManager::setDnsServers(void)
{
	Serial.println(MSG_CMD_SET_DNS_SERVERS);
	this->serialPrintCtrlCNotice();

	char serial_message[68];

	sprintf(serial_message, MSG_DNS_SERVER_IP_ADDRESS, "first");
	IPAddress ipaddr_dns1 = this->serialGetIPAddress(serial_message);
	// Seems like terminal in VSCode leaves some chars in buffer or something (does not happen in Eclipse terminal)
	while (Serial.available()) {
		Serial.read();
	}
	sprintf(serial_message, MSG_DNS_SERVER_IP_ADDRESS, "second");
	IPAddress ipaddr_dns2 = this->serialGetIPAddress(serial_message);

	// With the current display, both are displayed at one go, so there's no need to do the update twice -
	// - hence the "false" at the first DNS server address
	settings->setWifiIpv4AddressDns1(ipaddr_dns1, false);
	settings->setWifiIpv4AddressDns2(ipaddr_dns2, true);

	sprintf(serial_message, "%s, %s", ipaddr_dns1.toString().c_str(), ipaddr_dns2.toString().c_str());
	Serial.printf(MSG_DNS_SERVER_IP_ADDRESS_SUCCESS, serial_message);
	update_dns_info = true;
}

IPAddress WiFiManager::serialGetIPAddress(const char* serial_message)
{
	char ipaddr[16], cmd;
	uint8_t pos = 0, sizeof_ipaddr = sizeof(ipaddr);
	IPAddress result_ipaddress;

	Serial.printf(serial_message);

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
				Serial.print(cmd);
				if (this->isDigitChar(cmd) || cmd == 46) // digit or dot
					ipaddr[pos++] = cmd;
				else
					pos = sizeof_ipaddr;
			}
			this->checkAndResetIndexAndValue(pos, *ipaddr, sizeof_ipaddr, F("\n\rWrong IP address."), serial_message);
		}
		delay(100);
	}
}

//TODO: Doublecheck selection mechanism
void WiFiManager::switchMode (void)
{
	Serial.printf(F(" %s"), MSG_CMD_SET_OPERAION_MODE);

	uint8_t mode = this->serialGetInt();

	settings->setOperationMode(static_cast<device_operation_mode>(mode), true);
	update_mode_info = true;

	Serial.printf(MSG_OPERATION_MODE_UPDATE_SUCCESS, mode == OPERATION_MODE_DEFAULT ? F("Default") : F("SCPI"));
}

device_operation_mode WiFiManager::getOperationMode(bool from_storage)
{
	return settings->getOperationMode(from_storage);
}

uint8_t WiFiManager::serialGetInt(void)
{
	char mode[2], cmd;
	uint8_t pos = 0, sizeof_mode = sizeof(mode);
	uint8_t resulting_mode;

	memset(mode, 0x00, sizeof_mode);

	while(true) {
		if (Serial.available()) {
			cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				return false;
			} else if ( cmd == SERIAL_ENTER ) {
				resulting_mode = atoi(mode);
				if (resulting_mode >= 0 && resulting_mode < 2) {
					Serial.println();
					return resulting_mode;
				} else {
					pos = sizeof_mode;
				}
			} else {
				Serial.print(cmd);
				if (this->isDigitChar(cmd)) {
					mode[pos++] = cmd;
				} else {
					pos = sizeof_mode;
				}
			}
			this->checkAndResetIndexAndValue(pos, *mode, sizeof_mode, F("\n\rWrong number."), MSG_UDP_SERVER_IP_ADDRESS_PORT);
		}
		delay(100);
	}
}

void WiFiManager::exitCommandMode(void)
{
	this->goToMainMenu();
	commandMode = false;
	Serial.println(MSG_CMD_MODE_EXITED);
}
