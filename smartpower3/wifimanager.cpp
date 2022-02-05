#include "wifimanager.h"

const char *MSG_CMD_MODE_ENTERED = ">>> WiFi command mode entered <<<";
const char *MSG_CMD_MODE_EXITED = ">>> WiFi command mode exited <<<";
const char *MSG_CMD_UNKNOWN = ">>> Unknown command <<<";
const char *MSG_CMD_NO_NETWORK = ">>> No networks found <<<";
const char *MSG_CMD_SCANNING = ">>> AP scanning <<<";
const char *MSG_CMD_CONNECT = ">>> AP connecting <<<";
const char *MSG_CMD_SELECT = ">>> AP select <<<";
const char *MSG_CMD_CONNECT_INFO = ">>> AP connection info <<<";
const char *MSG_CMD_NO_CONNECT = ">>> AP no connnection <<<";
const char *MSG_CMD_SET_UDP = ">>> Setting UDP <<<";
const char *MSG_CMD = "Command : ";
const char *MSG_CMD_UDP_INFO = ">>> UDP server connection info for logging <<<";
const char *MSG_CMD_NO_UDP_INFO = ">>> No UDP server info <<<";

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

WiFiManager::WiFiManager(WiFiUDP &udp, WiFiClient &client)
{
	this->udp = udp;
	this->client = client;
}

void WiFiManager::view_main_menu(void)
{
    int i;

    Serial.printf("\n\r");
    for(i = 0; i < WIFI_CMD_MENU_CNT; i++)
        Serial.printf("%s\n\r", &WIFI_CMD_MENU[i][0]);

    Serial.printf("%s", MSG_CMD);
}

void WiFiManager::view_ap_list(int ap_list_cnt)
{
    char ap_name[64], i;

    if (ap_list_cnt) {
        Serial.printf("[ Networks found(%d) ]\n\r", ap_list_cnt);
        for (i = 0; i < ap_list_cnt; i++) {
            memset(ap_name, 0x00, sizeof(ap_name));
            WiFi.SSID(i).toCharArray(ap_name, sizeof(ap_name));
            Serial.printf("%d - %s (%d dbm), Encryption (%s)\n\r",
                            i,
                            &ap_name[0], 
                            WiFi.RSSI(i),
                            encryption_str(WiFi.encryptionType(i)));
        }
    }
    else
        Serial.printf("%s\n\r", MSG_CMD_NO_NETWORK);
}

void WiFiManager::ap_scanning(void)
{
    Serial.printf("%s\n\r", MSG_CMD_SCANNING);
    APListCount = WiFi.scanNetworks();
}

bool WiFiManager::ap_connect(String ssid, String passwd)
{
    char ap_name[64], ap_passwd[64];
	uint8_t i = 0;
	char cmd;

    WiFi.disconnect();
    ssid.toCharArray(ap_name, sizeof(ap_name));
    passwd.toCharArray(ap_passwd, sizeof(ap_passwd));

    WiFi.begin(ap_name, ap_passwd);

	Serial.println("**************************************************");
    Serial.println("*                                                *");
    Serial.println("*           For exit press [Ctrl+c]...           *");
    Serial.println("*                                                *");
	Serial.println("**************************************************");
    Serial.printf("Connecting... %s\n\r", ap_name);
    while((i++ < CONNECT_RETRY_CNT)) {
		if (WiFi.status() == WL_CONNECTED) {
			Serial.printf("[[[ Connection Okay ]]]\n\r");
			Serial.printf("Connected AP : %s(%d dbm), local IP : %s\n\r",
					WiFi.SSID().c_str(), WiFi.RSSI(), WiFi.localIP().toString().c_str());

			NVS.setString("ssid", ssid);
			NVS.setString("passwd", passwd);
			NVS.setString("wifi_conn_ok", "true");
			state = 1;

			return true;
		}
		Serial.printf("\nConnecting wait(%d)...\n\r", CONNECT_RETRY_CNT - i);
        if (Serial.available()) {
            cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				//NVS.setString("wifi_conn_ok", "false");
				WiFi.disconnect();
				return false;
			}
		}
        delay(1000);
    }

	if (NVS.getString("wifi_conn_ok")) {
		state = 0;
        Serial.printf("\nConnect other AP\n\r");
	} else {
		state = 2;
        Serial.printf("\n%s Connect failed\n\r", ap_name);
	}
	passwd = NVS.setString("wifi_conn_ok", "false");

    return false;
}

