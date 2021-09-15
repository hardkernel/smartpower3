#include "channel.h"
#include <ArduinoTrace.h>

Channel::Channel(TFT_eSPI *tft, TwoWire *theWire, uint16_t x, uint16_t y,
		uint8_t channel)
{
	this->tft = tft;
	this->x = x;
	this->y = y;
	this->channel = channel;

	volt = new FndWidget(tft);
	current = new FndWidget(tft);
	watt = new FndWidget(tft);
	icon_op = new FndWidget(tft);
	icon_sp = new FndWidget(tft);
	icon_cc = new FndWidget(tft);
	icon_tp = new FndWidget(tft);
	icon_tw = new FndWidget(tft);
	icon_ip = new FndWidget(tft);

	volt->fnd_init(NUM_OF_FND, 2, true, x, Y_VOLT + y+20, FG_COLOR, BG_COLOR);
	current->fnd_init(NUM_OF_FND, 2, true, x, Y_CURRENT + y+20, FG_COLOR, BG_COLOR);
	watt->fnd_init(NUM_OF_FND, 2, true, x, Y_WATT + y+20, FG_COLOR, BG_COLOR);
	icon_op->icon_init(0, x, y+2, FG_COLOR, BG_COLOR);
	icon_sp->icon_init(1, x + 34, y+2, FG_COLOR, BG_COLOR);
	icon_cc->icon_init(2, x + 68, y+2, FG_COLOR, BG_COLOR);
	icon_tp->icon_init(3, x + 102, y+2, FG_COLOR, BG_COLOR);
	icon_tw->icon_init(4, x + 136, y+2, FG_COLOR, BG_COLOR);
	icon_ip->icon_init(5, x + 170, y+2, FG_COLOR, BG_COLOR);

	stpd01 = new STPD01(0x5 + (channel*2), theWire);

	_volt = new Component(tft, 48, 22, 4);
	_current = new Component(tft, 48, 22, 4);
	stpd = new Component(tft, 38, 16, 2);
	pinMode(int_stpd01[channel], INPUT_PULLUP);
}

Channel::~Channel(void)
{
#if 0
	delete volt;
	delete ampere;
	delete watt;
	delete _volt;
	delete _ampere;
	volt = NULL;
	ampere = NULL;
	watt = NULL;
	_volt = NULL;
	_ampere = NULL;
#endif
}

bool Channel::isAvailableSTPD01()
{
	if (!stpd01->available()) {
		stpd->setTextColor(FG_DISABLED, BG_DISABLED);
		stpd->draw("STPD");
		return 0;
	}
	stpd->setTextColor(FG_ENABLED, BG_ENABLED);
	stpd->draw("STPD");
	return 1;
}

void Channel::initScreen()
{
	_volt->setCoordinate(x + 145, y + 25);
	_volt->init(TFT_YELLOW, TFT_BLACK, 1, TR_DATUM);
	_volt->pushValue(volt_set);
	_current->init(TFT_YELLOW, TFT_BLACK, 1, TR_DATUM);
	_current->setCoordinate(x + 145, y + 25 + Y_CURRENT);
	_current->pushValue(current_limit);

	stpd->init(FG_ENABLED, BG_ENABLED, 1, TR_DATUM);
	stpd->setCoordinate(x + 180, y + 180);
	stpd->draw(String("STPD"));

	tft->drawString("V", x + W_SEG-4, y + 58, 4);
	tft->drawString("A", x + W_SEG-4, y + 49 + H_SEG + OFFSET_SEG, 4);
	tft->drawString("W", x + W_SEG-4, y + 30 + H_SEG*2 + OFFSET_SEG*2, 4);
	drawChannel(true);
	drawVoltSet(true);
	drawInterrupt();
	drawPolarity();

}

void Channel::drawPolarity(void)
{
	// +
	tft->fillRoundRect(x, y + 240, 60, 40, 10, TFT_WHITE);
	for (int i=0; i < 10; i++)
		tft->drawLine(x + 15, y + 255 + i, x + 45, y + 255 + i, TFT_RED);
	for (int i=0; i < 10; i++)
		tft->drawLine(x + 25 + i, y + 245, x + 25 + i, y + 275, TFT_RED);

	// -
	tft->fillRoundRect(x + 135, y + 240, 60, 40, 10, TFT_WHITE);
	for (int i=0; i < 10; i++)
		tft->drawLine(x + 150, y + 255 + i, x + 180, y + 255 + i, TFT_BLACK);
}

