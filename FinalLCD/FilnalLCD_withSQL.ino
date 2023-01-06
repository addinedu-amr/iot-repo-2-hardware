#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <time.h> 
#include <WiFiUdp.h>
#include <NTPClient.h>

LiquidCrystal_I2C lcd(0x3C, 16, 2);
LiquidCrystal_I2C lcd2(0x3E, 16, 2);
LiquidCrystal_I2C lcd3(0x3F, 16, 2);
LiquidCrystal_I2C lcd_arr[3] = {lcd, lcd2, lcd3};

const int LIGHT_SENSOR_PIN = 35;
const int LIGHT_SENSOR_PIN2 = 34;
const int LIGHT_SENSOR_PIN3 = 36;

const int MAX_SSIZE = 15;
int park_pin[3] = {LIGHT_SENSOR_PIN, LIGHT_SENSOR_PIN2, LIGHT_SENSOR_PIN3};
char park_spot[3][MAX_SSIZE] = {{"a1b1"}, {"a2b1"}, {"a3b1"}};
int status[3] = {1, 1, 0}; // 0: empty, 1: moving, 2: fill
char plate_num[3][MAX_SSIZE] = {{"38FOK2"}, {"AB"}, {"CA"}};
char sTime[3][MAX_SSIZE] = {{"13:09:28"}, {"09:10:50"}, {"-"}};

//wifi 로그인
const char* ssid       = "PinkLab"; 
const char* password   = "pinkwink"; 

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// SQL flags
bool flag1 = 0;
bool flag2 = 0;
bool flag3 = 0;

void setup(){
  Serial.begin(4800);
  
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

int get_carIdx(char id_loc) {
  if(id_loc == 'a1b1') return 0;
  else if(id_loc == 'a2b1') return 1;
  else if(id_loc == 'a3b1') return 2;
  else return -1;
}

void lcdprint (int carIdx, String timeStamp) {
  int val = analogRead(park_pin[carIdx]);
  if (val > 300){
    if(status[carIdx] == 2) { // parked (2) -> car out (0)
      status[carIdx] = 0; // update status
      lcd_arr[carIdx].write(254);
    }
    else if(status[carIdx] == 0) {
      lcd_arr[carIdx].write(254);
    }
    else if (status[carIdx] == 1) { // car moving
      lcd_arr[carIdx].print(plate_num[carIdx]);
    }
  }
  else if (val <= 300 & status[carIdx] > 0){ // sensored car parked
    if(status[carIdx] == 1){ // was moving (1) --> update to parked (2)
      status[carIdx] = 2; // update status
    }
    lcd_arr[carIdx].print(getTimeDifference(sTime[carIdx], timeStamp));      
  }
}

// ********** 함수 만들어야해 *****************
// status 0 --> 1 pyserial 받아올때 update
// status 2 --> 0 (출차) pyserial publish 

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
  
  if (analogValue < 300 & flag1 ==0){
    flag1 = 1;
    Serial.println("P_ON!_a1b1");
  } 
  if (analogValue > 300 & flag1 ==1){
    flag1 = 0;
    Serial.println("P_OFF_a1b1");
  }
  
  if (analogValue2 < 300 & flag2 ==0){
    flag2 = 1;
    Serial.println("P_ON!_a2b1");
  } 
  if (analogValue2 > 300 & flag2 ==1){
    flag2 = 0;
    Serial.println("P_OFF_a2b1");
  }

  if (analogValue3 < 300 & flag3 ==0){
    flag3 = 1;
    Serial.println("P_ON!_a3b1");
  } 
  if (analogValue3 > 300 & flag3 ==1){
    flag3 = 0;
    Serial.println("P_OFF_a3b1");
  }  
}

// input from pyserial
char input = 'a2b1';

void loop(){
  update_SQL();  

  // 시간 추출 (ex) 2018-11-12T16:00:13Z
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  String formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  String timeStamp = formattedDate.substring(splitT+1, formattedDate.length()-1);

  // if get input from pyserial, update info[get_carIdx[input]] ()
  
  // ldc update
  lcdprint(0, timeStamp); 
  lcdprint(1, timeStamp); 
  lcdprint(2, timeStamp); 

  delay(2000);

  lcd_arr[0].clear();
  lcd_arr[1].clear();
  lcd_arr[2].clear();

}
