//our callback functions
void firstLightChanged(uint8_t brightness) {
  if (brightness == 255)
    {
      digitalWrite(RelayPin1, LOW);
      Serial.println("Device1 ON");
    }
  else
  {
    digitalWrite(RelayPin1, HIGH);
    Serial.println("Device1 OFF");
  }
}

void secondLightChanged(uint8_t brightness) {
  if (brightness == 255)
    {
      digitalWrite(RelayPin2, LOW);
      Serial.println("Device2 ON");
    }
  else
  {
    digitalWrite(RelayPin2, HIGH);
    Serial.println("Device2 OFF");
  }
}

void thirdLightChanged(uint8_t brightness) {
  if (brightness == 255)
    {
      digitalWrite(RelayPin3, LOW);
      Serial.println("Device3 ON");
    }
  else
  {
    digitalWrite(RelayPin3, HIGH);
    Serial.println("Device3 OFF");
  }
}
