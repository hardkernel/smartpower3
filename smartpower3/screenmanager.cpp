#include "screenmanager.h"
#include "helpers.h"


ScreenManager::ScreenManager()
{
	tft.init();
	tft.invertDisplay(true);
	tft.setRotation(3);
	tft.fillScreen(BG_COLOR);
	tft.setSwapBytes(true);
}

void ScreenManager::begin(Settings *settings, WiFiManager *wifi_manager, TwoWire *theWire)
{
	initLED();

	this->settings = settings;

	header = new Header(&tft);
	this->wifi_manager = wifi_manager;

	voltage_screen = new VoltageScreen(&tft, header, settings, theWire, wifi_manager, onoff);  // included object draw on screen :(
	setting_screen = new SettingScreen(&tft, header, settings, wifi_manager, onoff);
	network_screen = new NetworkScreen(&tft, header, settings, wifi_manager, onoff);
	logo_screen = new LogoScreen(&tft, header, settings, wifi_manager, onoff);
	logo_screen->init();

	screens[LOGO_SCREEN] = logo_screen;
	screens[VOLTAGE_SCREEN] = voltage_screen;
	screens[SETTING_SCREEN] = setting_screen;
	screens[NETWORK_SCREEN] = network_screen;

	fsInit();
	delay(2000); // approximate logo display time
	header->init();
}

uint8_t* ScreenManager::getOnOff()
{
	return onoff;
}

void ScreenManager::run()
{
	this->checkOnOff();
	if (!shutdown) {
		this->draw();

		if (!header->getLowInput()) {
			for (int i = 0; i < 2; i++) {
				if (!getVoltageScreen()->getEnabledSTPD01(i))
					continue;
				getVoltageScreen()->getChannel(i)->isr(onoff[i]);
			}
		}

#ifdef SCREEN_CAPTURE
		if (Serial.available()) {
			if (Serial.readString() == "S") {
				uint8_t color[2*8];
				for (uint32_t y = 0; y < 320; y++) {
					for (uint32_t x = 0; x < 480; x+=8) {
						tft.readRect(x, y, 8, 1, (uint16_t *)color);
						Serial.write(color, 2*8);
					}
				}
				Serial.flush();
			}
		}
#endif
	}
}

void ScreenManager::pushInputPower(uint16_t volt, uint16_t ampere, uint16_t watt)
{
	if ((volt < 6000 || volt >= 26000) && !low_input) {
		low_input = true;
		header->setLowInput(true);
		state_power = 1;
	} else if ((volt >= 6000 && volt <= 26000) && header->getLowInput()) {
		low_input = false;
		header->setLowInput(false);
		state_power = 3;
	}
	header->pushPower(volt, ampere, watt);
}

void ScreenManager::checkOnOff()
{
	if (state_power == 1) {
		state_power = 2;
		voltage_screen->getChannel(0)->off();
		voltage_screen->getChannel(1)->off();
		disablePower();
		onoff[0] = 2;
		onoff[1] = 2;
	} else if (state_power == 3) {
#ifdef USE_SINGLE_IRQ_STPD01
		for (int i = 0; i < 2; i++) {
			voltage_screen->getChannel(i)->enable();
			voltage_screen->getChannel(i)->off();
		}
#endif
		state_power = 4;
	} else if (state_power == 4) {
		state_power = 5;
	} else if (state_power == 5) {
		state_power = 6;
	} else if (state_power == 6) {
		for (int i = 0; i < 2; i++) {
			if (onoff[i] == 3) {
				if (!voltage_screen->getEnabledSTPD01(i)) {
					voltage_screen->setEnabledSTPD01(i, true);
					voltage_screen->getChannel(i)->enable();
				}
				voltage_screen->getChannel(i)->on();
				onoff[i] = 1;
			} else if (onoff[i] == 2) {
				onoff[i] = 0;
				voltage_screen->getChannel(i)->off();
				voltage_screen->getChannel(i)->drawChannel(true);
			}
		}
	}

	if (btn_pressed[4]) {
		btn_pressed[2] = false;
		btn_pressed[4] = false;
		if (!shutdown) {
			shutdown = true;
			for (int i = 0; i < LAST_SCREEN_COUNT; i++) {
				screens[i]->onShutdown();
			}
			tft.fillRect(0, 0, 480, 320, BG_COLOR);
			writeSysLED(0);
		} else {
			shutdown = false;
			tft.fillRect(0, 0, 480, 320, BG_COLOR);

			setting_screen->setLogInterval();
			setting_screen->setBacklightLevelPreset();

			header->init();

			for (int i = 0; i < LAST_SCREEN_COUNT; i++) {
				screens[i]->onWakeup();
			}

			updated_wifi_info = true;
			updated_wifi_icon = true;
			wifi_manager->update_wifi_info = true;
			wifi_manager->update_udp_info = true;
		}
	}
}

