//#include < Wire .h> we are removing this because it is already added in liquid crystal library
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
// Create the lcd object address 0x3F and 16 columns x 2 rows
LiquidCrystal_I2C lcd(0x3F, 16,2);  //
#define LIGHT_SENSOR_PIN 32 // ESP32 pin GIOP36 (ADC0)
//SDA pin -> 21
//SCL pin -> 22
void setup () {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 1);
  lcd.print("");
  delay(2000);
}
void  loop () {
    //Here cursor is placed on first position (col: 0) of the second line (row: 1)
  int light = analogRead(LIGHT_SENSOR_PIN);
  lcd.setCursor(0, 1);
  lcd.print("00:00:00");

  if(light>200) {
    lcd.setCursor(0, 1);
    lcd.print(light);
  }
  else{
    lcd.setCursor(0, 1);
    lcd.print("time");
  }
  delay(1000);
  lcd.clear();
}