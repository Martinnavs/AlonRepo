#include <SoftwareSerial.h>

//PIN CONFIGURATIONS
const byte HC12RxdPin = 4;
const byte HC12TxdPin = 5;
const byte HC12SetPin = 6;
const byte redLed = 2;
const byte yellowLed = 3;
const byte greenLed = 13;

const String boat_number = "1" ; //Predetermined according to the number of boat IDs created before this
const String boat_code = "RxY81Zxo"; //Random and uniquely generated code per boat

String received;
String received2;
String readBuffer;
bool isClear = false;
unsigned long previousTime;
SoftwareSerial HC_12(HC12TxdPin,HC12RxdPin); 

void setup() {
  pinMode(redLed,OUTPUT);
  pinMode(yellowLed,OUTPUT);
  pinMode(greenLed,OUTPUT);
  pinMode(HC12SetPin,OUTPUT);
  digitalWrite(HC12SetPin,HIGH);
  HC_12.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(HC_12.available() > 1){
      received = HC_12.readString();
  }
  
  if (received == boat_number){     
    yellowON();
    HC_12.println("OK");
    delay(100);
    if (HC_12.available()>1){
      readBuffer = HC_12.readString();
    }
    digitalWrite(HC12SetPin,LOW);
    delay(100);
    HC_12.println(readBuffer);
    delay(200);
    if(HC_12.available() >1){
      Serial.print(HC_12.readString());
    }
    digitalWrite(HC12SetPin,HIGH);   //Changes HC-12 to transparent mode
    
    HC_12.println(boat_code);       //BOAT ID SENDS BOAT CODE TO BUOY TO BE SENT TO ADMIN
    delay(100);
    //RETURNS BOAT ID BACK TO CHANNEL 1
    if (HC_12.available()>1){
      received2 = HC_12.readString();
    }
    if (HC_12.available()>1){
      readBuffer = HC_12.readString();
    }
    digitalWrite(HC12SetPin,LOW);   //Changes HC-12 to command mode
    delay(100);
    HC_12.println(readBuffer);      //Switches the buoy back to Channel 1
    delay(200);
    if(HC_12.available() >1){
      Serial.print(HC_12.readString());
    }
    digitalWrite(HC12SetPin,HIGH);
    isClear = true;
  }

  if (received2=="CLEAR"){
    greenON();          //BOAT HAS BEEN MARKED OK FOR FISHING IN MUNICIPAL AREA
    previousTime=millis();
  } else  if(received2 == "WARNING" || received2 ==""){   //BOAT IS IN A PROTECTED AREA
    redON();
  }

  if (millis()-previousTime > 1800000){
    received2="";
  }
  delay(20);
}

void redON(){
  digitalWrite(redLed,HIGH);
  digitalWrite(yellowLed,LOW);
  digitalWrite(greenLed,LOW);
}

void greenON() {
  digitalWrite(redLed,LOW);
  digitalWrite(yellowLed,LOW);
  digitalWrite(greenLed,HIGH);
}

void yellowON() {
  digitalWrite(redLed,LOW);
  digitalWrite(yellowLed,HIGH);
  digitalWrite(greenLed,LOW);
}
