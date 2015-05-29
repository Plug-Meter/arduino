#ifndef Relay_h 
#define Relay_h

enum RelayState {
	ON,
	OFF
};

class Relay {
	public:
		Relay(int pin);
		RelayState getState();
		void setState(RelayState state);
	private:
		void updateRelay();
		int pin;
		RelayState state;
};

#endif