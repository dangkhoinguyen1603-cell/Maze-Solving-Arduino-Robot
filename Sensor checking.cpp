 Serial.println(F("Put Robot Facing wall"));
  for(f = 38000; f < 70000; f += 500)
  {
      detected = irDetect(irLedPinMid, irReceiverPinMid, f);
      Serial.print(F("Freq: "));
      Serial.print(f);
      Serial.print(F("detected: "));
      Serial.println(detected);
      delay(200);
  }
  delay(5000);