#include "CurrentSensor.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

CurrentSensor::CurrentSensor(const int pin, const int sensitivity) {
	this->pin = pin;
	this->sensitivity = sensitivity; // 100 for ACS712-20A or to 66 for ACS712-30A
	this->VQ = 0;
}

void CurrentSensor::determineVQ() {
	Serial.print("estimating avg. quiscent voltage:");
	long VQ = 0;
	int num_samples = 5000;
	// read a few samples to stabilise value
	for (int i=0; i<num_samples; i++) {
		VQ += abs(analogRead(this->pin));
		delay(1); // depends on sampling (on filter capacitor), can be 1/80000 (80kHz) max.
	}
	VQ /= num_samples;
	Serial.print(map(VQ, 0, 1023, 0, 5000));Serial.println(" mV");
	this->VQ = int(VQ);
}

float CurrentSensor::readCurrent() {
	long current = 0;
	int num_samples = 200;

	// read a few samples to stabilise value
	for (int i=0; i<num_samples; i++) {
		current += abs(analogRead(this->pin) - this->VQ);
		delay(1);
	}
	current = map(current/num_samples, 0, 1023, 0, 5000);
	return float(current)/this->sensitivity;
}
