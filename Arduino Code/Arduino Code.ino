#include <Servo.h>
#include <SoftwareSerial.h>
#define RX 2
#define TX 3
String AP = "Redmi Note 10S";       // AP NAME
String PASS = "romanking1"; // AP PASSWORD
String API = "XL2G2XZKVRD4I5KJ";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
String total_slots = "field1";
String input_sensor = "field2";
String output_sensor = "field3";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
int no_cars=0;
SoftwareSerial esp8266(RX,TX); 


Servo myservo1, myservo2;
                // create servo object to control a servo
                // twelve servo objects can be created on most boards

int pos1 = 0;    // variable to store the servo position
int pos2 = 0;    // variable to store the servo position

//int pos1 = 5; //servo 1 current position
//int pos2 = 5; //servo 2 current position
int vel = 10; //servos' speed (100 -> 90º in 1 s)(1 -> 90º in 9 s)
int pos1min = 0; //servo 1 minimum position
int pos2min = 0; //servo 2 minimum position
int pos1max = 100; //servo 1 maximum position
int pos2max = 100; //servo 2 maximum position


#define DEBUG true                //To display Wi-Fi module messages on serail monitor


int strPos = 0;    //Starting Position      
int minPos = 0; //minimum position
int maxPos = 180; //max position

  
void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  myservo1.attach(9); //intput servo gate.
  myservo2.attach(10); //output servo gate.
  pinMode(6,INPUT); //output ir sensor.
  pinMode(7,INPUT); //input ir sensor.
  pinMode(12,OUTPUT); //binds to 6
  pinMode(13,OUTPUT); // binds to 7
 
  sendingData("AT+CIFSR\r\n",1000,DEBUG);          //Showing IP address on the serial monitor
  sendingData("AT+CIPMUX=1\r\n",1000,DEBUG);       //Allowing multiple connections
  sendingData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); //Starting the web server on port 80
  
}

void loop() {
 //valSensor = getSensorData();


 if(digitalRead(7)==LOW && no_cars<=4){
    Serial.print("Car is ready to enter the parking\n");
    no_cars++;
    valSensor=no_cars;
    while(sendData(total_slots)==0);
    delay(2000);
    digitalWrite(13,HIGH);
    
    if(pos1!=91){
      for (pos1 = 0; pos1 <= 90; pos1 += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
        myservo1.write(pos1);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
    }
    delay(5000);
    if(pos1!=-1){
      for (pos1 = 90; pos1 >= 0; pos1 -= 1) { // goes from 180 degrees to 0 degrees
        myservo1.write(pos1);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
    }
    digitalWrite(13,LOW);
  }
  else{
    digitalWrite(13,LOW);
    delay(2000);
    
    if(pos1!=-1){
      for (pos1 = 90; pos1 >= 0; pos1 -= 1) { // goes from 180 degrees to 0 degrees
        myservo1.write(pos1);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
    }
  }


  if(digitalRead(6)==LOW && no_cars>=1){
    Serial.print("Car is ready to leave the parking\n");
    no_cars--;
    valSensor=no_cars;
    while(sendData(total_slots)==0);
    delay(2000);
    digitalWrite(12,HIGH);
    
    if(pos2!=91){
      for (pos2 = 0; pos2 <= 90; pos2 += 1) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
        myservo2.write(pos2);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
    }
    delay(5000);
    if(pos2!=-1){
      for (pos2 = 90; pos2 >= 0; pos2 -= 1) { // goes from 180 degrees to 0 degrees
        myservo2.write(pos2);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
    }
    digitalWrite(12,LOW);
  }
  else{
    digitalWrite(12,LOW);
    //delay(2000);
    
    if(pos2!=-1){
      for (pos2 = 90; pos2 >= 0; pos2 -= 1) { // goes from 180 degrees to 0 degrees
        myservo2.write(pos2);              // tell servo to go to position in variable 'pos'
        delay(15);                       // waits 15ms for the servo to reach the position
      }
    }
    
  }

  if (esp8266.available())  //check if there is data available on ESP8266
  {
    if (esp8266.find("+IPD,")) //if there is a new command
    {
      String msg;
      esp8266.find("?"); //run cursor until command is found
      msg = esp8266.readStringUntil(' '); //read the message
      String command = msg.substring(0, 3); //command is informed in the first 3 characters. "sr1" = command to servo #1 and "sr2" = command to servo #2
      String valueStr = msg.substring(4);   //next 3 characters inform the desired angle
      int value = valueStr.toInt();         //convert to integer
      if (DEBUG) {
        Serial.println(command);
        Serial.println(value);
      }
      delay(100);
      
      //************
      // move servos
      //************
      
      //move servo1 to desired angle
      if(command == "sr1") {
         //limit input angle
         if (value >= pos1max) {
           value = pos1max;
         }
         if (value <= pos1min) {
           value = pos1min;
         }
         //s1.attach(SERV1); //attach servo
         while(pos1 != value) {
           if (pos1 > value) {
             pos1 -= 1;
             myservo1.write(pos1);
             delay(100/vel);
           }
           if (pos1 < value) {
             pos1 += 1;
             myservo1.write(pos1);
             delay(100/vel);
           }
         }
         //s1.detach(); //dettach
      }

      //move servo2 to desired angle
      if(command == "sr2") {
         //limit input angle
         if (value >= pos2max) {
           value = pos2max;
         }
         if (value <= pos2min) {
           value = pos2min;
         }
         //s2.attach(SERV2);
         while(pos2 != value) {
           if (pos2 > value) {
             pos2 -= 1;
             myservo2.write(pos2);
             delay(100/vel);
           }
           if (pos2 < value) {
             pos2 += 1;
             myservo2.write(pos2);
             delay(100/vel);
           }
         }   
         //s2.detach();     
      } 
    }
  }


  
}

int sendData(String field){
   String getData = "GET /update?api_key="+ API +"&"+ field +"="+String(valSensor);
   int x=sendCommand("AT+CIPMUX=1",5,"OK");
   delay(15);
   x&=sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
   delay(15);
   x&=sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
   delay(15);
   esp8266.println(getData);delay(1500);countTrueCommand++;
   delay(15);
   x&=sendCommand("AT+CIPCLOSE=0",5,"OK");
   delay(15);
   return x;
}

int sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }

  int x=1;
  if(found==false){
    x=0;
  }
  
  found = false;
  return x;
 }


String sendingData(String com, const int tOut, boolean debug)//function to send data
{
  String res = "";
  esp8266.print(com);
  long int time = millis();
  while ( (time + tOut) > millis())
  {
    while (esp8266.available())
    {
      char c = esp8266.read();
      res += c;
    }
  }
  if (debug)
  {
    Serial.print(res);
  }
  return res;
}
