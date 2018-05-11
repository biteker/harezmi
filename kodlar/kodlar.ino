#include "DHT.h"
#define DHTPIN 2     // what digital pin we're connected to
#define isikPin A0
#define sesPin A1
#define direkNemPin A2
#define karbonPin A3
#define alevPin 3
#define egimPin 4
#define toprakNemPin 12
#define rolePin 13

#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

#define DEBUG true
int connectionId;

void setup() {
  pinMode(alevPin, INPUT);
  pinMode(egimPin, INPUT);
  pinMode(toprakNemPin, INPUT);
  pinMode(rolePin,OUTPUT);

  dht.begin();

  Serial.begin(115200);    ///////For Serial monitor 
  Serial3.begin(115200); ///////ESP Baud rate
 
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=2\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  sendData("AT+CIPSERVER=1,80\r\n",1000,DEBUG); // turn on server on port 80
}


void loop() {
  // Wait a few seconds between measurements.
 /* delay(2000);*/
  sicaklik();
  isik();
  ses();
  alev();
  toprakNem();
  egim();
  direkNem();
  karbon();
  Serial.println("-----------------------------------------------------------------");

  if(Serial3.available())
  {
    /////////////////////Recieving from web browser to toggle led
    if(Serial3.find("+IPD,"))
    {
     delay(300);
     connectionId = Serial3.read()-48;
     if(Serial3.find("pin="))
     { 
     Serial.println("recieving data from web browser");
     int pinNumber = (Serial3.read()-48)*10; 
     pinNumber += (Serial3.read()-48); 
     digitalWrite(pinNumber, !digitalRead(pinNumber));
     }
   
    /////////////////////Sending data to browser
    else
    {
      String webpage = "<h1>Hello World</h1>";
      espsend(webpage);
     }

     
     String closeCommand = "AT+CIPCLOSE=";  ////////////////close the socket connection////esp command 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     sendData(closeCommand,3000,DEBUG);
    }
  }
  
}
void karbon() {
  int karbonDeger=analogRead(karbonPin);
  Serial.print("Karbonmonoksit Değeri: ");
  Serial.println(karbonDeger);
}
void direkNem() {
  int direkNemDeger=analogRead(direkNemPin);
  Serial.print("Toprak Nem Değeri: ");
  Serial.println(direkNemDeger);
}
void egim() {
  int egimDeger=digitalRead(egimPin);
  if (egimDeger) { 
    Serial.println("Direk Devrildi!!!!");
  }
  else {
    Serial.println("Direk Düzgün.");
  }
}

void sicaklik() {

  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Nem: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Sıcaklık: ");
  Serial.print(t);
  Serial.print(" *C\t");
  Serial.print("Çiğ Oluşma Noktası: ");
  Serial.print(hic);
  Serial.println(" *C ");
}
void toprakNem() {
  int toprakNemDegeri=digitalRead(toprakNemPin);
  if(toprakNemDegeri==0) {
    digitalWrite(rolePin, HIGH);
  }
  else {
    digitalWrite(rolePin,LOW);
  }
}

void isik() {
  int isikDeger=analogRead(isikPin);
  Serial.print("Işık Degeri: ");
  Serial.println(isikDeger);
}
void ses() {
  int sesDeger=analogRead(sesPin);
  Serial.print("Ses Degeri: ");
  Serial.println(sesDeger);
}
void alev() {
  int alevDeger=digitalRead(alevPin);
  if (alevDeger) { 
    Serial.println("Yangın VAR!!!!");
  }
  else {
    Serial.println("Yangın yok.");
  } 
}

 
  //////////////////////////////sends data from ESP to webpage///////////////////////////
 
 void espsend(String d)
         {
             String cipSend = " AT+CIPSEND=";
             cipSend += connectionId; 
             cipSend += ",";
             cipSend +=d.length();
             cipSend +="\r\n";
             sendData(cipSend,1000,DEBUG);
             sendData(d,1000,DEBUG); 
         }

//////////////gets the data from esp and displays in serial monitor///////////////////////         
String sendData(String command, const int timeout, boolean debug)
            {
                String response = "";
                Serial3.print(command);
                long int time = millis();
                while( (time+timeout) > millis())
                {
                   while(Serial3.available())
                      {
                         char c = Serial3.read(); // read the next character.
                         response+=c;
                      }  
                }
                
                if(debug)
                     {
                     Serial.print(response); //displays the esp response messages in arduino Serial monitor
                     }
                return response;
            }