void ScreenManager::setTime(uint32_t milisec)
{
	if (flag_long_press) {
		if (digitalRead(36) == 0) {
			if (count_long_press) {
				count_long_press = false;
				btn_pressed[4] = true;
				flag_long_press = 0;
			}
		} else {
			if (flag_long_press != 3) {
				btn_pressed[2] = true;
			}
			flag_long_press = 0;
		}
		if (flag_long_press == 1) {
			flag_long_press = 2;
		}
	}
	this->cur_time = milisec;
	this->getActiveScreen()->setTime(milisec);
}

void ScreenManager::disablePower()
{
	voltage_screen->getChannel(0)->disabled();
	voltage_screen->getChannel(1)->disabled();
	voltage_screen->setEnabledSTPD01(0, false);
	voltage_screen->setEnabledSTPD01(1, false);
}

void ScreenManager::draw()
{
	if (show_next_screen) {
		show_next_screen = false;
		this->getActiveScreen()->onLeave();
		this->setNextScreen();
		this->getActiveScreen()->onEnter();
	}
	show_next_screen = this->getActiveScreen()->draw();
}

void ScreenManager::fsInit(void)
{
	uint16_t volt_set0 = 5000;  // 5.0V in millivolts
	uint16_t volt_set1 = 5000;  // 5.0V in millivolts
	uint16_t current_limit0 = 3000;  // 3.0A in milliamps
	uint16_t current_limit1 = 3000;  // 3.0A in milliamps
	uint8_t backlight_level_preset = 3;
	uint8_t log_interval = 0;
	uint32_t serial_baud = 115200;
	uint16_t port_udp = 0;
	IPAddress address;
	address.fromString("0.0.0.0");

	//NOTE: When all relevant values have defaults in Settings, this block could be removed
	if (settings->isFirstBoot(true)) {
		Serial.println(F("First boot!!!"));
		settings->setBacklightLevelIndex(backlight_level_preset);
		settings->setSerialBaudRate(serial_baud);
		settings->setLogInterval(log_interval);
		settings->setFirstBoot(false);
		settings->setChannel0Voltage(volt_set0);
		settings->setChannel1Voltage(volt_set1);
		settings->setChannel0CurrentLimit(current_limit0);
		settings->setChannel1CurrentLimit(current_limit1);
		settings->setWifiIpv4UdpLoggingServerIpAddress(address, true);
		settings->setWifiIpv4UdpLoggingServerPort(port_udp);
	}
	volt_set0 = settings->getChannel0Voltage();
	volt_set1 = settings->getChannel1Voltage();
	current_limit0 = settings->getChannel0CurrentLimit();
	current_limit1 = settings->getChannel1CurrentLimit();
	backlight_level_preset = settings->getBacklightLevelIndex(true);
	serial_baud = settings->getSerialBaudRate(true);
	log_interval = settings->getLogInterval(true);

	voltage_screen->getChannel(0)->setVolt(volt_set0, 1);
	voltage_screen->getChannel(0)->setCurrentLimit(current_limit0, 1);
	voltage_screen->getChannel(1)->setVolt(volt_set1, 1);
	voltage_screen->getChannel(1)->setCurrentLimit(current_limit1, 1);
	setting_screen->setBacklightLevelPreset(backlight_level_preset, true);
	setting_screen->setSerialBaud(serial_baud);
	setting_screen->setLogInterval(log_interval);

	wifi_manager->ipaddr_udp = settings->getWifiIpv4UdpLoggingServerIpAddress(true);
	wifi_manager->port_udp = settings->getWifiIpv4UdpLoggingServerPort(true);
}

