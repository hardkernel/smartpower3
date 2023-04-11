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

	icon_op = new ProtectionChannelIconWidget(tft, channel, PROTECTION_OVERVOLTAGE);  //0
	icon_sp = new ProtectionChannelIconWidget(tft, channel, PROTECTION_SHORT_CIRCUIT);  //2
	icon_tp = new ProtectionChannelIconWidget(tft, channel, PROTECTION_TEMPERATURE);  //3
	icon_tw = new ProtectionChannelIconWidget(tft, channel, PROTECTION_TEMPERATURE_WARNING);  //4
	icon_ip = new ProtectionChannelIconWidget(tft, channel, PROTECTION_INDUCTOR_PEAK);  //5

	icon_positive = new PolarityChannelIconWidget(tft, channel, POSITIVE);
	icon_negative = new PolarityChannelIconWidget(tft, channel, NEGATIVE);

	icon_volt = new UnitChannelIconWidget(tft, channel, LARGE_VOLT);  //6
	icon_ampere = new UnitChannelIconWidget(tft, channel, LARGE_AMPERE);  //7
	icon_watt = new UnitChannelIconWidget(tft, channel, LARGE_WATT);  //8

	volt->fnd_init(NUM_OF_FND, 2, true, x + 40, + y+10, FG_COLOR, BG_COLOR, FND_FONT_32x64, 10);
	volt->setMonitorVoltage();
	current->fnd_init(NUM_OF_FND, 2, true, x + 40, 64 + 5 + y+10, FG_COLOR, BG_COLOR, FND_FONT_32x64, 10);
	current->setCurrentLimit();
	watt->fnd_init(NUM_OF_FND, 2, true, x + 40, 64*2 + 10 + y+10, FG_COLOR, BG_COLOR, FND_FONT_32x64, 10);

	_volt->fnd_init(3, 1, true, x + W_SEG-2, y+12, TFT_YELLOW, BG_COLOR, FND_FONT_16x32, 100);
	_current->fnd_init(2, 1, true, x + W_SEG+14, y+18+64, TFT_YELLOW, BG_COLOR, FND_FONT_16x32, 100);

	uint16_t gap_int_icon = 25;
	uint16_t base = 52;

	icon_op->init(x, y+base, FG_COLOR, BG_COLOR);
	icon_sp->init(x, y+base + gap_int_icon*1, FG_COLOR, BG_COLOR);
	icon_tp->init(x, y+base + gap_int_icon*2, FG_COLOR, BG_COLOR);
	icon_tw->init(x, y+base + gap_int_icon*3, FG_COLOR, BG_COLOR);
	icon_ip->init(x, y+base + gap_int_icon*4, FG_COLOR, BG_COLOR);

	icon_positive->init(x + 26 + channel*(10), y+234, TFT_RED, BG_ENABLED_INT);
	icon_negative->init(x + 156 + channel*(10), y+234, TFT_DARKGREY, BG_ENABLED_INT);

	icon_volt->init(x + W_SEG-10, y+46, FG_COLOR, BG_COLOR);
	icon_ampere->init(x + W_SEG-10, y+32 + H_SEG+OFFSET_SEG, FG_COLOR, BG_COLOR);
	icon_watt->init(x + W_SEG-10, y+20+H_SEG*2+OFFSET_SEG*2, FG_COLOR, BG_COLOR);

	stpd01 = new STPD01(0x5 + (channel*2), theWire);

#ifdef DEBUG_STPD01
	stpd = new Component(tft, 32, 16, 2);
	lowV = new Component(tft, 16, 16, 2);
	for (int i = 0; i < 8; i++) {
		if (i == 3 or i == 4)
			continue;
		debug_intr[i] = new Component(tft, 16, 16, 2);
	}
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

	if (flag_clear_debug == 0) {
		flag_clear_debug++;
		lowV->clear();
		for (int i = 0; i < 8; i++) {
			if (i == 3 or i == 4)
				continue;
			debug_intr[i]->clear();
		}
	}

	lowV->draw(String(low_volt));
	for (int i = 0; i < 8; i++) {
		if (i == 3 or i == 4)
			continue;
		debug_intr[i]->draw(String(count_intr[i]));
	}