void Channel::drawInterrupt(void)
{
	icon_op->icon_update(true, channel);
	icon_sp->icon_update(true, channel);
	icon_cc->icon_update(true, channel);
	icon_tp->icon_update(true, channel);
	icon_tw->icon_update(true, channel);
	icon_ip->icon_update(true, channel);
}

uint8_t Channel::getIntStatus(void)
{
	return stpd01->readIntStatus();
}

uint8_t Channel::getIntMask(void)
{
	return stpd01->readIntMask();
}

void Channel::enable(void)
{
	uint8_t cnt = 0;
	pinMode(en_stpd01[channel], OUTPUT);
	digitalWrite(en_stpd01[channel], HIGH);
	while (!stpd01->available()) {
		if (cnt++ > 10) {
			Serial.printf("channel : %d STPD01 is not available!\n\r", channel);
			break;
		}
		delay(100);
	}
}

void Channel::disabled(void)
{
	pinMode(en_stpd01[channel], OUTPUT);
	digitalWrite(en_stpd01[channel], LOW);

	volt->setTextColor(TFT_DARKGREY, TFT_BLACK);
	current->setTextColor(TFT_DARKGREY, TFT_BLACK);
	watt->setTextColor(TFT_DARKGREY, TFT_BLACK);
}

bool Channel::on(void)
{
	bool err;
	TRACE();
	if (stpd01->available()) {
		stpd01->setVoltage(volt_set);
		stpd01->setCurrentLimit(current_limit);
		stpd01->initInterrupt();
		err = stpd01->on();
	}
	volt->setTextColor(TFT_RED, TFT_BLACK);
	current->setTextColor(TFT_RED, TFT_BLACK);
	watt->setTextColor(TFT_RED, TFT_BLACK);
	return err;
}

bool Channel::off(void)
{
	bool err;
	TRACE();
	if (stpd01->available()) {
		err = stpd01->off();
	}
	volt->setTextColor(TFT_DARKGREY, TFT_BLACK);
	current->setTextColor(TFT_DARKGREY, TFT_BLACK);
	watt->setTextColor(TFT_DARKGREY, TFT_BLACK);
	this->volt->pushValue(0);
	this->current->pushValue(0);
	this->watt->pushValue(0);
	return err;
}

void Channel::setHide()
{
	hide = 1;
}

void Channel::clearHide()
{
	hide = 0;
}

void Channel::write(uint8_t addr, uint8_t reg)
{
	stpd01->write8(addr, reg);
}

uint16_t Channel::getVolt(void)
{
	return volt_set;
}

uint16_t Channel::getCurrentLimit(void)
{
	return current_limit/100;
}

void Channel::setVolt(float volt_set, uint8_t mode)
{
	if (mode == 0) {
		this->volt_set = this->volt_set + volt_set*100;
		stpd01->setVoltage(min((uint16_t)20000, this->volt_set));
	} else if (mode == 1) {
		this->volt_set = volt_set;
		this->_volt_set = volt_set;
		this->_volt->pushValue(min((uint16_t)20000, _volt_set));
	}
}

void Channel::editVolt(float volt_set)
{
	this->_volt_set = this->volt_set + volt_set*100;
}

void Channel::setCurrentLimit(float val, uint8_t mode)
{
	if (mode == 0) {
		current_limit += val*100;
		current_limit = min((uint16_t)3000, current_limit);
		stpd01->setCurrentLimit(current_limit);
	} else if (mode == 1) {
		current_limit = val;
		_current_limit = current_limit;
		_current_limit = min((uint16_t)3000, _current_limit);
		_current_limit = max((uint16_t)500, _current_limit);
		this->_current->pushValue(_current_limit);
	}
}

void Channel::editCurrentLimit(float val)
{
	_current_limit = current_limit + val*100;
	_current_limit = min((uint16_t)3000, _current_limit);
	_current_limit = max((uint16_t)500, _current_limit);
}

void Channel::drawVoltSet(bool forced)
{
	if (!hide) {
		_volt->draw(forced);
		_current->draw(forced);
	}
}

void Channel::drawChannel(bool forced)
{
	if (!hide) {
		volt->fnd_update(forced);
		current->fnd_update(forced);
		watt->fnd_update(forced);
	}
}

void Channel::clearCompColor(void)
{
	_volt->setTextColor(TFT_YELLOW, TFT_BLACK);
	_current->setTextColor(TFT_YELLOW, TFT_BLACK);
	_volt_set = volt_set;
	_current_limit = current_limit;
	_volt->pushValue(volt_set);
	_current->pushValue(current_limit);
	_volt->draw(true);
	_current->draw(true);
}

