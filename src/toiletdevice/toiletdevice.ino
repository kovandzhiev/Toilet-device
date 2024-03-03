// toiletdevice.ino
// Plamen Kovandzhiev's home projects
// Web: https://github.com/kovandzhiev/Toilet-device
// Supported boards:
//		All KMP ProDino MKR Zero series (https://kmpelectronics.eu/product-category/arduino-mkr-zero/)
// Description:
//		Manage toilet room devices. A LED light and a Fan
// Version: 1.0.0
// Date: 03.03.2024
// Author: Plamen Kovandzhiev <kovandjiev@gmail.com>

#include "KMPProDinoMKRZero.h"
#include "KMPCommon.h"

#define DEBUG
#define FAN_ON_DURATION_MS 30 * 60 * 1000 // 30 min

bool _movements[2];
unsigned long _fanOffTime;

void setup()
{
	delay(5000);
#ifdef DEBUG
	Serial.begin(115200);
#endif

	// Init Dino board. Set pins.
	KMPProDinoMKRZero.init(ProDino_MKR_Zero);

#ifdef DEBUG
	Serial.println("The example RS485Relay is started.");
#endif
}

void loop() {
	bool pirInput = KMPProDinoMKRZero.GetOptoInState(OptoIn1);
	bool doorInput = KMPProDinoMKRZero.GetOptoInState(OptoIn2);

	bool motionDetected = false;
	if(pirInput != _movements[0]) {
		motionDetected = true;
		_movements[0] = pirInput;
	}

	if(doorInput != _movements[1]) {
		motionDetected = true;
		_movements[1] = doorInput;
	}

	processFanLogic();
}

void processFanLogic() {
	bool fanInput = KMPProDinoMKRZero.GetOptoInState(OptoIn3);
	bool fanOn = false;
	if(fanInput) {
		// Add another xx minutes
		_fanOffTime = millis() + FAN_ON_DURATION_MS;
		fanOn = true;
	}

	// The fan has to be On and now is off -> swich On the fan relay
	if(fanOn && !KMPProDinoMKRZero.GetRelayState(Relay1)) {
		KMPProDinoMKRZero.SetRelayState(Relay1, true);
#ifdef DEBUG
		Serial.println("Fan On");
#endif
	}

	// The fan is On and fun On duration is gone -> swich Off the fan relay
	if(KMPProDinoMKRZero.GetRelayState(Relay1) && _fanOffTime < millis()) {
		KMPProDinoMKRZero.SetRelayState(Relay1, false);
#ifdef DEBUG
		Serial.println("Fan Off");
#endif
	}
}