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
#include "screen.h"

WifiCurrentPower WifiManager::currentPower[MAX_CHANNEL_NUM] = { 0 };

WifiManager::WifiManager() {
	// Set the properties using the default value
	discoveredApCount = -1;
	apConnected = WIFI_AP_DISCONNECTED;
	wifiServiceState = WIFI_SERVICE_OFF;
}

void WifiManager::init() {
	webServer = new AsyncWebServer(WIFI_SERVER_PORT);
	webSocket = new AsyncWebSocket(WIFI_SOCKET_URL);

	// Run in the AP and STA mode simutaneously
	WiFi.mode(WIFI_MODE_APSTA);

	// Discover the APs before starting the server
	// TODO: Have to change this to as a callback function from request by a client
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

	webServer->rewrite("/", "/index.html");
	webServer->rewrite("/index.html", "/index-ap.html").setFilter(ON_AP_FILTER);
	webServer->serveStatic("/", SPIFFS, "/website");

	webSocket->onEvent(onWebSocketEvent);
	webServer->addHandler(webSocket);

	// Start web server
	DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
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

void WifiManager::setCurrentPower(uint8_t channel, WifiCurrentPower currentPower) {
	WifiManager::currentPower[channel] = currentPower;
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

// The following contents have the basic debugging log codes of the ESPAsyncWebServer documents
void WifiManager::onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
	if(type == WS_EVT_CONNECT){
		//client connected
		Serial.printf("WiFi: ws[%s][%u] connect\n", server->url(), client->id());
	} else if(type == WS_EVT_DISCONNECT){
		//client disconnected
		Serial.printf("WiFi: ws[%s][%u] disconnect: %u\n", server->url(), client->id());
	} else if(type == WS_EVT_ERROR){
		//error was received from the other end
		Serial.printf("WiFi: ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
	} else if(type == WS_EVT_PONG){
		//pong message was received (in response to a ping request maybe)
		Serial.printf("WiFi: ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
	} else if(type == WS_EVT_DATA){
		int actualJsonSize = 0;
		std::string resultJson;

		// Parse the JSON data from client
		// The minimal memory allocation for DynamicJsonDocument is 1024 bytes
		DynamicJsonDocument receivedJsonDoc(1024);
		deserializeJson(receivedJsonDoc, (char *) data);

		/*
		 * The receiving JSON object has a command number which is one of the below:
		 * - 1: get the current power (power settings for each channel)
		 * - 2: get the current device settings (backlight level, fan level, ...)
		 * - 3: set the power/device related settings (activity, voltage, current limit for each channel, backlight level, fanspeed level, ...)
		 * >>> These numbers equivalent to the SocketCommand enum values
		 *
		 * Here is the example of the received JSON object:
		{
			"command": 1,
			"data": {
				"channel": 0,
				"channelPowerSwitch": false,
				"voltage": 0,
				"currentLimit": 0,
				"backlightLevel": 0,
				"fanSpeedLevel": 0,
				"logIntervalLevel": 0,
				"apName": "",
				"apPassword": "",
			},
		}
		*/
		switch (static_cast<SocketCommand>((int) receivedJsonDoc["command"])) {
			case SOCKET_COMMAND_GET_CURRENT_POWER: {
				actualJsonSize = JSON_ARRAY_SIZE(MAX_CHANNEL_NUM) + JSON_OBJECT_SIZE(2) + MAX_CHANNEL_NUM * JSON_OBJECT_SIZE(5) + 230;
				// At least 1024 is recommended for heap allocation
				DynamicJsonDocument jsonDoc(actualJsonSize < 1024 ? 1024 : actualJsonSize);

				/*
				{
					"type": "power",
					"data": [
						{
							"channel": 0,
							"active": true,
							"voltage": "12000",
							"ampere": "3000",
							"watt": "36000",
							"setVoltage": "12000",
							"setCurrentLimit": "30",
						},
						{
							"channel": 1,
							"active": false,
							"voltage": "5000",
							"ampere": "0",
							"watt": "0",
							"setVoltage": "5000",
							"setCurrentLimit": "30",
						}
					]
				}
				*/
				jsonDoc["type"] = "power";
				JsonArray jsonArr = jsonDoc.createNestedArray("data");
				for (int i = 0; i < MAX_CHANNEL_NUM; i++) {
					JsonObject jsonObj = jsonArr.createNestedObject();
					jsonObj["channel"] = i;
					jsonObj["active"] = currentPower[i].isActive;
					jsonObj["voltage"] = currentPower[i].voltage;
					jsonObj["ampere"] = currentPower[i].ampere;
					jsonObj["watt"] = currentPower[i].watt;
					jsonObj["setVoltage"] = currentPower[i].setVoltage;
					jsonObj["setCurrentLimit"] = currentPower[i].setCurrentLimit;
				}

				serializeJson(jsonDoc, resultJson);

				// Delete the JSON document
				jsonDoc.clear();
				break;
			}
			case SOCKET_COMMAND_GET_SETTINGS: {

				break;
			}
			case SOCKET_COMMAND_SET_SETTINGS: {
				JsonObject dataObject = receivedJsonDoc["data"];

				if (dataObject["channel"] != nullptr) {
					int channel = dataObject["channel"];

					if (dataObject["channelPowerSwitch"] != nullptr) {
						if (dataObject["channelPowerSwitch"] == true) {
							Screen::remoteSwitchChannelOnoff(channel);
						} else {
							// Do nothing
						}
					}

					if (dataObject["voltage"] != nullptr) {
						Screen::remoteSetVoltage(channel, dataObject["voltage"]);
					}

					if (dataObject["currentLimit"] != nullptr) {
						Screen::remoteSetCurrentLimit(channel, dataObject["currentLimit"]);
					}
				}

				if (dataObject["backlightLevel"] != nullptr) {
					// TODO: implement this condition
					// Serial.println("WiFi: DEBUG: new backlightLevel: " + String(dataObject["backlightLevel"]));
				}

				if (dataObject["fanSpeedLevel"] != nullptr) {
					// TODO: implement this condition
					// Serial.println("WiFi: DEBUG: new fanSpeedLevel: " + String(dataObject["fanSpeedLevel"]));
				}

				if (dataObject["logIntervalLevel"] != nullptr) {
					// TODO: implement this condition
					// Serial.println("WiFi: DEBUG: new logIntervalLevel: " + String(dataObject["logIntervalLevel"]));
				}

				if (dataObject["apName"] != nullptr && dataObject["apPassword"] != nullptr) {
					// TODO: implement this condition
					// Serial.println("WiFi: DEBUG: new AP auth: " + String(dataObject["apName"]) + "/" + String(dataObject["apPassword"]));
				}

				// At least 1024 is recommended for heap allocation
				DynamicJsonDocument jsonDoc(1024);

				/*
				{
					"type": "log",
					"data": {
						"message": "New settings saved",
						"level": 0,
					}
				}
				*/
				// TODO: Needs to check it is saved actually
				jsonDoc["type"] = "log";
				JsonArray jsonArr = jsonDoc.createNestedArray("data");
				JsonObject jsonObj = jsonArr.createNestedObject();
				jsonObj["message"] = "New settings saved";
				jsonObj["level"] = 0;

				serializeJson(jsonDoc, resultJson);

				// Delete the JSON document
				jsonDoc.clear();
				break;
			}
			case SOCKET_COMMAND_NONE:
				// Do nothing, just explicit the case
			default:
				// Return the error code
				actualJsonSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(1) + 70;

				// At least 1024 is recommended for heap allocation
				DynamicJsonDocument jsonDoc(1024);

				/*
				{
					"type": "log",
					"data": {
						"message": "Unknown command",
						"level": 2
					}
				}
				*/
				jsonDoc["type"] = "log";
				JsonArray jsonArr = jsonDoc.createNestedArray("data");
				JsonObject jsonObj = jsonArr.createNestedObject();
				jsonObj["message"] = "Unknown command - Error occurs";
				jsonObj["level"] = 2;

				serializeJson(jsonDoc, resultJson);

				// Delete the JSON document
				jsonDoc.clear();
				break;
		}

		// Send to client the JSON data as string
		client->text(resultJson.c_str());
	}
}
