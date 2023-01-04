#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3C, 16, 2);
LiquidCrystal_I2C lcd2(0x3E, 16, 2);
LiquidCrystal_I2C lcd3(0x3F, 16, 2);
#define LIGHT_SENSOR_PIN 35 // ESP32 pin GIOP36 (ADC0)
#define LIGHT_SENSOR_PIN2 34
#define LIGHT_SENSOR_PIN3 36

void setup(){
  Serial.begin(4800);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN2, INPUT);
  pinMode(LIGHT_SENSOR_PIN3, INPUT);
  lcd.begin();
  lcd2.begin();
  lcd3.begin();
  lcd.backlight();
  lcd2.backlight();
  lcd3.backlight();
  lcd.print("Serial");
  lcd2.print("Serial2");
  lcd3.print("Serial3");
}

void loop(){
  int analogValue = analogRead(LIGHT_SENSOR_PIN);
  int analogValue2 = analogRead(LIGHT_SENSOR_PIN2);
  int analogValue3 = analogRead(LIGHT_SENSOR_PIN3);
  
  Serial.print("Analog Value = ");
  Serial.println(analogValue);
  Serial.print("Analog Value2 = ");
  Serial.println(analogValue2);
  Serial.print("Analog Value3 = ");
  Serial.println(analogValue3);

  if (analogValue > 700){
    lcd.print("carNum")
  }
  else {
    lcd.print("Time")
  }

  

  
  
  delay(500);

  lcd.clear();

}
