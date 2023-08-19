const uint8_t DC_FAN_MOTOR_INA = D2;
const uint8_t DC_FAN_MOTOR_INB = D3;

unsigned char valFanMotorSpeed = 0;

void setup() {
  pinMode(DC_FAN_MOTOR_INA, OUTPUT);
  pinMode(DC_FAN_MOTOR_INB, OUTPUT);  
  digitalWrite(DC_FAN_MOTOR_INA, 0);
  analogWrite(DC_FAN_MOTOR_INB, valFanMotorSpeed);
}

void loop() {
  for(int iPWM = 0; iPWM <= 255; iPWM++)  analogWrite(DC_FAN_MOTOR_INB, iPWM);
  for(int iPWM = 255; iPWM >= 0; iPWM--)  analogWrite(DC_FAN_MOTOR_INB, iPWM);
}