bool WiFiManager::ap_connect(int ap_number, char *passwd)
{
    char ap_name[64], i = 0;
	char cmd;

    WiFi.disconnect();
    WiFi.SSID(ap_number).toCharArray(ap_name, sizeof(ap_name));
    Serial.printf("%s\n\r", MSG_CMD_CONNECT);
    Serial.printf("%s (%d dbm) %s\n\r",
        ap_name,
        WiFi.RSSI(ap_number),
        encryption_str(WiFi.encryptionType(ap_number)));

    WiFi.begin(ap_name, passwd);

	Serial.println("**************************************************");
    Serial.println("*                                                *");
    Serial.println("*           For exit press [Ctrl+c]...           *");
    Serial.println("*                                                *");
	Serial.println("**************************************************");
    Serial.printf("Connecting... %s\n\r", ap_name);
    while(i++ < CONNECT_RETRY_CNT) {
		if (WiFi.status() == WL_CONNECTED) {
			Serial.printf("[[[ Connection Okay ]]]\n\r");
			Serial.printf("Connected AP : %s(%d dbm), local IP : %s\n\r",
					WiFi.SSID().c_str(), WiFi.RSSI(), WiFi.localIP().toString().c_str());

			NVS.setString("ssid", ap_name);
			NVS.setString("passwd", passwd);
			NVS.setString("wifi_conn_ok", "true");
			state = 1;

			return true;
		}
		Serial.printf("\nConnecting wait(%d)...\n\r", CONNECT_RETRY_CNT - i);

        if (Serial.available()) {
            cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				//NVS.setString("wifi_conn_ok", "false");
				WiFi.disconnect();
				return false;
			}
		}

        delay(1000);
    }

	if (NVS.getString("wifi_conn_ok") == "true") {
        Serial.printf("\nConnect other AP\n\r");
		state = 0;
	} else {
        Serial.printf("\n%s Connect failed\n\r", ap_name);
		state = 2;
	}
	NVS.setString("wifi_conn_ok", "false");

    return false;
}

void WiFiManager::ap_set_passwd(int ap_number)
{
    char passwd[64], cmd;
    int pos = 0;

    memset(passwd, 0x00, sizeof(passwd));

    while(true) {
        if (WiFi.encryptionType(ap_number) == WIFI_AUTH_OPEN) {
            isConnectedAP = ap_connect(ap_number, passwd);
            return;
        } 
        if (Serial.available()) {
            cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				return;
			} else if ( cmd == SERIAL_ENTER ) {
                Serial.printf("\n\r");
                isConnectedAP = ap_connect(ap_number, passwd);
				if (isConnectedAP) {
                    char buf[64];
                    memset(buf, 0x00, sizeof(buf));
                    WiFi.SSID(ap_number).toCharArray(buf, sizeof(buf));
					nvs_ssid = String(buf);
					nvs_passwd = String(passwd);
				}
                return;
            }
            else {
                Serial.printf("*");
                passwd[pos++] = cmd;
            }
        }
        delay(100);
    }
}

void WiFiManager::ap_select(int ap_list_cnt)
{
    char cmd, sel_ap_number[5];
    int pos = 0;
    memset(sel_ap_number, 0x00, sizeof(sel_ap_number));

	Serial.println("**************************************************");
    Serial.println("*                                                *");
    Serial.println("*           For exit press [Ctrl+c]...           *");
    Serial.println("*                                                *");
	Serial.println("**************************************************");
    Serial.printf("%s\n\r", MSG_CMD_SELECT);
    Serial.printf("Select AP Number (0 - %d) & Enter : ", ap_list_cnt-1);

    while(true) {
        if (Serial.available()) {
            cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				return;
			} else if ( cmd >= '0' || cmd < '9' ) {
                if (pos < sizeof(sel_ap_number) -1)
                    sel_ap_number[pos++] = cmd;
                Serial.printf("%c", cmd);
            } else {
                Serial.printf("%s\n\r", MSG_CMD_UNKNOWN);
                return;
            }

            if ( cmd == SERIAL_ENTER ) {
                Serial.printf("\n\r");

                ConnectAP_Number = strtoul(&sel_ap_number[0], NULL, 10);
                Serial.printf("Select number : %d \n\r", ConnectAP_Number);

                if (ConnectAP_Number > ap_list_cnt-1) {
                    Serial.printf(">>> Error ap select number <<<\n\r");
                    Serial.printf("Select AP Number (0 - %d) & Enter : ", ap_list_cnt-1);
                    pos = 0;
                    memset(sel_ap_number, 0x00, sizeof(sel_ap_number));
                } else {
                    char buf[64];
                    // ap select & passwd read
                    memset(buf, 0x00, sizeof(buf));
                    WiFi.SSID(ConnectAP_Number).toCharArray(buf, sizeof(buf));
                    Serial.printf("\nAP(%s, %d dbm) Password & Enter : ",
                        &buf[0],
                        WiFi.RSSI(ConnectAP_Number));
                    ap_set_passwd(ConnectAP_Number);
                    return;
                }
            }
        }
        delay(100);
    }
}