#endif
	return 1;
}

void Channel::initScreen(uint8_t onoff)
{
	_volt->pushValue(volt_set);
	_current->pushValue(current_limit);

#ifdef DEBUG_STPD01
	stpd->init(FG_ENABLED, BG_ENABLED, 1, TR_DATUM);
	stpd->setCoordinate(x, y + 200);
	stpd->draw(String("STPD"));

	lowV->init(FG_ENABLED, BG_ENABLED, 1, TR_DATUM);
	lowV->setCoordinate(x + 100, y + 250);
	lowV->draw(String(low_volt));

	for (int i = 0; i < 8; i++) {
		if (i == 3 or i == 4)
			continue;
		debug_intr[i]->init(FG_ENABLED, BG_ENABLED, 1, TR_DATUM);
		debug_intr[i]->setCoordinate(x + 34 + i*18, y + 216);
		debug_intr[i]->draw(String(0));
	}
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
		icon_volt->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_volt->draw();
		icon_ampere->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_ampere->draw();
		icon_watt->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_watt->draw();
	} else {
		icon_volt->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_volt->draw();
		icon_ampere->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_ampere->draw();
		icon_watt->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_watt->draw();
	}
}

void Channel::drawPolarity(void)
{
	icon_positive->draw();
	icon_negative->draw();
}

void Channel::drawInterrupt(void)
{
	icon_op->update(true);
	icon_sp->update(true);
	icon_tp->update(true);
	icon_tw->update(true);
	icon_ip->update(true);
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
		if (cnt++ > 10)
			break;
		delay(100);
	}
}

void Channel::disabled(void)
{
	pinMode(en_stpd01[channel], OUTPUT);
	digitalWrite(en_stpd01[channel], LOW);

	volt->setTextColor(TFT_DARKGREY, BG_COLOR);
	current->setTextColor(TFT_DARKGREY, BG_COLOR);
	watt->setTextColor(TFT_DARKGREY, BG_COLOR);
}

bool Channel::on(void)
{
	bool err = 0;
	if (stpd01->available()) {
		stpd01->setVoltage(volt_set);
		stpd01->setCurrentLimit(current_limit);
		stpd01->initInterrupt();
		err = stpd01->on();
	}
	volt->setTextColor(TFT_RED, BG_COLOR);
	current->setTextColor(TFT_RED, BG_COLOR);
	watt->setTextColor(TFT_RED, BG_COLOR);
	if (!hide) {
		icon_volt->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_volt->draw();
		icon_ampere->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_ampere->draw();
		icon_watt->setIconColor(TFT_RED, BG_ENABLED_INT);
		icon_watt->draw();
	}
	return err;
}