void ScreenManager::debug()
{
	header->setDebug();
}

bool ScreenManager::getShutdown(void)
{
	return shutdown;
}

void ScreenManager::dimmingLED(uint8_t led)
{
	for (int i = 0; i < 150; i++) {
		ledcWrite(led, i);
		delay(10);
	}
	for (int i = 150; i > 0; i--) {
		ledcWrite(led, i);
		delay(10);
	}
}

void ScreenManager::writeSysLED(uint8_t val)
{
	ledcWrite(0, val);
}

void ScreenManager::writePowerLED(uint8_t val)
{
	ledcWrite(1, val);
}

void ScreenManager::initLED()
{
	ledcSetup(0, FREQ, RESOLUTION);
	ledcSetup(1, FREQ, RESOLUTION);
	ledcSetup(2, FREQ, RESOLUTION);  // backligh LED initialization
	ledcAttachPin(LED2, 0);
	ledcAttachPin(LED1, 1);
	ledcAttachPin(BL_LCD, 2);

	ledcWrite(0, 50);
	ledcWrite(1, 50);
}

VoltageScreen* ScreenManager::getVoltageScreen(void)
{
	return this->voltage_screen;
}

SettingScreen* ScreenManager::getSettingScreen(void)
{
	return this->setting_screen;
}

NetworkScreen* ScreenManager::getNetworkScreen(void)
{
	return this->network_screen;
}

Screen* ScreenManager::getActiveScreen(void)
{
	return screens[screen];
}

void ScreenManager::setNextScreen()
{
	  // next screen or second screen (first one is logo)
	screen = max(static_cast<screen_t>((screen + 1) % LAST_SCREEN_COUNT), static_cast<screen_t>(1));
}

void ScreenManager::getBtnPress(uint8_t idx, uint32_t cur_time, bool long_pressed)
{
	switch (idx) {
	case 0:  // Channel0 ON/OFF
	case 1:  // Channel1 ON/OFF
		if (shutdown || screen != VOLTAGE_SCREEN || long_pressed)
			break;
		if (onoff[idx] == 1)
			onoff[idx] = 2;
		else if (onoff[idx] == 0)
			onoff[idx] = 3;
		btn_pressed[idx] = true;
		break;
	case 2:  // MENU/CANCEL
		dial_time = cur_time;
		flag_long_press = 1;
		count_long_press = long_pressed;
		break;
	case 3:  // Set value - dial button
		if (shutdown)
			break;
		dial_time = cur_time;
		btn_pressed[idx] = true;
		break;
	default:
		break;
	}
	this->getActiveScreen()->getBtnPress(idx, cur_time, long_pressed);
}

void ScreenManager::pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch)
{
	if (onoff[ch] == 1)
		this->voltage_screen->getChannel(ch)->pushPower(volt, ampere, watt);
}

void ScreenManager::setWiFiIconState()
{
	if (WiFi.status() == WL_CONNECTED) {
		header->onWiFi();
	} else if (wifi_manager->canConnect()) {
		header->possibleWiFi();
	} else {
		header->offWiFi();
	}
}

Settings* ScreenManager::getSettings()
{
	return this->settings;
}

