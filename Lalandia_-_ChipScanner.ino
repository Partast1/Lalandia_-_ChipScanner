
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#include <Arduino.h>
//#include <ESP8266HTTPClient.h>

#include <MFRC522.h> 
#include <SPI.h> 
#define SS_PIN    21
#define RST_PIN   22
#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16

MFRC522::MIFARE_Key key;
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::StatusCode status;

//Block til læsning af data
  byte block = 1;
//API response
  String payload;

//WIfi information
const char* ssid     = "dd-wrt";
const char* password = "";
const char* host = "192.168.1.1";
const char* streamId   = "....................";
const char* privateKey = "....................";

//Json dokument til deserialization
StaticJsonDocument<100> doc;


void setup() {
   Serial.begin(115200);
    delay(10);

  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
    
  // Forbind til WIFI
   Serial.print("Forbinder tilnetværk:");
   Serial.println(ssid);
    WiFi.begin(ssid, password);
    //Bliver ved med at prøve at forbinde til netværket
while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        tryConnecting();
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    // 
  

}

void loop() {
  // put your main code here, to run repeatedly:
  CheckConnection();

  GetChip();

  

}

void CheckConnection(){
    if(WiFi.status() != WL_CONNECTED){
      tryConnecting();
    }
    delay(1000);
}

int value = 0;

void tryConnecting(){
    delay(5000);
    ++value;

    Serial.print("Forbinder til");
    Serial.println(host);

    // Bruger client til at oprette TCP forbindelse
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }

    // We now create a URI for the request
    String url = "/input/";
    url += streamId;
    url += "?private_key=";
    url += privateKey;
    url += "&value=";
    url += value;

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }

    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }

    Serial.println();
    Serial.println("closing connection");
    delay(1000);
}

void GetChip(){
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    delay(50);
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    delay(50);
    return;
  }
  
  String id = ScanChip();
  VerifyChip(id);


  
}

String ScanChip(){

  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  return content;
  Serial.println();
  delay(4000);
  }


//verificere chip ID
  void VerifyChip(String id){

   apicall(id);

    

  
    }



void apicall(String rfID){

 if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;

      rfID.replace(" ", "");
      rfID.toUpperCase();
Serial.println(rfID);
      

//      String serverPath =  "http://192.168.1.141:8080/Lalandia-Backend-1.0-SNAPSHOT/api/chips";

       String serverPath = "http://192.168.1.141:8080/Lalandia-Backend-1.0-SNAPSHOT/api/test/" + rfID;
Serial.println(serverPath);

      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());
      
      // Sender HTTP GET 
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
 
          http.getString();
          if(httpResponseCode == 200){
            Serial.print("Chip accepted ");
          }
            if(httpResponseCode == 400){
            Serial.print("Chip ikke accepted ");
          }
          

        }
 else {
        Serial.print("Fejl kode: ");
        Serial.println(httpResponseCode);
      }
  // Free resources
      http.end();
       delay(100);
      }
     
    
    }
   
