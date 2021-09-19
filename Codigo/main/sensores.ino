bool onPowerState(const String &deviceId, bool &state) {
  if(deviceId == TEMP_SENSOR_ID){
    Serial.printf("Temperaturesensor turned %s (via SinricPro) \r\n", state ? "on" : "off");
    deviceIsOn = state; // turn on / off temperature sensor
  }
  if(deviceId == DEVICE_ID_MQ2){
    Serial.printf("Device %s turned %s (via SinricPro) \r\n", deviceId.c_str(), state?"on":"off");
    myPowerStateMQ2 = state;
  }
  if(deviceId == DEVICE_ID_MQ7){
    Serial.printf("Device %s turned %s (via SinricPro) \r\n", deviceId.c_str(), state?"on":"off");
    myPowerStateMQ7 = state;
  }
  return true; // request handled properly
}




/* handleTemperatatureSensor()
   - Checks if Temperaturesensor is turned on
   - Checks if time since last event > EVENT_WAIT_TIME to prevent sending too much events
   - Get actual temperature and humidity and check if these values are valid
   - Compares actual temperature and humidity to last known temperature and humidity
   - Send event to SinricPro Server if temperature or humidity changed
*/
void handleTemperaturesensor() {
  if (deviceIsOn == false) return; // device is off...do nothing

  unsigned long actualMillis = millis();
  if (actualMillis - lastEvent < EVENT_WAIT_TIME) return; //only check every EVENT_WAIT_TIME milliseconds

  temperature = dht.readTemperature();          // get actual temperature in °C
  //  temperature = dht.getTemperature() * 1.8f + 32;  // get actual temperature in °F
  humidity = dht.readHumidity();                // get actual humidity

  if (isnan(temperature) || isnan(humidity)) { // reading failed...
    Serial.printf("DHT reading failed!\r\n");  // print error message
    return;                                    // try again next time
  }

  if (temperature == lastTemperature || humidity == lastHumidity) return; // if no values changed do nothing...

  SinricProTemperaturesensor &mySensor = SinricPro[TEMP_SENSOR_ID];  // get temperaturesensor device
  bool success = mySensor.sendTemperatureEvent(temperature, humidity); // send event
  if (success) {  // if event was sent successfuly, print temperature and humidity to serial
    Serial.printf("Temperature: %2.1f Celsius\tHumidity: %2.1f%%\r\n", temperature, humidity);
  } else {  // if sending event failed, print error message
    Serial.printf("Something went wrong...could not send Event to server!\r\n");
  }

  lastTemperature = temperature;  // save actual temperature for next compare
  lastHumidity = humidity;        // save actual humidity for next compare
  lastEvent = actualMillis;       // save actual time for next compare
}






// setup function for SinricPro
void setupSinricPro() {
  // add device to SinricPro
  SinricProTemperaturesensor &mySensor = SinricPro[TEMP_SENSOR_ID];
  mySensor.onPowerState(onPowerState);
   // add device to SinricPro
  SinricProMotionsensor &myMotionsensor_MQ2 = SinricPro[DEVICE_ID_MQ2];

  // set callback function to device
  myMotionsensor_MQ2.onPowerState(onPowerState);

  SinricProMotionsensor &myMotionsensor_MQ7 = SinricPro[DEVICE_ID_MQ7];

  // set callback function to device
  myMotionsensor_MQ7.onPowerState(onPowerState);

  // setup SinricPro
  SinricPro.onConnected([]() {
    Serial.printf("Connected to SinricPro\r\n");
  });
  SinricPro.onDisconnected([]() {
    Serial.printf("Disconnected from SinricPro\r\n");
  });
  SinricPro.begin(APP_KEY, APP_SECRET);
  SinricPro.restoreDeviceStates(true); // get latest known deviceState from server (is device turned on?)
}




/**
 * @brief Checks motionsensor connected to MOTIONSENSOR_PIN
 *
 * If motion sensor state has changed, send event to SinricPro Server
 * state from digitalRead():
 *      HIGH = motion detected
 *      LOW  = motion not detected
 */
void handleMotionsensorMQ2() {
  if (!myPowerStateMQ2) return;                            // if device switched off...do nothing

  unsigned long actualMillis = millis();
  if (actualMillis - lastChangeMQ2 < 250) return;          // debounce motionsensor state transitions (same as debouncing a pushbutton)

  bool actualMotionState = digitalRead(PIN_MQ2);   // read actual state of motion sensor

  if (actualMotionState != lastMotionStateMQ2) {         // if state has changed
    Serial.printf("Motion %s\r\n", actualMotionState?"detected":"not detected");
    lastMotionStateMQ2 = actualMotionState;              // update last known state
    lastChangeMQ2 = actualMillis;                        // update debounce time
    SinricProMotionsensor &myMotionsensor_MQ2 = SinricPro[DEVICE_ID_MQ2]; // get motion sensor device
    myMotionsensor_MQ2.sendMotionEvent(actualMotionState);
  }
}

void handleMotionsensorMQ7() {
  if (!myPowerStateMQ7) return;                            // if device switched off...do nothing

  unsigned long actualMillis = millis();
  if (actualMillis - lastChangeMQ7 < 250) return;          // debounce motionsensor state transitions (same as debouncing a pushbutton)

  bool actualMotionState = digitalRead(PIN_MQ7);   // read actual state of motion sensor

  if (actualMotionState != lastMotionStateMQ7) {         // if state has changed
    Serial.printf("Motion %s\r\n", actualMotionState?"detected":"not detected");
    lastMotionStateMQ7 = actualMotionState;              // update last known state
    lastChangeMQ7 = actualMillis;                        // update debounce time
    SinricProMotionsensor &myMotionsensor_MQ7 = SinricPro[DEVICE_ID_MQ7]; // get motion sensor device
    myMotionsensor_MQ7.sendMotionEvent(actualMotionState);
  }
}
