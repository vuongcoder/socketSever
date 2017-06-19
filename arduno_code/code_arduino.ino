

#include <ArduinoJson.h>
#include <SerialCommand.h>
#include <SoftwareSerial.h>

#include <OneWire.h>
#include <DallasTemperature.h>

////////////////////////////////////////////////////////
const byte RX = 10;
const byte TX = 11;

SoftwareSerial mySerial = SoftwareSerial(RX, TX); //rx tx
SerialCommand sCmd(mySerial); // Khai báo biến sử dụng thư viện Serial Command
#define ONE_WIRE_BUS 52
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
int red = 13, blue = 12; // led đỏ đối vô digital 4, led xanh đối vô digital 5
int VQ;
int PinACS = A8;

const unsigned long Chu_Ky_Gui = 5000UL; //sau 2000ms = 2s
///////////////////////////////////////////////////////////
void setup() {
 
  Serial.begin(115200);
  
  mySerial.begin(9600);
  //pinMode 2 đèn LED là OUTPUT
  pinMode(red,OUTPUT);
  pinMode(blue,OUTPUT);
  //pinMode(PinACS, INPUT)
  sCmd.addCommand("LED", led);
  sCmd.addCommand("SENSORS", readSensors);
  
  Serial.println("Arduino Say: Da san sang nhan lenh");
  VQ = vAcs(PinACS); 
  sensors.begin();
}


///////////////////////vong lap chinh cua chuong trinh/////////////////////////////////
unsigned long chuky1= 0;
int redStatus;
int blueStatus;
void loop() {

 sCmd.readSerial();
 if(millis() - chuky1 > Chu_Ky_Gui){
      chuky1= millis();
      readSensors();
 }
    
}
///////////////////void Led///////////////////////////////////////
void led(){

    Serial.println("");
    Serial.print("LED ");
    
    char *json = sCmd.next(); // dong de doc tham so nhan duoc
    Serial.println(json);
    
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(json); //dat bien root nang kieu json
    
    redStatus = root["led"][0];
    blueStatus = root["led"][1];
    
    StaticJsonBuffer<200> jsonBuffer3;
    JsonObject& root3 = jsonBuffer3.createObject();
    root3["redStatus"] = redStatus;
    root3["blueStatus"] = blueStatus;
    //tao mot mang trong JSON
    JsonArray& data = root3.createNestedArray("data");
    data.add(redStatus);
    data.add(blueStatus);
    // gui qua esp8266
    mySerial.print("LED_STATUS"); // gui ten lenh
    mySerial.print('\r');
    root3.printTo(mySerial);
    mySerial.print('\r');
    ////////////////// xong ////////////////
    
    digitalWrite(red, redStatus);
    digitalWrite(blue, blueStatus);
    // kiem thu gia tri
    Serial.print(F("redStatus "));
    Serial.println(redStatus);
    Serial.print(F("blueStatus "));
    Serial.println(blueStatus);
    
}
/////////////////////////////////////////////////////

void readSensors(){
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0); // vi co 1 ic nen la 0
    int mV = map(vAcs(PinACS), 0, 1023, 0, 5000);
    float mA = iAcs(PinACS);
    
    StaticJsonBuffer<200> jsonBuffer2;
    JsonObject& root2 = jsonBuffer2.createObject();
    //root2["SENSORS"] = "Sensors: Current - Temperature"; // chuoi kieu string tinh
    root2["mV"] = mV;// kieu int dong 
    root2["mA"] = mA; //kieu so thuc
    root2["Temperature"] = temperature;
    root2["Message"] = temperature ? "Hot" : "Cool";
   
    
    

   
    //gui qua esp8266 nhan xu ly va gui den server
    mySerial.print("SENSORS"); //gui ten lenh
    mySerial.print('\r');
    root2.printTo(mySerial); //gui chuoi json
    mySerial.print('\r');
    // in ra debug
    //root2.printTo(Serial);
     
}
/////////////////// cac chuong trinh con//////////////
int vAcs(int PIN){
  long VQ = 0;
  for(int i =0; i <5000; i ++){
    VQ += analogRead(PIN);
    delay(1);
  }
  VQ /= 5000;
  Serial.print(map(VQ, 0, 1023, 0, 5000));
  Serial.println("mV");
  return VQ;
}
float iAcs(int PIN){
  int current = 0;
  int sensitivity = 66.0;
  // doc 5 lan de lay may i
  for(int i = 0; i < 5; i++){
    current += analogRead(PIN) - VQ;
    delay(1);
  }
  current = map(current/5, 0, 1023, 0, 5000);
  return float (current)/ sensitivity;
}

