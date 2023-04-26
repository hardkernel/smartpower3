#include "voltagescreen.h"
#include "helpers.h"


VoltageScreen::VoltageScreen()
{
}

VoltageScreen::~VoltageScreen()
{
}

VoltageScreen::VoltageScreen(TFT_eSPI *tft, Header *header, Settings *settings, TwoWire *twoWire, WiFiManager *wifi_manager, uint8_t onoff[]) : Screen(tft, header, settings, wifi_manager, onoff)
{
	this->_wire = twoWire;
	channel[0] = new Channel(tft, _wire, 0, 50, 0);
	channel[1] = new Channel(tft, _wire, 246, 50, 1);
}

void VoltageScreen::setIntFlag(uint8_t ch)
{
	channel[ch]->setIntFlag();
}

void VoltageScreen::init(void)
{
	tft->fillRect(0, 52, 480, 285, BG_COLOR);
	for (int i = 0; i < 2; i++) {
		tft->drawLine(0, 50 + i, 480, 50 + i, TFT_DARKGREY);
		tft->drawLine(0, 274 + i, 480, 274 + i, TFT_DARKGREY);
	}
	for (int i = 0; i < 2; i++)
		tft->drawLine(236 + i, 50, 236 + i, 320, TFT_DARKGREY);

	channel[0]->initScreen(onoff[0]);
	channel[1]->initScreen(onoff[1]);
	selected = dial_cnt = dial_cnt_old = STATE_NONE;

	is_initialized = true;
}

void VoltageScreen::pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch)
{
	if (onoff[ch] == 1)
		channel[ch]->pushPower(volt, ampere, watt);
}

void VoltageScreen::pushInputPower(uint16_t volt, uint16_t ampere, uint16_t watt)
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

void VoltageScreen::disablePower()
{
	channel[0]->disabled();
	channel[1]->disabled();
	enabled_stpd01[0] = false;
	enabled_stpd01[1] = false;
}

void VoltageScreen::deSelect()
{
	channel[0]->deSelect(VOLT);
	channel[0]->deSelect(CURRENT);
	channel[1]->deSelect(VOLT);
	channel[1]->deSelect(CURRENT);
	header->deSelect(LOGGING);
	header->deSelect(WIFI);
}

void VoltageScreen::select()
{
	if (dial_cnt == dial_cnt_old) {
		return;
	}
	dial_cnt_old = dial_cnt;
	clampVariableToCircularRange(1, 6, dial_direct, &dial_cnt);  // 6 is 1 based count of screen_state_base elements

	deSelect();
	selected = dial_cnt;
	switch (dial_cnt) {
		case STATE_VOLT0:
			channel[0]->select(VOLT);
			break;
		case STATE_CURRENT0:
			channel[0]->select(CURRENT);
			break;
		case STATE_CURRENT1:
			channel[1]->select(CURRENT);
			break;
		case STATE_VOLT1:
			channel[1]->select(VOLT);
			break;
		case STATE_WIFI:
			header->select(WIFI);
			break;
		case STATE_LOGGING:
			header->select(LOGGING);
			break;
		default:
			break;
	}
}

void VoltageScreen::drawBase()
{
	if (dial_cnt != dial_cnt_old) {
		clearBtnEvent();
		mode = VOLTAGE_BASE_MOVE;
		show_next_screen = false;
	}
	if (btn_pressed[2] == true) {
		btn_pressed[2] = false;

		show_next_screen = true;

		updated_wifi_info = true;
		updated_wifi_icon = true;
		wifi_manager->update_wifi_info = true;
		wifi_manager->update_udp_info = true;
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
	}

	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
#ifdef DEBUG_STPD01
		channel[0]->clearDebug();
		channel[1]->clearDebug();
#endif
	}
}

void VoltageScreen::drawBaseMove()
{
	select();
	if ((cur_time - dial_time) > 5000) {
		mode = VOLTAGE_BASE;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		mode = VOLTAGE_BASE;
		btn_pressed[2] = false;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (selected == STATE_VOLT0) {
			mode = VOLTAGE_BASE_EDIT;
			channel[0]->setCompColor(VOLT);
			volt_set = channel[0]->getVolt();
		} else if (selected == STATE_CURRENT0) {
			mode = VOLTAGE_BASE_EDIT;
			channel[0]->setCompColor(CURRENT);
			current_limit = channel[0]->getCurrentLimit()/100;
		} else if (selected == STATE_VOLT1) {
			mode = VOLTAGE_BASE_EDIT;
			channel[1]->setCompColor(VOLT);
			volt_set = channel[1]->getVolt();
		} else if (selected == STATE_CURRENT1) {
			mode = VOLTAGE_BASE_EDIT;
			channel[1]->setCompColor(CURRENT);
			current_limit = channel[1]->getCurrentLimit()/100;
		} else if (selected == STATE_LOGGING) {
			// same as current state, but redraw will check selection timeout
			mode = VOLTAGE_BASE_MOVE;
			settings->switchLogging();
		} else if (selected == STATE_WIFI) {
			// same as current state, but redraw will check selection timeout
			mode = VOLTAGE_BASE_MOVE;
			settings->switchWifi();
		}
		dial_state = dial_cnt;
		dial_cnt = dial_cnt_old;
		show_next_screen = false;
	}
}

