/*
 * This file is part of the Hardkernel SmartPower 3 distribution.
 * Copyright (c) 2021 Deokgyu Yang <secugyu@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "screen.h"

// Soft AP mode configuration
#define WIFI_SOFT_AP_SSID "SmartPower 3"
#define WIFI_SOFT_AP_PASSWORD "12345678"

// Web server configuration
#define WIFI_SERVER_PORT 80

// Web socket configuration
#define WIFI_SOCKET_URL "/socket"

// ArduinoJson
// This size is calculatated by the ArduinoJson assistant
// This mumber is mulitplied by the number of bytes of the JSON array
// and summed with the number from the size calculating macros
#define WIFI_EXTRA_CAPACITY_FOR_JSON 84

// Tag for logging
#define WIFI_LOGTAG "WiFi"

// TODO: These should not be here
#define PROJECT_NAME "SmartPower 3"
#define MAX_CHANNEL_NUM 2
#define CHANNEL_0 0
#define CHANNEL_1 1

enum WifiServiceState {
	WIFI_SERVICE_ON = 0,
	WIFI_SERVICE_OFF
};

enum WifiConnectionState {
	WIFI_AP_DISCONNECTED = 0,
	WIFI_AP_CONNECTED
};

enum SocketCommand {
	SOCKET_COMMAND_NONE = 0,
	SOCKET_COMMAND_GET_CURRENT_POWER,
	SOCKET_COMMAND_GET_SETTINGS,
	SOCKET_COMMAND_SET_SETTINGS
};

struct WifiCurrentPower {
	uint8_t channel;
	bool isActive;
	uint16_t voltage;
	uint16_t ampere;
	uint16_t watt;
	uint16_t setVoltage;
	uint16_t setCurrentLimit;
};

class WifiManager {
public:
	static WifiManager &instance() {
		static WifiManager *instance = new WifiManager();
		return *instance;
	}

	WifiManager();

	static WifiCurrentPower currentPower[2];

	void init();
	void start();
	void stop();

	bool getWifiServiceState();
	int getConnectionState();

	void setCurrentPower(uint8_t channel, WifiCurrentPower currentPower);

	void serialLog(String message);
	void serialLogLine(String meesage);

private:
	AsyncWebServer *webServer;
	AsyncWebSocket *webSocket;
	int discoveredApCount;
	bool wifiServiceState;

	static void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
};

#endif
