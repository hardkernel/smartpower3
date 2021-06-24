#include "channel.h"

Channel::Channel(TFT_eSPI *tft, TwoWire *theWire, uint8_t channel)
{
	this->tft = tft;
	this->channel = channel;

	volt = new FndWidget(tft);
	current = new FndWidget(tft);
	watt = new FndWidget(tft);
	//watt = new Component(tft, W_SEG, H_SEG, 7);

	stpd01 = new STPD01();
	stpd01->begin(0x5 -(channel*1), theWire);
	//stpd01->setCurrentLimit(3000);

	_volt = new Component(tft, 64, 22, 4);
	_current = new Component(tft, 64, 22, 4);

	_int_cc = new Component(tft, 40, 22, 2);
	_int_ovp = new Component(tft, 48, 22, 2);
	_int_otp = new Component(tft, 48, 22, 2);
	_int_otw = new Component(tft, 54, 22, 2);
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

void Channel::initPower()
{
	if (test() != 1)
		delay(100);
	off();
	stpd01->setVoltage(volt_set);
	stpd01->setCurrentLimit(current_limit);
}

void Channel::initScreen(uint16_t x, uint16_t y)
{
	volt->fnd_init(NUM_OF_FND, 2, true, x, Y_VOLT + y, FG_COLOR, BG_COLOR);
	current->fnd_init(NUM_OF_FND, 2, true, x, Y_CURRENT + y, FG_COLOR, BG_COLOR);
	watt->fnd_init(NUM_OF_FND, 2, true, x, Y_WATT + y, FG_COLOR, BG_COLOR);
	//watt->setCoordinate(x, y + H_SEG*2 + OFFSET_SEG*2);
	//watt->init(TFT_RED, TFT_BLACK, 1, TR_DATUM);
	_volt->setCoordinate(x + 145, y + 5);
	_volt->init(TFT_YELLOW, TFT_BLACK, 1, TR_DATUM);
	_volt->pushValue(5000);

	_current->init(TFT_YELLOW, TFT_BLACK, 1, TR_DATUM);
	_current->setCoordinate(x + 145, y + 5 + Y_CURRENT);
	_current->pushValue(3000);

	_int_cc->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
	_int_cc->setCoordinate(x-10, y + 230);
	_int_cc->draw(String("CC"));

	_int_ovp->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
	_int_ovp->setCoordinate(x + 40, y + 230);
	_int_ovp->draw(String("OVP"));

	_int_otp->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
	_int_otp->setCoordinate(x + 100, y + 230);
	_int_otp->draw(String("OTP"));

	_int_otw->init(TFT_YELLOW, TFT_DARKGREY, 1, TR_DATUM);
	_int_otw->setCoordinate(x + 160, y + 230);
	_int_otw->draw(String("OTW"));

	tft->drawString("V", x + W_SEG-4, y + 38, 4);
	tft->drawString("A", x + W_SEG-4, y + 29 + H_SEG + OFFSET_SEG, 4);
	tft->drawString("W", x + W_SEG-4, y + 20 + H_SEG*2 + OFFSET_SEG*2, 4);
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
	//clearInterruptUI();
	Serial.printf("channel off %d\n\r", err);
	return err;
}

uint16_t Channel::getVolt(void)
{
	return volt_set;
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
		this->_volt_set = this->volt_set + volt_set*100;
	}
}

void Channel::editVolt(float volt_set)
{
	this->_volt_set = this->volt_set + volt_set*100;
}

uint16_t Channel::getCurrentLimit(void)
{
	return current_limit/100;
}

void Channel::setCurrentLimit(float val)
{
	current_limit += val*100;
	current_limit = min((uint16_t)3000, current_limit);
	stpd01->setCurrentLimit(current_limit);
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
	_int_cc->setTextColor(TFT_WHITE, TFT_DARKGREY);
	_int_cc->draw(String("CC"));
	_int_ovp->setTextColor(TFT_WHITE, TFT_DARKGREY);
	_int_ovp->draw(String("OVP"));
	_int_otp->setTextColor(TFT_WHITE, TFT_DARKGREY);
	_int_otp->draw(String("OTP"));
	_int_otw->setTextColor(TFT_WHITE, TFT_DARKGREY);
	_int_otw->draw(String("OTW"));
}

void Channel::checkInterrupt(void)
{
	uint8_t reg;
	reg = stpd01->readInterrupt();
	if (reg & INT_CONSTANT_CURRENT_FUNCTION) {
		_int_cc->setTextColor(TFT_WHITE, TFT_DARKGREY);
		_int_cc->draw(String("CC"));
	} else {
		_int_cc->setTextColor(TFT_YELLOW, TFT_BLACK);
		_int_cc->draw(String("CC"));
	}
	if (reg & INT_OVERVOLTAGE_PROTECTION) {
		_int_ovp->setTextColor(TFT_YELLOW, TFT_BLACK);
		_int_ovp->draw(String("OVP"));
	} else {
		_int_ovp->setTextColor(TFT_WHITE, TFT_DARKGREY);
		_int_ovp->draw(String("OVP"));
	}
	/*
	if (reg & INT_SHORT_CIRCUIT_PROTECTION)
		Serial.println("SCP");
		*/
	if (reg & INT_OVERTEMPERATURE_PROTECTION) {
		_int_ovp->setTextColor(TFT_YELLOW, TFT_BLACK);
		_int_otp->draw(String("OTP"));
	} else {
		_int_otp->setTextColor(TFT_WHITE, TFT_DARKGREY);
		_int_otp->draw(String("OTP"));
	}
	if (reg & INT_OVERTEMPERATURE_WARNING) {
		_int_otw->setTextColor(TFT_YELLOW, TFT_BLACK);
		_int_otw->draw(String("OTW"));
	} else {
		_int_otw->setTextColor(TFT_WHITE, TFT_DARKGREY);
		_int_otw->draw(String("OTW"));
	}
}
