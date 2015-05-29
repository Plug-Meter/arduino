#include "Relay.h"

#include <Arduino.h>
#include <EEPROM.h>

#define EEPROM_RELAY_STATE_ADDR 0

Relay::Relay(int pin)
{
	this->pin = pin;

	pinMode(pin, OUTPUT);
	EEPROM.get(EEPROM_RELAY_STATE_ADDR, this->state);

	this->updateRelay();
}

RelayState Relay::getState()
{
	return this->state;
}

void Relay::setState(RelayState state)
{
	this->state = state;
	EEPROM.put(EEPROM_RELAY_STATE_ADDR, state);

	this->updateRelay();
}

void Relay::updateRelay() {
	if (this->state == ON)
	{
		digitalWrite(this->pin, LOW);
	}
	else if (this->state == OFF)
	{
		digitalWrite(this->pin, HIGH);
	}
}