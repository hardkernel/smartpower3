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

#include <random>
#include <sstream>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "FS.h"
#include "SPIFFS.h"

#include "ble_manager.h"
#include "ble_char_callback.h"
#include "ble_server_callback.h"

BleManager::BleManager() {
	// Set the properties using the default value
	deviceConnected = BLE_DEVICE_DISCONNECTED;
	bleServiceState = BLE_SERVICE_OFF;
}

void BleManager::init() {
	uint8_t chIndex;

	serialLogLine("Initializing...");

	// TODO: Implement store/restore the board unique UUID to a setting.txt file
	//// SPIFFS is initialized in the screen.cpp file
	//fs = &SPIFFS;

	//// Check the stored UUID from the setting.txt file
	//// If there isn't, generate a UUID if there's no UUID stored
	//restoreUuidFromFile();
	//if (strlen(boardUniqueUuid) < BOARD_UNIQUE_UUID_LENGTH) {
	//	generateUuid(boardUniqueUuid);
	//	saveUuidToFile();
	//}
	generateUuid(boardUniqueUuid);
	serialLogLine("It will use the generated UUID: " + (std::string) boardUniqueUuid);

	// TODO: Set the name from the setting.txt file or append last 4 chars of UUID
	sprintf(blePreference.name, "%s", PROJECT_NAME);
	serialLogLine("Set the default name" + (std::string) PROJECT_NAME);

	// Initialize the blePowerInfo structures
	for (chIndex = 0; chIndex < MAX_CHANNEL_NUM; chIndex++) {
		blePowerInfo[chIndex].channel = chIndex;
	}

	serialLogLine("Initialized");

	// TODO: Do not turn on the BLE service by default
	start();
}

void BleManager::start() {
	char serviceUuid[BLE_UUID_LENGTH],
		txCharUuid[BLE_UUID_LENGTH],
		rxCharUuid[BLE_UUID_LENGTH];

	getUuid(serviceUuid, BLE_UUID_BOARD);
	getUuid(txCharUuid, BLE_UUID_TX_CHAR);
	getUuid(rxCharUuid, BLE_UUID_RX_CHAR);

	BLEDevice::init(blePreference.name);

	bleServer = BLEDevice::createServer();
	bleServer->setCallbacks(new BleServerCallback());

	serialLogLine("bleServer ready");

	bleService = bleServer->createService(serviceUuid);

	serialLogLine("bleService ready: " + (std::string) serviceUuid);

	bleTxCharacteristic = bleService->createCharacteristic(
		txCharUuid,
		BLECharacteristic::PROPERTY_NOTIFY
	);
	bleRxCharacteristic = bleService->createCharacteristic(
		rxCharUuid,
		BLECharacteristic::PROPERTY_WRITE
	);

	serialLogLine("bleTxCharacteristic ready: " + (std::string) txCharUuid);
	serialLogLine("bleRxCharacteristic ready: " + (std::string) rxCharUuid);

	bleTxCharacteristic->addDescriptor(new BLE2902());
	bleRxCharacteristic->setCallbacks(new BleCharCallback());

	bleService->start();
	bleServer->getAdvertising()->start();

	bleServiceState = BLE_SERVICE_ON;

	serialLogLine("bleService and bleServer started");
}

void BleManager::stop() {
	// TODO: Implement stop() method

	bleServiceState = BLE_SERVICE_OFF;
}

void BleManager::notify(uint16_t channel) {
	// To shorten the code, and to convert the type to a string
	std::string delimiter = std::string(HK_SP3_ADV_DELIMITER);

	// Max data length is 64
	std::string advData =
		HK_SP3_ADV_FILT_CHANNEL + delimiter +
		std::to_string(blePowerInfo[channel].channel) + // To see where the data is from
		HK_SP3_ADV_FILT_VOLTAGE + delimiter +
		std::to_string(blePowerInfo[channel].voltage) +
		HK_SP3_ADV_FILT_AMPERE + delimiter +
		std::to_string(blePowerInfo[channel].ampere) +
		HK_SP3_ADV_FILT_WATT + delimiter +
		std::to_string(blePowerInfo[channel].watt);

	bleTxCharacteristic->setValue(advData);
	bleTxCharacteristic->notify();
}

bool BleManager::getBleServiceState() {
	return bleServiceState;
}

int BleManager::getConnectionState() {
	return deviceConnected;
}

