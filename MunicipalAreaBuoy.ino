#include <SoftwareSerial.h>

String boat_Code;          
int boat_number = 1;       //BOAT NUMBER THAT IT IS COMMUNICATING TO. AKIN TO THE PHARMACY NUMBERING SYSTEM
String received;
String buoyCode ="";       //EACH BUOY WILL HAVE THEIR OWN CODE USED TO IDENTIFY THEM

//PIN CONFIGURATIONS
const byte HC12RxdPin = 4;
const byte HC12TxdPin = 5;
const byte HC12SetPin = 6;

unsigned long time_now=0;
bool isConnected=false;
bool toSend = false;

//CONNECTS TO THE LED STRIP (RGB)
//const byte RedLed = 2;    //THIS IS USED IF THE BUOY IS IN A MARINE PROTECTED AREA
const byte GreenLed = 2;    //THIS IS USED IF BUOY IS IN MUNICIPAL WATERS


SoftwareSerial HC_12(HC12TxdPin,HC12RxdPin); 
SoftwareSerial mySerial(9, 10);   //RX,TX  
void setup() {
  pinMode(GreenLed, OUTPUT);
  digitalWrite(GreenLed,HIGH);
  pinMode(HC12SetPin,OUTPUT);      // Output High for Transparent / Low for Command
  digitalWrite(HC12SetPin,HIGH);
  delay(80);
  Serial.begin(9600); 
  HC_12.begin(9600);
  mySerial.begin(9600);   // Setting the baud rate of GSM Module 
}

void loop() {
  //Buoy moves to the next boat number if not connected
  if (!isConnected){
    if(millis() - time_now >= 10000){
        time_now = millis();
        boat_number++;
        boat_number%=1000;    //MAXIMUM BOAT IDS THAT MAY BE REGISTERED IN THE BUOY
    }         
  }
  
  //BUOY SENDS THE BOAT NUMBER THAT IT IS TRYING TO COMMUNICATE TO
  HC_12.println(boat_number);
  String num_toSend = String(boat_number);
  
  if (HC_12.available() > 1){
    received = HC_12.readString();
  }  
  if (received == "OK"){         //IF THE RIGHT BOAT ID IS CONNECTED, IT WILL SEND AN "OK"
      isConnected = true;        //Prevents the buoy from moving to the next number even if there's slight disconnections
      HC_12.println("AT+C002");    //Switches the boat ID to channel 2 
      delay(100);
      digitalWrite(HC12SetPin,LOW);   //Changes the HC-12 to command mode
      delay(100);
      HC_12.println("AT+C002");
      delay(200);
      if(HC_12.available() >1){
      Serial.print(HC_12.readString());
      }
      digitalWrite(HC12SetPin,HIGH);  //Changes the HC-12 back to transparent mode
      if(HC_12.available() > 1){
        boat_Code = HC_12.readString();
        received = "";
        toSend = true;
        HC_12.println("CLEAR");
        delay(200);
        HC_12.println("AT+C001");    //Switches the boat ID back to channel 1 
        delay(100);
        digitalWrite(HC12SetPin,LOW);   //Changes the HC-12 to Transparent Mode
        delay(100);
        HC_12.println("AT+C001");
        delay(200);
        isConnected = false;
        boat_number++;                  //Buoy moves to the next boat number
        digitalWrite(HC12SetPin,HIGH);  //Changes the HC-12 back to transparent mode
      }
    }
  
  //Buoy sends the boat ID's unique code to the server/database
  if (toSend){
    sendSMS_Municipal(); 
  }
  
  delay(20);
}

void sendSMS_Municipal(){
  mySerial.println("AT+CMGF=1");    //Sets Text Mode
  delay(1000);  // Delay 1 second
  mySerial.println("AT+CMGS=\"+63xxxxxxxxxx\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("Alert! Alert! A registered boat is in municipal Area Area");// The SMS 
  delay(100);
  mySerial.println("AT+CMGS=\"+63xxxxxxxxxx\"\r"); // Replace x with server SMS number
  delay(1000);
  mySerial.println(boat_Code);// Sends the boat code 
  delay(100);
  mySerial.println(buoyCode);// Sends the buoy code 
  delay(100);
  
  // ASCII code of CTRL+Z (exit code ??)
   mySerial.println((char)26);
  delay(1000);
  boat_Code="";
  toSend=false;
  
}
