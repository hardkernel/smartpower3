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

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

#include "wifi_manager.h"

WifiManager::WifiManager() {
	// Set the properties using the default value
	apConnected = WIFI_AP_DISCONNECTED;
	wifiServiceState = WIFI_SERVICE_OFF;
}

void WifiManager::init() {
	webServer = new AsyncWebServer(WIFI_SERVER_PORT);

	// Run in the AP and STA mode simutaneously
	WiFi.mode(WIFI_MODE_APSTA);

	// For the AP mode
	WiFi.softAP(
		WIFI_SOFT_AP_SSID,
		WIFI_SOFT_AP_PASSWORD
	);

	serialLogLine("AP: Soft AP with IP: " + WiFi.softAPIP().toString());

	// For the STA mode
	WiFi.begin(
		WIFI_DEBUG_CONNECT_AP_SSID,
		WIFI_DEBUG_CONNECT_AP_PASSWORD
	);

	serialLogLine("Connecting to WiFi AP: " + String(WIFI_DEBUG_CONNECT_AP_SSID));
	serialLog("...");
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(1000);
	}
	Serial.print('\n');
	serialLogLine("STA: Connected to the AP with IP: " + WiFi.localIP().toString());
	apConnected = WIFI_AP_CONNECTED;

	// Start WiFi service automatically
	start();
}

void WifiManager::start() {
	// Web Server Root URL
	webServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/index.html", "text/html");
	});

	webServer->serveStatic("/", SPIFFS, "/");

	// Start server
	webServer->begin();

	wifiServiceState = WIFI_SERVICE_ON;
	serialLogLine("Web service started");
}

void WifiManager::stop() {
	webServer->end();
	WiFi.disconnect();

	wifiServiceState = WIFI_SERVICE_OFF;
	serialLogLine("Web service stopped");
}

bool WifiManager::getWifiServiceState() {
	return wifiServiceState;
}

int WifiManager::getConnectionState() {
	switch (WiFi.status()) {
		case WL_CONNECTED:
			return WIFI_AP_CONNECTED;
		default:
			// Handle it as disconnected if not connected
			return WIFI_AP_DISCONNECTED;
	}
}

void WifiManager::serialLog(String message) {
	Serial.print(WIFI_LOGTAG);
	Serial.print(": ");
	Serial.print(message.c_str());
}

void WifiManager::serialLogLine(String message) {
	Serial.print(WIFI_LOGTAG);
	Serial.print(": ");
	Serial.println(message.c_str());
}