void Channel::setCompColor(comp_t comp)
{
	if (comp == VOLT) {
		_volt->setTextColor(TFT_DARKGREY, TFT_WHITE);
		_volt->draw(true);
	} else if (comp == CURRENT) {
		_current->setTextColor(TFT_RED, TFT_GREEN);
		_current->draw(true);
	}
}

uint16_t Channel::getValueVolt()
{
	return this->volt->getValue();
}

uint16_t Channel::getValueCurrent()
{
	return this->current->getValue();
}

uint16_t Channel::getValueWatt()
{
	return this->watt->getValue();
}

void Channel::pushPower(uint16_t volt, uint16_t current, uint16_t watt)
{
	moving_avg[(cnt_mavg++)%5] = current;
	if (cnt_mavg > 4) {
		current = 0;
		for (int i = 0; i < 5; i++) {
			current += moving_avg[i];
		}
		current = current/5;
	} else {
		this->volt->pushValue(volt);
		return;
	}

	if (current < 5) {
		current = 0;
		watt = 0;
	}
	this->volt->pushValue(volt);
	this->current->pushValue(current);
	this->watt->pushValue(watt);
}

void Channel::pushPowerEdit()
{
	this->_volt->pushValue(_volt_set);
	this->_current->pushValue(_current_limit);
}

void Channel::activate(comp_t comp)
{
	switch (comp) {
		case VOLT:
			_volt->activate();
			break;
		case CURRENT:
			_current->activate();
			break;
		case WATT:
			break;
	}
}

void Channel::deActivate(comp_t comp)
{
	switch (comp) {
		case VOLT:
			_volt->deActivate();
			break;
		case CURRENT:
			_current->deActivate();
			break;
		case WATT:
			break;
	}
}

void Channel::setIntFlag(void)
{
	flag_int = true;
}

void Channel::isr(void)
{
	if (flag_int || !digitalRead(int_stpd01[channel])) {
		DUMP(channel);
		DUMP(flag_int);
		if (stpd01->available()) {
			latch = checkInterruptLatch();
			DUMP(latch);
			if (latch & INT_OTP) {
				off();
				Serial.printf("ch%d, flag_int %d, latch : %x OTP\n\r", channel, flag_int, latch);
			} else if (latch & (INT_OVP|INT_SCP|INT_OTW|INT_IPCP)) {
				on();
				Serial.printf("ch%d, flag_int %d, latch : %x Retry set voltage for interrupts\n\r", channel, flag_int, latch);
			}
			flag_int = false;
		}
	}
}

uint8_t Channel::checkInterruptLatch(void)
{
	uint8_t reg_latch;

	reg_latch = stpd01->readIntLatch();
	if (reg_latch & INT_OVP) {
		icon_op->setIconColor(TFT_RED, BG_ENABLED_INT);
	} else {
		icon_op->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	}
	if (reg_latch & INT_SCP) {
		icon_sp->setIconColor(TFT_RED, BG_ENABLED_INT);
	} else {
		icon_sp->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	}
	if (reg_latch & INT_OTP) {
		icon_tp->setIconColor(TFT_RED, BG_ENABLED_INT);
	} else {
		icon_tp->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	}
	if (reg_latch & INT_OTW) {
		icon_tw->setIconColor(TFT_RED, BG_ENABLED_INT);
	} else {
		icon_tw->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	}
	if (reg_latch & INT_IPCP) {
		icon_ip->setIconColor(TFT_RED, BG_ENABLED_INT);
	} else {
		icon_ip->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	}

	icon_op->icon_update(channel);
	icon_sp->icon_update(channel);
	icon_tp->icon_update(channel);
	icon_tw->icon_update(channel);
	icon_ip->icon_update(channel);

	return reg_latch;
}

uint8_t Channel::checkInterruptStat(void)
{
	uint8_t reg_stat;
	reg_stat = stpd01->readIntStatus();

	if (reg_stat & INT_CC) {
		icon_cc->setIconColor(TFT_RED, BG_ENABLED_INT);
	} else {
		icon_cc->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	}
	icon_cc->icon_update(channel);

	return reg_stat;
}

uint8_t Channel::checkInterrupt(void)
{
	uint8_t reg_stat, reg_latch, reg_mask;
	reg_latch = stpd01->readIntLatch();
	reg_stat = stpd01->readIntStatus();
	reg_mask = stpd01->readIntMask();
	
	return reg_latch;
}
