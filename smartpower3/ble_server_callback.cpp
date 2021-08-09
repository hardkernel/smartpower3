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

#include <BLEServer.h>

#include "ble_manager.h"
#include "ble_server_callback.h"

void BleServerCallback::onConnect(BLEServer *bleServer) {
	ble.setConnectionState(BLE_DEVICE_CONNECTED);
}

void BleServerCallback::onDisconnect(BLEServer *bleServer) {
	ble.setConnectionState(BLE_DEVICE_DISCONNECTED);
}
