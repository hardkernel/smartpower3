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

#ifndef BLEMANAGER_H
#define BLEMANAGER_H

#include <BLEServer.h>
#include "FS.h"

// Every SmartPower 3 has to have these values unless somewhat special reason
// - Refered from our ethernet MAC address
#define HK_SP3_UUID_PREFIX "001E0642"
// - Randomly generated. These are for each characteristic
#define HK_SP3_UUID_RX_FIX "F394"
#define HK_SP3_UUID_TX_FIX "E2B3"
// - The delimiter to filter the values in one advertised data
#define HK_SP3_ADV_DELIMITER ","
// - The keyword to filter among the data
#define HK_SP3_ADV_FILT_CHANNEL "ch"
#define HK_SP3_ADV_FILT_VOLTAGE "v"
#define HK_SP3_ADV_FILT_AMPERE "a"
#define HK_SP3_ADV_FILT_WATT "w"

// Tag for logging
#define BLE_LOGTAG "BLE"

// Preference
#define BLE_PREF_NAME_LENGTH 17

// Full UUID length is 36, but we will include the null terminator
// to use the string related functions
#define BLE_UUID_LENGTH 37

// So that the board specific UUID will be
// 24 bytes for characters + 4 bytes for hypens
#define BOARD_UNIQUE_UUID_LENGTH 29

// Key names of the setting.txt file
#define SETTING_KEY_BLE_UUID "ble_board_unique_uuid"

// TODO: These should not be here
#define PROJECT_NAME "SmartPower 3"
#define MAX_CHANNEL_NUM 2
#define CHANNEL_0 0
#define CHANNEL_1 1

enum BleServiceState {
	BLE_SERVICE_ON = 0,
	BLE_SERVICE_OFF
};

enum BleConnectionState {
	BLE_DEVICE_CONNECTING = 0,
	BLE_DEVICE_CONNECTED,
	BLE_DEVICE_DISCONNECTING,
	BLE_DEVICE_DISCONNECTED
};

enum BleUuid {
	BLE_UUID_BOARD = 0,
	BLE_UUID_RX_CHAR,
	BLE_UUID_TX_CHAR
};

struct BlePreference {
	char name[BLE_PREF_NAME_LENGTH];
};

struct BleDevicePowerInfo {
	unsigned char channel;
	uint16_t voltage;
	uint16_t ampere;
	uint16_t watt;
};

class BleManager {
public:
	static BleManager &instance() {
		static BleManager *instance = new BleManager();
		return *instance;
	}

	BleManager();

	void init();
	void start();
	void stop();
	void notify(uint16_t channel);

	bool getBleServiceState();
	int getConnectionState();
	void getUuid(char *uuid, BleUuid what);

	void setPreference(BlePreference pref);
	void setCurrentPowerInfo(BleDevicePowerInfo powerInfo);
	void setConnectionState(BleConnectionState state);

	void serialLogLine(std::string message);

private:
	BLEServer *bleServer;
	BLEService *bleService;
	BLECharacteristic *bleRxCharacteristic;
	BLECharacteristic *bleTxCharacteristic;

	BlePreference blePreference;
	BleDevicePowerInfo blePowerInfo[MAX_CHANNEL_NUM];
	char boardUniqueUuid[BOARD_UNIQUE_UUID_LENGTH];
	bool deviceConnected;
	bool bleServiceState;
	fs::FS *fs;

	void restoreUuidFromFile();
	void saveUuidToFile();
	void generateUuid(char *uuid);
};

#endif
