#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <time.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <stdio.h>
#include <string.h>

LiquidCrystal_I2C lcd(0x3C, 16, 2);
LiquidCrystal_I2C lcd2(0x3E, 16, 2);
LiquidCrystal_I2C lcd3(0x3F, 16, 2);
LiquidCrystal_I2C lcd_arr[3] = {lcd, lcd2, lcd3};

const int LIGHT_SENSOR_PIN = 35;
const int LIGHT_SENSOR_PIN2 = 34;
const int LIGHT_SENSOR_PIN3 = 36;

const int MAX_SSIZE = 15;
const int park_pin[3] = {LIGHT_SENSOR_PIN, LIGHT_SENSOR_PIN2, LIGHT_SENSOR_PIN3};
String park_spot[3] = {"a1b1", "a2b1", "a3b1"};
String plate_num[3] = {"-", "-", "-"};
String sTime[3] = {"-", "-", "-"};
int status[3] = {0, 0, 0}; // 0: empty, 1: moving, 2: fill
int brightness[3] = {1500, 500, 1500};


//wifi 로그인
const char* ssid       = "PinkLab";
const char* password   = "pinkwink";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// SQL flags
bool flag[3] = {0,0,0};
// bool flag1 = 0;
// bool flag2 = 0;
// bool flag3 = 0;

// variables for pySerial
String incomingByte;
String locID, tStart, pNum;

void setup(){
  Serial.begin(9600);
  pinMode(LIGHT_SENSOR_PIN, INPUT);
  pinMode(LIGHT_SENSOR_PIN2, INPUT);
  pinMode(LIGHT_SENSOR_PIN3, INPUT);

  for(int i=0; i<3; i++){
    lcd_arr[i].begin();
    lcd_arr[i].backlight();
  }

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  timeClient.begin();
  timeClient.setTimeOffset(32400);  // 한국은 GMT+9이므로 9*3600=32400
}

int get_carIdx(String id_loc) {
  if(id_loc == "a1b1") return 0;
  else if(id_loc == "a2b1") return 1;
  else if(id_loc == "a3b1") return 2;
  else return -1;
}

void car_exit_handling(int idx) {
  // update status & reset info
  status[idx] = 0;
  sTime[idx] = "-";
  plate_num[idx] = "-";
  // 시리얼 통신으로 데이터 보내기
  String serial_data = "publishData " + park_spot[idx];
  Serial.println(serial_data);
}

void lcdprint (int carIdx, String timeStamp) {
  int val = analogRead(park_pin[carIdx]);
  Serial.print(park_spot[carIdx]);
  Serial.print(":   ");
  Serial.println(val);
  if (val > brightness[carIdx]){
    if(status[carIdx] == 2) { // parked (2) -> car out (0) --> 출차
      car_exit_handling(carIdx);
      lcd_arr[carIdx].write(254); 
    }
    else if(status[carIdx] == 0) {
      lcd_arr[carIdx].write(254);
    }
    else if (status[carIdx] == 1) { // car moving
      lcd_arr[carIdx].print(plate_num[carIdx]);
    }
  }
  else if (val <= brightness[carIdx] & status[carIdx] > 0){ // sensored car parked
    if(status[carIdx] == 1){ // was moving (1) --> update to parked (2)
      status[carIdx] = 2; // update status
    }
    lcd_arr[carIdx].print(getTimeDifference(sTime[carIdx], timeStamp));
  }
}

String getTimeDifference(String start_time, String timeStamp) {
  // Split start_time into hours, minutes, and seconds
  int start_hours = start_time.substring(0, 2).toInt();
  int start_minutes = start_time.substring(3, 5).toInt();
  int start_seconds = start_time.substring(6, 8).toInt();
  Serial.print("sTime: ");
  Serial.println(start_time);
  Serial.print("  && TimeNOW: ");
  Serial.println(timeStamp);
  // Split timeStamp into hours, minutes, and seconds
  int hours = timeStamp.substring(0, 2).toInt();
  int minutes = timeStamp.substring(3, 5).toInt();
  int seconds = timeStamp.substring(6, 8).toInt();
  // Convert start time and timeStamp into total seconds
  int start_total_seconds = start_hours * 3600 + start_minutes * 60 + start_seconds;
  int total_seconds = hours * 3600 + minutes * 60 + seconds;
  // Subtract start time from timeStamp to get time difference in seconds
  int time_difference_seconds = total_seconds - start_total_seconds;
  // Convert time difference in seconds back into hours, minutes, and seconds
  int time_difference_hours = time_difference_seconds / 3600;
  time_difference_seconds -= time_difference_hours * 3600;
  int time_difference_minutes = time_difference_seconds / 60;
  time_difference_seconds -= time_difference_minutes * 60;
  // Return time difference as a formatted string
  return String(time_difference_hours) + ":" + String(time_difference_minutes) + ":" + String(time_difference_seconds);
}

void update_SQL(){
  int analogValue = analogRead(park_pin[0]);
  int analogValue2 = analogRead(park_pin[1]);
  int analogValue3 = analogRead(park_pin[2]);
  for(int i=0; i<3; i++){
    analogValue = analogRead(park_pin[i]);
    if (analogValue < brightness[i] & flag[i] ==0){
      flag[i] = 1;
      Serial.println("P_ON!_" + park_spot[i]);
    }
    if (analogValue > brightness[i] & flag[i] ==1){
      flag[i] = 0;
      Serial.println("P_OFF_" + park_spot[i]);
    }
  }
}

void pyserial_callback(String id_loc, String start, String plateNum) {
  int idx = get_carIdx(id_loc);
  status[idx] = 1;
  plate_num[idx] = plateNum;
  sTime[idx] = start;
}

void split_str(String input) {
  locID = incomingByte.substring(0, 4);
  tStart = incomingByte.substring(5, 13);
  pNum = incomingByte.substring(14, 20);
}

void loop(){
  // if get pyserial input, call pyserial_callback with data given 
  if(Serial.available() > 0) {
    incomingByte = Serial.readString();
    split_str(incomingByte);
    pyserial_callback(locID, tStart, pNum);
  }
  
  // 시간 추출 (ex) 2018-11-12T16:00:13Z
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  String formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  String timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);
  
  // ldc update
  lcdprint(0, timeStamp);
  lcdprint(1, timeStamp);
  lcdprint(2, timeStamp);
  delay(2000);
  lcd_arr[0].clear();
  lcd_arr[1].clear();
  lcd_arr[2].clear();
  update_SQL();
}