void BleManager::getUuid(char *uuid, BleUuid what) {
	char boardUniqueUuidTemp[BOARD_UNIQUE_UUID_LENGTH];

	// uuidPart1 is predefined to HK_SP3_UUID_PREFIX
	// uuidPart2 is predefined in some case and included in the boardUniqueUuid variable
	// Each part terminated with '\0' because it will be used as a string
	char uuidPart3[5], uuidPart4[5], uuidPart5[13];
	char *ch;
	uint8_t tokenIdx = 0;

	sprintf(boardUniqueUuidTemp, boardUniqueUuid, BOARD_UNIQUE_UUID_LENGTH);
	ch = strtok(boardUniqueUuidTemp, "-");
	ch = strtok(NULL, "-"); // uuidPart2
	while (ch != NULL) {
		serialLogLine(ch);

		switch (tokenIdx) {
			case 0:
				strncpy(uuidPart3, ch, 5);
			break;
			case 1:
				strncpy(uuidPart4, ch, 5);
			break;
			case 2:
				strncpy(uuidPart5, ch, 13);
			break;
		}

		ch = strtok(NULL, "-");
		tokenIdx++;
	}

	switch (what) {
		case BLE_UUID_BOARD:
			sprintf(uuid, "%s%s",
				HK_SP3_UUID_PREFIX,
				strlen(boardUniqueUuid) > 0 ? boardUniqueUuid : ""
			);
		break;
		case BLE_UUID_RX_CHAR:
			sprintf(uuid, "%s-%s-%s-%s-%s",
				HK_SP3_UUID_PREFIX,
				HK_SP3_UUID_RX_FIX,
				uuidPart3,
				uuidPart4,
				uuidPart5
			);
		break;
		case BLE_UUID_TX_CHAR:
			sprintf(uuid, "%s-%s-%s-%s-%s",
				HK_SP3_UUID_PREFIX,
				HK_SP3_UUID_TX_FIX,
				uuidPart3,
				uuidPart4,
				uuidPart5
			);
		break;
	}
}

void BleManager::setPreference(BlePreference pref) {
	// TODO: Implement setPreference() method
}

void BleManager::setCurrentPowerInfo(BleDevicePowerInfo powerInfo) {
	blePowerInfo[powerInfo.channel].voltage = powerInfo.voltage;
	blePowerInfo[powerInfo.channel].ampere = powerInfo.ampere;
	blePowerInfo[powerInfo.channel].watt = powerInfo.watt;
}

void BleManager::setConnectionState(BleConnectionState state) {
	// TODO: Implement connecting, disconnecting states

	deviceConnected = state == BLE_DEVICE_CONNECTED ? true : false;
}

void BleManager::serialLogLine(std::string message) {
	Serial.print(BLE_LOGTAG);
	Serial.print(": ");
	Serial.println(message.c_str());
}

void BleManager::restoreUuidFromFile() {
	File file = fs->open("/setting.txt", "r");

	serialLogLine("Reading the current settings...");

	file.seek(0, SeekSet);
	file.findUntil(SETTING_KEY_BLE_UUID, "\n\r");
	file.seek(1, SeekCur);
	strncpy(boardUniqueUuid, file.readStringUntil('\n').c_str(), BOARD_UNIQUE_UUID_LENGTH);
	file.close();

	serialLogLine("UUID restored: " + (std::string) boardUniqueUuid);
}

void BleManager::saveUuidToFile() {
	File file = fs->open("/setting.txt", "r+");

	serialLogLine("Writing the new UUID a setting file...");

	file.seek(0, SeekSet);
	file.findUntil(SETTING_KEY_BLE_UUID, "\n\r");
	file.seek(1, SeekCur);
	file.print(boardUniqueUuid);
	file.flush();
	file.close();

	serialLogLine("UUID wrote");
}

void BleManager::generateUuid(char *uuid) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 15);
	std::stringstream ss;

	int i;

	ss << std::hex;
	ss << "-";
	for (i = 0; i < 4; i++) {
		ss << dis(gen);
	}
	ss << "-";
	for (i = 0; i < 4; i++) {
		ss << dis(gen);
	}
	ss << "-";
	for (i = 0; i < 4; i++) {
		ss << dis(gen);
	}
	ss << "-";
	for (i = 0; i < 12; i++) {
		ss << dis(gen);
	};

	// Will contain "-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
	strncpy(uuid, ss.str().c_str(), BOARD_UNIQUE_UUID_LENGTH);

	serialLogLine("UUID generated: " + (std::string) uuid);
	serialLogLine("- The first part of the UUID is the predefined: " + (std::string) HK_SP3_UUID_PREFIX);
}