void VoltageScreen::drawBaseEdit()
{
	if ((cur_time - dial_time) > 10000) {
		mode = VOLTAGE_BASE;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		mode = VOLTAGE_BASE_MOVE;
		dial_cnt = dial_cnt_old = dial_state;
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
		btn_pressed[2] = false;
		return;
	}
	if (btn_pressed[3] == true) {
		mode = VOLTAGE_BASE_MOVE;
		btn_pressed[3] = false;
		changeVolt(mode);
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
		dial_cnt = dial_state;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		dial_cnt_old = dial_cnt;
		changeVolt(mode);
		channel[0]->pushPowerEdit();
		channel[1]->pushPowerEdit();
	}
}

bool VoltageScreen::draw()
{
	show_next_screen = false;

	if (!this->isInitialized()) {
		this->init();
	}

	switch (mode) {
	case VOLTAGE_BASE:
		drawBase();
		break;
	case VOLTAGE_BASE_MOVE:
		drawBaseMove();
		break;
	case VOLTAGE_BASE_EDIT:
		drawBaseEdit();
		break;
	}
	if ((cur_time - fnd_time) > 300) {
		fnd_time = cur_time;
		if (onoff[0])
			channel[0]->drawChannel();
		if (onoff[1])
			channel[1]->drawChannel();
		checkSTPD01InterruptStat();
		header->draw();
	}
	channel[0]->drawVoltSet();
	channel[1]->drawVoltSet();
	return show_next_screen;
}

void VoltageScreen::changeVolt(voltage_screen_mode_t mode)
{
	if (selected == STATE_VOLT0) {
		clampVoltageDialCountToRange(volt_set, &dial_cnt);
		if (mode == VOLTAGE_BASE_MOVE) {
			channel[0]->setVolt(dial_cnt);  // this by default sets incremental difference to currently set value
			if (dial_cnt != 0) {
				settings->setChannel0Voltage(channel[0]->getVolt());
			}
		} else {
			channel[0]->editVolt(dial_cnt);
		}
	} else if (selected == STATE_CURRENT0) {
		clampVariableToRange(-(current_limit - 5), (30 - current_limit), &dial_cnt);
		if (mode == VOLTAGE_BASE_MOVE) {
			channel[0]->setCurrentLimit(dial_cnt);  // this by default sets incremental difference to currently set value
			if (dial_cnt != 0) {
				settings->setChannel0CurrentLimit(channel[0]->getCurrentLimit());
			}
		} else {
			channel[0]->editCurrentLimit(dial_cnt);
		}

	} else if (selected == STATE_VOLT1) {
		clampVoltageDialCountToRange(volt_set, &dial_cnt);
		if (mode == VOLTAGE_BASE_MOVE) {
			channel[1]->setVolt(dial_cnt);  // this by default sets incremental difference to currently set value
			if (dial_cnt != 0) {
				settings->setChannel1Voltage(channel[1]->getVolt());
			}
		} else {
			channel[1]->editVolt(dial_cnt);
		}
	} else if (selected == STATE_CURRENT1) {
		clampVariableToRange(-(current_limit - 5), (30 - current_limit), &dial_cnt);
		if (mode == VOLTAGE_BASE_MOVE) {
			channel[1]->setCurrentLimit(dial_cnt);  // this by default sets incremental difference to currently set value
			if (dial_cnt != 0) {
				settings->setChannel1CurrentLimit(channel[1]->getCurrentLimit());
			}
		} else {
			channel[1]->editCurrentLimit(dial_cnt);
		}
	}
}

void VoltageScreen::checkSTPD01InterruptStat()
{
	for (int i = 0; i < 2; i++) {
		channel[i]->isAvailableSTPD01();
		int_stat[i] = channel[i]->checkInterruptStat(onoff[i]);
	}
}

uint8_t VoltageScreen::getIntStat(uint8_t channel)
{
	return int_stat[channel];
}

void VoltageScreen::countLowVoltage(uint8_t ch)
{
	channel[ch]->countLowVoltage();
}

void VoltageScreen::clearLowVoltage(uint8_t ch)
{
	channel[ch]->clearLowVoltage();
}

Channel* VoltageScreen::getChannel(uint8_t channel_number)
{
	return this->channel[channel_number];
}

bool VoltageScreen::getEnabledSTPD01(uint8_t which_stpd)
{
	return this->enabled_stpd01[which_stpd];
}

void VoltageScreen::setEnabledSTPD01(uint8_t which_stpd, bool enabled)
{
	this->enabled_stpd01[which_stpd] = enabled;
}

void VoltageScreen::onShutdown(void)
{
	this->shutdown = true;
	onoff[0] = 0;
	onoff[1] = 0;
	this->getChannel(0)->off();
	this->getChannel(1)->off();
	this->is_initialized = false;
}

void VoltageScreen::onWakeup(void)
{
	this->shutdown = false;
	selected = dial_cnt = dial_cnt_old = STATE_NONE;
}

void VoltageScreen::onLeave(void)
{
	this->is_initialized = false;
	channel[0]->setHide();
	channel[1]->setHide();
}

void VoltageScreen::onEnter(void)
{
	channel[0]->clearHide();
	channel[1]->clearHide();
}

screen_t VoltageScreen::getType(void)
{
	return VOLTAGE_SCREEN;
}
