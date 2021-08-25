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
#include <ArduinoJson.h>
#include "SPIFFS.h"

#include "wifi_manager.h"

WifiManager::WifiManager() {
	// Set the properties using the default value
	discoveredApCount = -1;
	apConnected = WIFI_AP_DISCONNECTED;
	wifiServiceState = WIFI_SERVICE_OFF;
}

void WifiManager::init() {
	webServer = new AsyncWebServer(WIFI_SERVER_PORT);

	// Run in the AP and STA mode simutaneously
	WiFi.mode(WIFI_MODE_APSTA);

	// Discover the APs before starting the server
	discoveredApCount = WiFi.scanNetworks(false, false, false, 500U, 0U);
	serialLogLine("Discovered networks count: " + String(discoveredApCount));
	for (int i = 0; i < discoveredApCount; i++) {
		Serial.printf("%s, %s, %d, %s\n",
			WiFi.SSID(i).c_str(),
			WiFi.BSSIDstr(i).c_str(),
			WiFi.RSSI(i),
			(WiFi.encryptionType(i) != WIFI_AUTH_OPEN) ? "true" : "false");
	}

	// For the AP mode
	WiFi.softAP(
		WIFI_SOFT_AP_SSID,
		WIFI_SOFT_AP_PASSWORD
	);
	serialLogLine("AP: Soft AP with IP: " + WiFi.softAPIP().toString());

	// Start WiFi service automatically
	start();
}

void WifiManager::start() {
	// Web Server Root URL
	webServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
		request->send(SPIFFS, "/website/index.html", "text/html");
	});

	// Return the discovered AP list
	webServer->on("/api/get_ap_list", HTTP_GET, [&](AsyncWebServerRequest *request) {
		int actualJsonSize = 0;
		std::string apListJson;

		// Have to edit the parameter for JSON_OBJECT_SIZE
		// if the members of the WifiDiscoveredAp structure is changed
		actualJsonSize =
			JSON_ARRAY_SIZE(discoveredApCount) +
			(discoveredApCount * JSON_OBJECT_SIZE(4)) +
			WIFI_EXTRA_CAPACITY_FOR_JSON * discoveredApCount;

		// At least 1024 is recommended for heap allocation
		DynamicJsonDocument jsonDoc(actualJsonSize < 1024 ? 1024 : actualJsonSize);

		/*
		[
			{
				"ssid": "ssid1",
				"macaddr": "ab:cd:...",
				"rssi": -50,
				"encryption": true
			},
			{
				"ssid": "ssid2",
				"macaddr": "ab:cd:...",
				"rssi": -50,
				"encryption": false
			}
		]
		 */
		for (int i = 0; i < discoveredApCount; i++) {
			JsonObject jsonObj = jsonDoc.createNestedObject();

			jsonObj["ssid"] = WiFi.SSID(i);
			jsonObj["macaddr"] = WiFi.BSSIDstr(i);
			jsonObj["rssi"] = WiFi.RSSI(i);
			jsonObj["encrypted"] = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
		}
		Serial.println("WiFi: Send the networks in JSON format:");
		serializeJson(jsonDoc, Serial);
		Serial.println();

		serializeJson(jsonDoc, apListJson);
		request->send_P(200, "text/html", apListJson.c_str());
	});

	webServer->serveStatic("/", SPIFFS, "/website");

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
