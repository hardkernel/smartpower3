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
	volt->fnd_init(NUM_OF_FND, 2, true, x, Y_VOLT + y, FG_COLOR, BG_COLOR);
	current->fnd_init(NUM_OF_FND, 2, true, x, Y_CURRENT + y, FG_COLOR, BG_COLOR);
	watt->fnd_init(NUM_OF_FND, 2, true, x, Y_WATT + y, FG_COLOR, BG_COLOR);

	stpd01 = new STPD01();
	/*
	 * Rev0.4 sptd01 address
	 * I2CA : 0x5
	 * I2CB : 0x4
	 * stpd01->begin(0x5 -(channel*1), theWire);
	 *
	 * Rev1.0 sptd01 address
	 * I2CA : 0x5
	 * I2CB : 0x7
	 * stpd01->begin(0x5 +(channel*2), theWire);
	 */
	stpd01->begin(0x5 +(channel*2), theWire);
	//stpd01->begin(0x5 -(channel*1), theWire);

	_volt = new Component(tft, 48, 22, 4);
	_current = new Component(tft, 48, 22, 4);
	stpd = new Component(tft, 38, 16, 2);

	for (int i = 0; i < 8; i++) {
		int_stat[i] = new Component(tft, 14, 22, 2);
		int_latch[i] = new Component(tft, 14, 22, 2);
		int_mask[i] = new Component(tft, 14, 22, 2);
	}
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
	if (test() == 0xff) {
		stpd->setTextColor(FG_DISABLED, BG_DISABLED);
		stpd->draw("STPD");
		return 0;
	}
	stpd->setTextColor(FG_ENABLED, BG_ENABLED);
	stpd->draw("STPD");
	return 1;
}

void Channel::initPower()
{
	if (test() != 1)
		delay(100);
	off();
	/*
	stpd01->setVoltage(volt_set);
	stpd01->setCurrentLimit(current_limit);
	stpd01->initInterrupt();
	*/
}

void Channel::deleteScreen()
{
}

void Channel::initScreen()
{
	_volt->setCoordinate(x + 145, y + 5);
	_volt->init(TFT_YELLOW, TFT_BLACK, 1, TR_DATUM);
	_volt->pushValue(volt_set);
	_current->init(TFT_YELLOW, TFT_BLACK, 1, TR_DATUM);
	_current->setCoordinate(x + 145, y + 5 + Y_CURRENT);
	_current->pushValue(current_limit);

	tft->drawString("Status", x, y + 220, 2);
	tft->drawString("Latch", x, y + 235, 2);
	tft->drawString("Mask", x, y + 250, 2);
	for (int i = 0; i < 8; i++) {
		int_stat[i]->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
		int_stat[i]->setCoordinate(x + 90 + (i*15), y + 220);
		int_stat[i]->draw(String("1"));
		int_latch[i]->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
		int_latch[i]->setCoordinate(x + 90 + (i*15), y + 235);
		int_latch[i]->draw(String("1"));
		int_mask[i]->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
		int_mask[i]->setCoordinate(x + 90 + (i*15), y + 250);
		int_mask[i]->draw(String("1"));
	}

	//FG_DISABLED, BG_DISABLED
	//stpd->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
	stpd->init(FG_ENABLED, BG_ENABLED, 1, TR_DATUM);
	//stpd->init(FG_DISABLED, BG_DISABLED, 1, TR_DATUM);
	stpd->setCoordinate(x + 180, y + 180);
	stpd->draw(String("STPD"));

	tft->drawString("V", x + W_SEG-4, y + 38, 4);
	tft->drawString("A", x + W_SEG-4, y + 29 + H_SEG + OFFSET_SEG, 4);
	tft->drawString("W", x + W_SEG-4, y + 10 + H_SEG*2 + OFFSET_SEG*2, 4);
	drawChannel(true);
}

uint8_t Channel::getIntStatus(void)
{
	return stpd01->readIntStatus();
}

uint8_t Channel::getIntMask(void)
{
	return stpd01->readIntMask();
}

bool Channel::on(void)
{
	bool err;
	stpd01->setVoltage(volt_set);
	stpd01->setCurrentLimit(current_limit);
	stpd01->initInterrupt();
	err = stpd01->on();
	volt->setTextColor(TFT_RED, TFT_BLACK);
	current->setTextColor(TFT_RED, TFT_BLACK);
	watt->setTextColor(TFT_RED, TFT_BLACK);
	return err;
}

uint8_t Channel::test(void)
{
	return stpd01->read8(STPD01_REGISTER_6);
}

bool Channel::off(void)
{
	bool err;
	err = stpd01->off();
	volt->setTextColor(TFT_DARKGREY, TFT_BLACK);
	current->setTextColor(TFT_DARKGREY, TFT_BLACK);
	watt->setTextColor(TFT_DARKGREY, TFT_BLACK);
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
	//return volt_set;
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

void Channel::drawChannel(bool forced)
{
	if (!hide) {
		volt->fnd_update(forced);
		current->fnd_update(forced);
		watt->fnd_update(forced);
		//watt->draw();
		_volt->draw(forced);
		_current->draw(forced);
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
		//_volt->setTextColor(TFT_YELLOW, TFT_BLACK);
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
	this->volt->pushValue(volt);
	this->current->pushValue(current);
	this->watt->pushValue(watt);
}

void Channel::pushPowerEdit(uint16_t volt, uint16_t current, uint16_t watt)
{
	this->volt->pushValue(volt);
	this->current->pushValue(current);
	this->watt->pushValue(watt);

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

void Channel::monitorInterrupt()
{
	stpd01->monitorInterrupt(channel);
}

void Channel::clearInterruptUI(void)
{
	for (int i = 0; i < 8; i++) {
		int_stat[i]->setTextColor(TFT_WHITE, TFT_DARKGREY);
		int_stat[i]->draw(String("0"));
		int_latch[i]->setTextColor(TFT_WHITE, TFT_DARKGREY);
		int_latch[i]->draw(String("0"));
		int_mask[i]->setTextColor(TFT_WHITE, TFT_DARKGREY);
		int_mask[i]->draw(String("0"));
	}
}

uint8_t Channel::checkInterrupt(void)
{
	uint8_t reg_stat, reg_latch, reg_mask;
	reg_latch = stpd01->readIntLatch();
	reg_stat = stpd01->readIntStatus();
	reg_mask = stpd01->readIntMask();
	for (int i = 0; i < 8; i++) {
		if (reg_stat & (1 << i)) {
			int_stat[7-i]->setTextColor(TFT_YELLOW, TFT_BLACK);
			int_stat[7-i]->draw(String("1"));
		} else {
			int_stat[7-i]->setTextColor(TFT_WHITE, TFT_DARKGREY);
			int_stat[7-i]->draw(String("0"));
		}
		if (reg_latch & (1 << i)) {
			int_latch[7-i]->setTextColor(TFT_YELLOW, TFT_BLACK);
			int_latch[7-i]->draw(String("1"));
		} else {
			int_latch[7-i]->setTextColor(TFT_WHITE, TFT_DARKGREY);
			int_latch[7-i]->draw(String("0"));
		}
		if (reg_mask & (1 << i)) {
			int_mask[7-i]->setTextColor(TFT_YELLOW, TFT_BLACK);
			int_mask[7-i]->draw(String("1"));
		} else {
			int_mask[7-i]->setTextColor(TFT_WHITE, TFT_DARKGREY);
			int_mask[7-i]->draw(String("0"));
		}
	}
	
	return reg_latch;
}
