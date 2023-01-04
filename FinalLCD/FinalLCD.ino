#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <time.h> 
#include <WiFiUdp.h>
#include <NTPClient.h>

LiquidCrystal_I2C lcd(0x3C, 16, 2);
LiquidCrystal_I2C lcd2(0x3E, 16, 2);
LiquidCrystal_I2C lcd3(0x3F, 16, 2); //lcd 할당

#define LIGHT_SENSOR_PIN 35 // ESP32 pin GIOP36 (ADC0)
#define LIGHT_SENSOR_PIN2 34
#define LIGHT_SENSOR_PIN3 36

const char* ssid       = "PinkLab"; 
const char* password   = "pinkwink"; //wifi 로그인

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Variables to save date and time
String formattedDate;
String dayStamp;
String timeStamp;

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

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();    // NTP 클라이언트 초기화
  // 자신의 timezone에 맞게 초 단위로 time offset을 설정해준다. 예를 들어
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(32400);  // 한국은 GMT+9이므로 9*3600=32400
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

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // formattedDate 은 다음과 같은 형태임
  // 2018-11-12T16:00:13Z
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // 날짜 추출
  int splitT = formattedDate.indexOf("T");
  dayStamp = formattedDate.substring(0, splitT);
  Serial.print("DATE: ");
  Serial.println(dayStamp);
  // 시간 추출
  timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.print("HOUR: ");
  Serial.println(timeStamp);

  if (analogValue > 500){
    lcd.print("13A2C1");
  }
  else {
    lcd.print(timeStamp);
  }
   delay(2000);

  lcd.clear();

}
