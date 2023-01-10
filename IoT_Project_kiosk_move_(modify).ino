#include <Servo.h>
Servo servo;

#define trig 7
#define echo 6
int angle = 0;

void setup() {
  // put your setup code here, to run once:
  servo.attach(2);
  Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  long duration, distance;

  digitalWrite(trig, LOW);
  digitalWrite(echo, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  duration = pulseIn(echo, HIGH);
  distance = ((float)(340 * duration) / 10000) / 2;

  Serial.print("거리: ");
  Serial.print(distance);
  Serial.println("cm");

  if (distance == 2){
    Serial.println(" is entering");
    
    for(; angle < 150; angle++){
    	servo.write(angle);
    	delay(20);
    } }
    	
  else if (distance == 3){
    Serial.println(" is entering");
    
    for(; angle < 120; angle++){
    	servo.write(angle);
    	delay(20);
    	}}      
    	
  else if (distance == 4){
    Serial.println(" is entering");
    
    for(; angle < 90; angle++){
    	servo.write(angle);
    	delay(20);
    	}} 
    	
  else if (distance == 5){
    Serial.println(" is entering");
    
    for(; angle < 60; angle++){
    	servo.write(angle);
    	delay(20);
    	} }
    	
  else if (distance > 10){
  	angle = 0;
  	servo.write(angle);
    delay(20);
}}
