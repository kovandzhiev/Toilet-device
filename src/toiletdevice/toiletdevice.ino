// toiletdevice.ino
// Plamen Kovandzhiev's home projects
// Web: https://github.com/kovandzhiev/Toilet-device
// Supported boards:
//		PRODINo WIFI-ESP WROOM-02 V1 (https://kmpelectronics.eu/products/prodino-wifi-esp-wroom-02-v1/)
// Description:
//		Manage toilet room devices. A LED light and a Fan
// Used libraries:
//      Fluent light (https://github.com/kovandzhiev/FluentLight)
// Version: 1.0.0
// Date: 03.03.2024
// Author: Plamen Kovandzhiev <kovandjiev@gmail.com>

#include "KMPDinoWiFiESP.h"
#include "FluentLight.h"

#define DEBUG
#define FAN_RUNNING_DURATION_MS 30 * 60 * 1000 // 30 min
#define LIGHT_RUNNING_DURATION_MS 3 * 60 * 1000 // 3 min
#define LIGHT_BRIGHTEN_TIME_MS 5 * 1000 // 5 sec
#define LIGHT_FADE_TIME_MS 1 * 60 * 1000 // 1 min

bool _movements[2] {false, false};
unsigned long _fanOffTime;
FluentLight _ledLight(EXT_GROVE_D0);

void setup()
{
#ifdef DEBUG
	Serial.begin(115200);
	Serial.println("Starting...");
#endif

	delay(5000); // Do not block upload of the new sketch

	KMPDinoWiFiESP.init();

	_ledLight.setMaxBrightness(1024);
	_ledLight.setRunningDuration(LIGHT_RUNNING_DURATION_MS);
	_ledLight.setBrightenTime(LIGHT_BRIGHTEN_TIME_MS);
	_ledLight.setFadeTime(LIGHT_FADE_TIME_MS);

	_ledLight.begin();

#ifdef DEBUG
	Serial.println("The toilet device is started.");
#endif
}

void loop() {
	processLedLampLogic();
	processFanLogic();
	_ledLight.process();
}

void processLedLampLogic() {
	bool motionDetected = false;

	bool pirInput = KMPDinoWiFiESP.GetOptoInState(OptoIn1);
	if(pirInput != _movements[0]) {
		motionDetected = true;
		_movements[0] = pirInput;
	}

	bool doorInput = KMPDinoWiFiESP.GetOptoInState(OptoIn2);
	if(doorInput != _movements[1]) {
		motionDetected = true;
		_movements[1] = doorInput;
	}

	if(motionDetected) {
		_ledLight.on();
#ifdef DEBUG
		Serial.println("Light On");
#endif
	}
}

void processFanLogic() {
	bool fanInput = KMPDinoWiFiESP.GetOptoInState(OptoIn3);
	bool fanOn = false;
	if(fanInput) {
		// Add another xx minutes
		_fanOffTime = millis() + FAN_RUNNING_DURATION_MS;
		fanOn = true;
#ifdef DEBUG
		Serial.println("Fan extend running time");
#endif
	}

	// The fan has to be On and now is off -> swich On the fan relay
	if(fanOn && !KMPDinoWiFiESP.GetRelayState(Relay1)) {
		KMPDinoWiFiESP.SetRelayState(Relay1, true);
#ifdef DEBUG
		Serial.println("Fan On");
#endif
	}

	// The fan is On and fun On duration is gone -> swich Off the fan relay
	if(KMPDinoWiFiESP.GetRelayState(Relay1) && _fanOffTime < millis()) {
		KMPDinoWiFiESP.SetRelayState(Relay1, false);
#ifdef DEBUG
		Serial.println("Fan Off");
#endif
	}
}