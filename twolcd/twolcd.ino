#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3C, 16, 2);
LiquidCrystal_I2C lcd2(0x3E, 16, 2);
LiquidCrystal_I2C lcd3(0x3F, 16, 2);

void setup(){
  Serial.begin(9660);
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

}