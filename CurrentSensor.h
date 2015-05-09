#ifndef CurrentSensor_h
#define CurrentSensor_h

class CurrentSensor {
public:
	CurrentSensor(const int pin, const int sensitivity);
	void determineVQ();
	float readCurrent();
private:
	int pin;
	int sensitivity;
	int VQ;
};

#endif /* CurrentSensor_h */