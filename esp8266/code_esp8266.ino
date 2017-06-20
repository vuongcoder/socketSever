#include <SoftwareSerial.h>


#include <ESP8266WiFi.h>
#include <SocketIOClient.h>
//#include <SoftwareSerial.h>
#include <SerialCommand.h>


// include thu vien dekiem tra free Ram tren con esp8266
extern "C"{
  #include "user_interface.h"
}

const byte RX = 0;  //GPIO0
const byte TX = 2;  //GPIO2
//SoftwareSerial mySerial = SoftwareSerial(RX, TX, false, 256);
SoftwareSerial mySerial = SoftwareSerial(RX, TX);
SerialCommand sCmd(mySerial);
SocketIOClient client;

// tham so cho ket noi wifi
const char* ssid= "arduino_Qn";
const char* password = "security@@#";
char host[] = "nhimxusmarthome.herokuapp.com";
int port = 80;

char namespace_esp8266[]="esp8266";
extern String RID;
extern String Rfull;
//Một số biến dùng cho việc tạo một task
unsigned long previousMillis = 0;
long interval = 5000;
int valueAnalog=1234;

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600);
  delay(10);
 
  Serial.println("Connect to wifi  name: ");
 
  
  Serial.println(ssid);
  /*
  WiFi.begin(ssid, password);
  */
  while(WiFi.status()!= WL_CONNECTED){
    delay(500);
    Serial.print('.');
  }
  
    Serial.println();
    Serial.println(F("Da ket noi WiFi"));
    Serial.println(F("Di chi IP cua ESP8266 (Socket Client ESP8266): "));
    Serial.println(WiFi.localIP());
    delay(1000);
    if(!client.connect(host, port, namespace_esp8266)) {
        Serial.println(F("Ket noi den socket server that bai!"));
        return;
    }
    if(client.connected()){
        Serial.println(F("Ket noi den socket server thanh cong!"));
    }

    sCmd.addDefaultHandler(defaultCommand);
    Serial.println("Redy command !!!");
}

void loop() {
  sCmd.readSerial();
  
 
  // +RID: Ten su kien
  // +Rfull: Danh sach tham so duoc nen thanh chuoi JSON! 
  if(client.monitor()){
    
    // gui qua arduino de xu ly
    mySerial.println(RID);
    //mySerial.println('\r');
    mySerial.println(Rfull);
    //mySerial.println('\r');

    Serial.println(RID);
    Serial.println(Rfull);
    
    uint32_t free = system_get_free_heap_size();
    Serial.print("RAM Free: ");
    Serial.println(free);
  }
  if(!client.connected()){
    client.reconnect(host, port, namespace_esp8266);
    Serial.println(F("Ket noi den socket server that bai!"));
    delay(1000);
  }
  if (millis() - previousMillis > interval) {
        //lệnh:
        previousMillis = millis();
        client.send("JsName", "Temp: ", "Timer please");
        //gửi sự kiện "atime" là một JSON chứa tham số message có nội dung là Time please?
        //client.send("atime", "message", "Time please?");
        //Serial.println(F("-"));
        client.send("JsAnalog", "Value:", (String)valueAnalog );
   }
}

/**********************************
 * 
*********************************/
void defaultCommand(String command){
  
  char *json = sCmd.next();
  client.send(command, (String)json); // gui du lie cho socket server
  
  ///in ra serial monition de debug
  Serial.print(command);
  Serial.print(' ');
  Serial.println(json);
  Serial.print("da gui qua socket server hi!");
}

