#include "channel.h"

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

	_volt = new FndWidget(tft);
	_current = new FndWidget(tft);

	icon_op = new IconWidget(tft, channel);
	icon_sp = new IconWidget(tft, channel);
	icon_cc = new IconWidget(tft, channel);
	icon_tp = new IconWidget(tft, channel);
	icon_tw = new IconWidget(tft, channel);
	icon_ip = new IconWidget(tft, channel);

	icon_p = new IconWidget(tft, channel);
	icon_n = new IconWidget(tft, channel);

	icon_v = new IconWidget(tft, channel);
	icon_w = new IconWidget(tft, channel);
	icon_a = new IconWidget(tft, channel);

	volt->fnd_init(NUM_OF_FND, 2, true, x + 40, + y+10, FG_COLOR, BG_COLOR);
	current->fnd_init(NUM_OF_FND, 2, true, x + 40, 64 + 5 + y+10, FG_COLOR, BG_COLOR);
	watt->fnd_init(NUM_OF_FND, 2, true, x + 40, 64*2 + 10 + y+10, FG_COLOR, BG_COLOR);

	_volt->fnd_init(3, 1, true, x + W_SEG-2, y+12, TFT_YELLOW, BG_COLOR, FND_FONT_16x32, 10);
	_current->fnd_init(2, 1, true, x + W_SEG+14, y+18+64, TFT_YELLOW, BG_COLOR, FND_FONT_16x32, 10);

	uint16_t gap_int_icon = 25;
	uint16_t base = 40;
	icon_op->init(0, x, y+base, FG_COLOR, BG_COLOR);
	icon_cc->init(1, x, y+base + gap_int_icon, FG_COLOR, BG_COLOR);
	icon_sp->init(2, x, y+base + gap_int_icon*2, FG_COLOR, BG_COLOR);
	icon_tp->init(3, x, y+base + gap_int_icon*3, FG_COLOR, BG_COLOR);
	icon_tw->init(4, x, y+base + gap_int_icon*4, FG_COLOR, BG_COLOR);
	icon_ip->init(5, x, y+base + gap_int_icon*5, FG_COLOR, BG_COLOR);

	icon_p->init(9, x + 26 + channel*(10), y+234, FG_COLOR, BG_COLOR);
	icon_n->init(9, x + 156 + channel*(10), y+234, FG_COLOR, BG_COLOR);

	icon_v->init(9, x + W_SEG-10, y+46, FG_COLOR, BG_COLOR);
	icon_a->init(9, x + W_SEG-10, y+32 + H_SEG+OFFSET_SEG, FG_COLOR, BG_COLOR);
	icon_w->init(9, x + W_SEG-10, y+20+H_SEG*2+OFFSET_SEG*2, FG_COLOR, BG_COLOR);

	stpd01 = new STPD01(0x5 + (channel*2), theWire);

#ifdef DEBUG_STPD01
	stpd = new Component(tft, 32, 16, 2);
#endif
	pinMode(int_stpd01[channel], INPUT_PULLUP);
}

Channel::~Channel(void)
{
	delete volt;
	delete current;
	delete watt;
	delete _volt;
	delete _current;
	volt = NULL;
	current = NULL;
	watt = NULL;
	_volt = NULL;
	_current = NULL;
}

bool Channel::isAvailableSTPD01()
{
#ifdef DEBUG_STPD01
	if (!stpd01->available()) {
		stpd->setTextColor(FG_DISABLED, BG_DISABLED);
		stpd->draw("STPD");
		return 0;
	}
	stpd->setTextColor(FG_ENABLED, BG_ENABLED);
	stpd->draw("STPD");
	return 1;
#endif
}

void Channel::initScreen(uint8_t onoff)
{
	_volt->pushValue(volt_set);
	_current->pushValue(current_limit);

#ifdef DEBUG_STPD01
	stpd->init(FG_ENABLED, BG_ENABLED, 1, TR_DATUM);
	stpd->setCoordinate(x, y + 210);
	stpd->draw(String("STPD"));
#endif

	drawChannel(true);
	drawVoltSet(true);
	drawInterrupt();
	drawPolarity();
	if ((onoff == 1) or (onoff == 1))
		drawUnits(true);
	else
		drawUnits(false);
}

void Channel::drawUnits(bool onoff)
{
	if (onoff) {
		icon_v->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_v->unitsWrite(0);
		icon_a->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_a->unitsWrite(1);
		icon_w->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_w->unitsWrite(2);
	} else {
		icon_v->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_v->unitsWrite(0);
		icon_a->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_a->unitsWrite(1);
		icon_w->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_w->unitsWrite(2);
	}
}

