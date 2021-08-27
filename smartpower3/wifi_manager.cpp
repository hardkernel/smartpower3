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
	// TODO: Is there a way to do this in the WebServer callback for Ajax? Without dying?
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

	// Start web server automatically
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
				"encrypted": true
			},
			{
				"ssid": "ssid2",
				"macaddr": "ab:cd:...",
				"rssi": -50,
				"encrypted": false
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

		serialLogLine("Send the networks in JSON format");
		serializeJson(jsonDoc, apListJson);
		request->send(200, "application/json", apListJson.c_str());

		// Delete the JSON document
		jsonDoc.clear();
	});

	// Connect to an AP
	webServer->on("/api/connect_to_ap", HTTP_GET, [&](AsyncWebServerRequest *request) {
		int actualJsonSize = 0;
		std::string connResultsJson;

		// TODO: Disconnect first if the previous connection is still active
		WiFi.begin(
			request->arg("ssid").c_str(),
			request->arg("password") == "" ? (char *) __null : request->arg("password").c_str()
			// 0,
			// reinterpret_cast<const uint8_t*>(request->arg("macaddr").c_str())
		);

		serialLog("Connecting to AP [" + request->arg("ssid") + "] ");
		Serial.print("that has MAC address [" + request->arg("macaddr") + "] ");
		if (request->arg("password") == "") {
			Serial.println("with no password ...");
		} else {
			Serial.println("with password [" + request->arg("password") + "] ...");
		}
		serialLogLine("Waiting for the STA connection in the background task...");
		serialLogLine("The website should request about whether the STA connection established or not");

		actualJsonSize = JSON_OBJECT_SIZE(3) + WIFI_EXTRA_CAPACITY_FOR_JSON;

		// At least 1024 is recommended for heap allocation
		DynamicJsonDocument jsonDoc(actualJsonSize < 1024 ? 1024 : actualJsonSize);

		/*
		{
			"success": true,
			"ipAddress": "192.168.0.2",
			"error": false,
			"errorMessage": "message",
		}
		 */
		// If there's an error about starting the connecting to AP, how can I know?
		jsonDoc["success"] = true;
		jsonDoc["error"] = false;
		jsonDoc["errorMessage"] = "";

		serialLogLine("Send the requesting results in JSON format");
		serializeJson(jsonDoc, connResultsJson);
		request->send(200, "application/json", connResultsJson.c_str());

		// Delete the JSON document
		jsonDoc.clear();
	});

	// Is the STA connected?
	webServer->on("/api/is_sta_connected", HTTP_GET, [&](AsyncWebServerRequest *request) {
		int actualJsonSize = 0;
		std::string connResultsJson;

		actualJsonSize = JSON_OBJECT_SIZE(4) + WIFI_EXTRA_CAPACITY_FOR_JSON;

		// At least 1024 is recommended for heap allocation
		DynamicJsonDocument jsonDoc(actualJsonSize < 1024 ? 1024 : actualJsonSize);

		/*
		{
			"established": true,
			"ipAddress": "192.168.0.2",
			"error": false,
			"errorMessage": "message",
		}
		 */
		switch (WiFi.status()) {
			case WL_CONNECTED:
				jsonDoc["established"] = true;
				jsonDoc["ipAddress"] = WiFi.localIP().toString();
				jsonDoc["error"] = false;
				jsonDoc["errorMessage"] = "";
			break;
			case WL_NO_SSID_AVAIL:
				jsonDoc["established"] = false;
				jsonDoc["ipAddress"] = "";
				jsonDoc["error"] = true;
				jsonDoc["errorMessage"] = "Not found the given SSID";
			break;
			case WL_CONNECT_FAILED:
				jsonDoc["established"] = false;
				jsonDoc["ipAddress"] = "";
				jsonDoc["error"] = true;
				jsonDoc["errorMessage"] = "Connection failed with unknown reason";
			break;
			case WL_NO_SHIELD:
			case WL_IDLE_STATUS:
				// Currently in idle state or waiting for the trying to connection to be finished
				jsonDoc["established"] = false;
				jsonDoc["ipAddress"] = "";
				jsonDoc["error"] = false;
				jsonDoc["errorMessage"] = "";
			break;
			default:
				jsonDoc["established"] = false;
				jsonDoc["ipAddress"] = "";
				jsonDoc["error"] = true;
				jsonDoc["errorMessage"] = "Connection failed with unknown reason. Maybe the wrong password?";
			break;
		}

		serialLogLine("Send the STA connection results in JSON format");
		serializeJson(jsonDoc, connResultsJson);
		request->send(200, "application/json", connResultsJson.c_str());

		// Delete the JSON document
		jsonDoc.clear();
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
