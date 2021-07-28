#include "channel.h"

Channel::Channel(TFT_eSPI *tft, TwoWire *theWire, uint8_t channel)
{
	this->tft = tft;
	this->channel = channel;

	volt = new FndWidget(tft);
	current = new FndWidget(tft);
	watt = new FndWidget(tft);

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
		int_stat[i] = new Component(tft, 14, 22, 4);
		int_latch[i] = new Component(tft, 14, 22, 4);
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

void Channel::monitorSTPD01()
{
	if (test() == 0xff) {
		stpd->setTextColor(FG_DISABLED, BG_DISABLED);
		stpd->draw("STPD");
	} else {
		stpd->setTextColor(FG_ENABLED, BG_ENABLED);
		stpd->draw("STPD");
#if 0
		char str[5];
		sprintf(str, "%4.1f", stpd01->readVoltage()/1000.0);
		//Serial.println(stpd01->readVoltage(), HEX);
		//sprintf(str, "%x", stpd01->readVoltage());
		sprintf(str, "%3.1f", stpd01->readCurrentLimit()/1000.0);
#endif
	}
	monitorInterrupt();
}

void Channel::initPower()
{
	if (test() != 1)
		delay(100);
	off();
	stpd01->setVoltage(volt_set);
	stpd01->setCurrentLimit(current_limit);
	stpd01->initInterrupt();
}

void Channel::initScreen(uint16_t x, uint16_t y)
{
	volt->fnd_init(NUM_OF_FND, 2, true, x, Y_VOLT + y, FG_COLOR, BG_COLOR);
	current->fnd_init(NUM_OF_FND, 2, true, x, Y_CURRENT + y, FG_COLOR, BG_COLOR);
	watt->fnd_init(NUM_OF_FND, 2, true, x, Y_WATT + y, FG_COLOR, BG_COLOR);

	_volt->setCoordinate(x + 145, y + 5);
	_volt->init(TFT_YELLOW, TFT_BLACK, 1, TR_DATUM);
	_volt->pushValue(volt_set);
	_current->init(TFT_YELLOW, TFT_BLACK, 1, TR_DATUM);
	_current->setCoordinate(x + 145, y + 5 + Y_CURRENT);
	_current->pushValue(current_limit);

	tft->drawString("Status", x, y + 230, 4);
	tft->drawString("Latch", x, y + 255, 4);
	for (int i = 0; i < 8; i++) {
		int_stat[i]->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
		int_stat[i]->setCoordinate(x + 90 + (i*15), y + 230);
		int_stat[i]->draw(String("1"));
		int_latch[i]->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
		int_latch[i]->setCoordinate(x + 90 + (i*15), y + 255);
		int_latch[i]->draw(String("1"));
	}

	//FG_DISABLED, BG_DISABLED
	//stpd->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
	stpd->init(FG_ENABLED, BG_ENABLED, 1, TR_DATUM);
	//stpd->init(FG_DISABLED, BG_DISABLED, 1, TR_DATUM);
	stpd->setCoordinate(x + 180, y + 200);
	stpd->draw(String("STPD"));

	tft->drawString("V", x + W_SEG-4, y + 38, 4);
	tft->drawString("A", x + W_SEG-4, y + 29 + H_SEG + OFFSET_SEG, 4);
	tft->drawString("W", x + W_SEG-4, y + 20 + H_SEG*2 + OFFSET_SEG*2, 4);
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
	clearInterruptUI();
	return err;
}

void Channel::write(uint8_t addr, uint8_t reg)
{
	stpd01->write8(addr, reg);
}

uint16_t Channel::getVolt(void)
{
	//return volt_set;
	Serial.println(volt_set);
	return volt_set;
}

uint16_t Channel::getCurrentLimit(void)
{
	Serial.printf("getCurrentLimit %d\n\r", current_limit);
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

void Channel::drawChannel(void)
{
	volt->fnd_update();
	current->fnd_update();
	watt->fnd_update();
	//watt->draw();
	_volt->draw();
	_current->draw();
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
	Serial.printf("getValueCurrent %d\n\r", this->current->getValue());
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
	}
}

void Channel::checkInterrupt(void)
{
	uint8_t reg_stat, reg_latch;
	reg_stat = stpd01->readIntStatus();
	reg_latch = stpd01->readIntLatch();
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
	}
}