void Channel::drawPolarity(void)
{
	icon_p->setIconColor(TFT_RED, BG_ENABLED_INT);
	icon_p->polarityWrite(1);
	icon_n->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	icon_n->polarityWrite(0);
}

void Channel::drawInterrupt(void)
{
	icon_op->update(true, channel);
	icon_sp->update(true, channel);
	icon_cc->update(true, channel);
	icon_tp->update(true, channel);
	icon_tw->update(true, channel);
	icon_ip->update(true, channel);
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
	if (stpd01->available()) {
		stpd01->setVoltage(volt_set);
		stpd01->setCurrentLimit(current_limit);
		stpd01->initInterrupt();
		err = stpd01->on();
	}
	volt->setTextColor(TFT_RED, TFT_BLACK);
	current->setTextColor(TFT_RED, TFT_BLACK);
	watt->setTextColor(TFT_RED, TFT_BLACK);
	if (!hide) {
		icon_v->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_v->unitsWrite(0);
		icon_a->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_a->unitsWrite(1);
		icon_w->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_w->unitsWrite(2);
	}
	return err;
}

bool Channel::off(void)
{
	bool err;
	if (stpd01->available()) {
		err = stpd01->off();
	}
	volt->setTextColor(TFT_DARKGREY, TFT_BLACK);
	current->setTextColor(TFT_DARKGREY, TFT_BLACK);
	watt->setTextColor(TFT_DARKGREY, TFT_BLACK);
	volt->pushValue(0);
	current->pushValue(0);
	watt->pushValue(0);
	if (!hide) {
		icon_v->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_v->unitsWrite(0);
		icon_a->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_a->unitsWrite(1);
		icon_w->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_w->unitsWrite(2);
	}
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
	} else if (mode == 2) {
		this->volt_set = volt_set;
		this->_volt_set = volt_set;
		this->_volt->pushValue(min((uint16_t)20000, _volt_set));
		stpd01->setVoltage(min((uint16_t)20000, this->_volt_set));
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
	} else if (mode == 2) {
		current_limit = val;
		_current_limit = current_limit;
		_current_limit = min((uint16_t)3000, _current_limit);
		_current_limit = max((uint16_t)500, _current_limit);
		this->_current->pushValue(_current_limit);
		stpd01->setCurrentLimit(current_limit);
	}
}

void Channel::editCurrentLimit(float val)
{
	_current_limit = current_limit + val*100;
	_current_limit = min((uint16_t)3000, _current_limit);
	_current_limit = max((uint16_t)500, _current_limit);
	Serial.println(_current_limit);
}

void Channel::drawVoltSet(bool forced)
{
	if (!hide) {
		_volt->fnd_update(forced);
		_current->fnd_update(forced);
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
	_volt->fnd_update(true);
	_current->fnd_update(true);
}

void Channel::setCompColor(comp_t comp)
{
	if (comp == VOLT) {
		_volt->setTextColor(TFT_RED, BG_COLOR);
		_volt->fnd_update(true);
	} else if (comp == CURRENT) {
		_current->setTextColor(TFT_RED, BG_COLOR);
		_current->fnd_update(true);
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
	}

	if (current < 10) {
		current = 0;
		watt = 0;
	}

	this->volt->pushValue(volt, volt_set);
	this->current->pushValue(current, current_limit);
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
		if (stpd01->available()) {
			latch = checkInterruptLatch();
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
	icon_op->setInt(reg_latch & INT_OVP);
	icon_sp->setInt(reg_latch & INT_SCP);
	icon_tp->setInt(reg_latch & INT_OTP);
	icon_tw->setInt(reg_latch & INT_OTW);
	icon_ip->setInt(reg_latch & INT_IPCP);

	if (!hide) {
		icon_op->update(channel);
		icon_sp->update(channel);
		icon_tp->update(channel);
		icon_tw->update(channel);
		icon_ip->update(channel);
	}

	return reg_latch;
}

uint8_t Channel::checkInterruptStat(uint8_t onoff)
{
	uint8_t reg_stat;
	reg_stat = stpd01->readIntStatus();

	if (onoff == 0)
		reg_stat = 0x00;

	if (reg_stat & INT_CC) {
		icon_cc->setIconColor(TFT_RED, BG_ENABLED_INT);
	} else {
		icon_cc->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	}
	icon_cc->update(channel);

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
