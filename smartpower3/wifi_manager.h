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

// WiFi configuration
#define WIFI_SERVER_PORT 80

// Tag for logging
#define WIFI_LOGTAG "WIFI"

// TODO: For debugging, have to implement the connection process for an AP
#define WIFI_DEBUG_CONNECT_AP_SSID "HK_JOSHUA"
#define WIFI_DEBUG_CONNECT_AP_PASSWORD "hard4624"

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
	WIFI_AP_CONNECTED = 0,
	WIFI_AP_DISCONNECTED
};

class WifiManager {
public:
	static WifiManager &instance() {
		static WifiManager *instance = new WifiManager();
		return *instance;
	}

	WifiManager();

	void init();
	void start();
	void stop();

	bool getWifiServiceState();
	int getConnectionState();

	void serialLog(String message);
	void serialLogLine(String meesage);

private:
	AsyncWebServer *webServer;

	bool apConnected;
	bool wifiServiceState;
};

#endif