void WiFiManager::udp_server_info()
{
    Serial.printf("IP Address [%s]\n\r", ipaddr_udp.toString().c_str());
    Serial.printf("Port [%d]\n\r", port_udp);
}

void WiFiManager::ap_info(int ap_number)
{
	String ip_addr, mac_addr, ssid;
        
    ip_addr = WiFi.localIP().toString();
    mac_addr = WiFi.macAddress();

    ssid = WiFi.SSID();

    Serial.printf("%s (%d dbm))\n\r", ssid.c_str(), WiFi.RSSI());
        
    Serial.printf("IP Address [%s]\n\r", ip_addr.c_str());
    Serial.printf("MAC Address [%s]\n\r", mac_addr.c_str());
}

void WiFiManager::set_udp()
{
    char ipaddr[16], port[5], cmd;
    uint8_t pos = 0;
	bool idx = 0;


	Serial.println("**************************************************");
    Serial.println("*                                                *");
    Serial.println("*           For exit press [Ctrl+c]...           *");
    Serial.println("*                                                *");
	Serial.println("**************************************************");

	Serial.println("Input your IP address of UDP server ex) 192.168.0.5:");

    memset(ipaddr, 0x00, sizeof(ipaddr));
	memset(port, 0x00, sizeof(port));

    while(true) {
        if (Serial.available()) {
            cmd = Serial.read();
			if (cmd  == SERIAL_CTRL_C) {
				return;
			} else if ( cmd == SERIAL_ENTER ) {
				if (idx == 0) {
					Serial.printf("\n\r");
					Serial.println(ipaddr_udp.fromString(ipaddr));
					if (ipaddr_udp.fromString(ipaddr)) {
						Serial.printf("IP address set ok: ");
						Serial.println(ipaddr_udp.toString());
						NVS.setString("ipaddr_udp", ipaddr);
						Serial.printf("\n\rInput your port number of UDP server ex) 6000:\n\r");
						idx = 1;
						pos = 0;
					} else {
						pos = 16;
					}
				} else {
					Serial.printf("\n\r");
					Serial.println(port);
					uint16_t _port = atoi(port);
					if (_port > 0 and _port < 10000) {
						Serial.printf("port set ok: ");
						Serial.println(_port);
						NVS.setInt("port_udp", _port);
						port_udp = _port;
						update_udp_info = true;
						return;
					} else {
						pos = 5;
					}
				}
            } else {
                Serial.printf("%c", cmd);
				if (idx == 0) {
					if ((cmd > 47) && (cmd < 58))
						ipaddr[pos++] = cmd;
					else if (cmd == 46)
						ipaddr[pos++] = cmd;
					else
						pos = 16;
				} else {
					if ((cmd > 47) && (cmd < 58))
						port[pos++] = cmd;
					else
						pos = 5;
				}
            }
			if (idx == 0) {
				if (pos > 15) {
					Serial.printf("Wrong IP address.\n\r");
					Serial.println("Input your IP address of UDP server ex) 192.168.0.5:");
					pos = 0;
					memset(ipaddr, 0x00, sizeof(ipaddr));
				}
			} else {
				if (pos > 4) {
					Serial.printf("Wrong port number.\n\r");
					Serial.println("Input your port number of UDP server ex) 6000:");
					pos = 0;
					memset(port, 0x00, sizeof(port));
				}
			}
        }
        delay(100);
    }
}

void WiFiManager::cmd_main(char idata)
{
    Serial.println(idata);

    switch(idata) {
        case    '1':
            if (WiFi.status() == 3) {
                Serial.printf("%s\n\r", MSG_CMD_CONNECT_INFO);
                ap_info(ConnectAP_Number);
            } 
            else
                Serial.printf("%s\n\r", MSG_CMD_NO_CONNECT);
        break;
        case    '2':
			Serial.printf("%s\n\r", MSG_CMD_UDP_INFO);
			udp_server_info();
        break;
        case    '3':
            ap_scanning();
            view_ap_list(APListCount);
            if (APListCount)
                ap_select(APListCount);
        break;
        case    '4':
			Serial.printf("%s\n\r", MSG_CMD_SET_UDP);
            set_udp();
        break;
        case    '5':
        case    SERIAL_CTRL_C:
            commandMode = false;
            Serial.printf("%s\n\r", MSG_CMD_MODE_EXITED);
        return;
        default :
            Serial.printf("%s\n\r", MSG_CMD_UNKNOWN);
        break;
    }

    view_main_menu();
    while(Serial.available())
        Serial.read();
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