bool Channel::off(void)
{
	bool err = 0;
	if (stpd01->available()) {
		err = stpd01->off();
	}
	volt->setTextColor(TFT_DARKGREY, BG_COLOR);
	current->setTextColor(TFT_DARKGREY, BG_COLOR);
	watt->setTextColor(TFT_DARKGREY, BG_COLOR);
	volt->pushValue(0);
	current->pushValue(0);
	watt->pushValue(0);
	if (!hide) {
		icon_volt->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_volt->draw();
		icon_ampere->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_ampere->draw();
		icon_watt->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
		icon_watt->draw();
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
	return current_limit;
}

uint16_t Channel::countVolt(float dial_count)
{
	int16_t step_change_voltage = 11000;
	uint16_t steps = abs(dial_count);
	uint16_t return_volt_set = this->volt_set;
	int16_t distance_to_step_change = this->volt_set-step_change_voltage;

	if (distance_to_step_change <= 0 && dial_count > 0) {  // below limit and upwards
		while (return_volt_set < step_change_voltage && steps > 0) {
			steps--;
			return_volt_set += 100;
		}
		return_volt_set += steps*200;
	} else if (distance_to_step_change <= 0 && dial_count < 0) {  // below limit and downwards
		return_volt_set -= steps*100;
	} else if (distance_to_step_change >= 0 && dial_count > 0) {  // above limit and upwards
		return_volt_set += steps*200;
	} else if (distance_to_step_change >= 0 && dial_count < 0) {  // above limit and downwards
		while (return_volt_set > step_change_voltage && steps > 0) {
			steps--;
			return_volt_set -= 200;
		}
		return_volt_set -= steps*100;
	}
	return return_volt_set;
}

void Channel::setVolt(float volt_set, uint8_t mode)
{
	if (mode == 0) {
		this->volt_set = this->countVolt(volt_set);
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
	this->_volt_set = this->countVolt(volt_set);
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
	_volt->setTextColor(TFT_YELLOW, BG_COLOR);
	_current->setTextColor(TFT_YELLOW, BG_COLOR);
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

void Channel::select(comp_t comp)
{
	switch (comp) {
		case VOLT:
			_volt->select();
			break;
		case CURRENT:
			_current->select();
			break;
		case WATT:
			break;
	}
}

void Channel::deSelect(comp_t comp)
{
	switch (comp) {
		case VOLT:
			_volt->deSelect();
			break;
		case CURRENT:
			_current->deSelect();
			break;
		case WATT:
			break;
	}
}

void Channel::setIntFlag(void)
{
	flag_int = true;
}

void Channel::isr(uint8_t onoff)
{
	if (flag_int || !digitalRead(int_stpd01[channel])) {
		if (stpd01->available()) {
			latch = checkInterruptLatch();
			for (int i = 0; i < 8; i++) {
				if ((latch >> i) & 0x1) {
					count_intr[i]++;
				}
			}
			if (latch & INT_OTP)
				off();
			else if (latch & (INT_OVP|INT_SCP|INT_OTW|INT_IPCP))
				on();
			flag_int = false;
		}
	}
}

uint8_t Channel::checkInterruptLatch(void)
{
	uint8_t reg_latch;

	reg_latch = stpd01->readIntLatch();
	icon_op->setProtection(reg_latch & INT_OVP);
	icon_sp->setProtection(reg_latch & INT_SCP);
	icon_tp->setProtection(reg_latch & INT_OTP);
	icon_tw->setProtection(reg_latch & INT_OTW);
	icon_ip->setProtection(reg_latch & INT_IPCP);

	if (!hide) {
		icon_op->update();
		icon_sp->update();
		icon_tp->update();
		icon_tw->update();
		icon_ip->update();
	}

	return reg_latch;
}

uint8_t Channel::checkInterruptStat(uint8_t onoff)
{
	uint8_t reg_stat;
	reg_stat = stpd01->readIntStatus();

	if (onoff == 0)
		reg_stat = 0x00;
	icon_op->setProtection(reg_stat & INT_OVP);
	icon_sp->setProtection(reg_stat & INT_SCP);
	icon_tp->setProtection(reg_stat & INT_OTP);
	icon_tw->setProtection(reg_stat & INT_OTW);
	icon_ip->setProtection(reg_stat & INT_IPCP);

	if (flag_clear_debug == 1) {
		flag_clear_debug++;
	}

	if (!hide) {
		icon_op->update();
		icon_sp->update();
		icon_tp->update();
		icon_tw->update();
		icon_ip->update();
	}

	return reg_stat;
}

uint8_t Channel::checkInterrupt(void)
{
	uint8_t reg_latch;
	reg_latch = stpd01->readIntLatch();
	
	return reg_latch;
}

void Channel::countLowVoltage()
{
	low_volt++;
}

void Channel::clearLowVoltage()
{
	low_volt = 0;
}

void Channel::clearDebug()
{
	clearLowVoltage();
	icon_op->setProtection(0);
	icon_sp->setProtection(0);
	icon_tp->setProtection(0);
	icon_tw->setProtection(0);
	icon_ip->setProtection(0);
	for (int i = 0; i < 8; i++)
		count_intr[i] = 0;
	flag_clear_debug = 0;
}
