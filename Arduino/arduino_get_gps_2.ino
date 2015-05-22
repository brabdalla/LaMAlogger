

#include <SPI.h>
#include <WiFi.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#include "TinyGPS++.h"
 

char ssid[] = "lamawifi";  // wireless network name
char pass[] = "lamalog2015"; // wireless password

int sensorId = 1;
int status = WL_IDLE_STATUS;

// Initialize the Wifi client library
WiFiClient client;

// server address:
char server[] = "192.168.0.15";


unsigned long lastConnectionTime = 0;            // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds

float temperatura, umidade;
double latitude, longitude, altura;

#define DHTPIN 2 // dht pin
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);


static const int RXPin = 6, TXPin = 5;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  ss.begin(GPSBaud);
  dht.begin();
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

 // check shield firmware
  String fv = WiFi.firmwareVersion();
  if ( fv != "1.1.0" )
    Serial.println("Please upgrade the firmware");

  // attempt to connect to Wifi network:
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  // you're connected now
  printWifiStatus();
  dht.begin();
}

void loop() {
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

}

// this method makes a HTTP connection to the server:
void httpRequest() {
  
  //Serial.println(freeRam ());
  lerDht(); //read sensor
  lerGPS(); // read GPS
   
  client.stop(); // close connection.

  // if there's a successful connection:
  //Serial.println(freeRam ());
  if (client.connect(server, 80)) {
    //Serial.println(freeRam ());
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print("GET /insert_mysql.php?temp="); //url
    client.print(temperatura);
    client.print("&umid="); //name variable
    client.print(umidade); 
    client.print("&lat="); //name variable
    //Serial.println(latitude,6);
    client.print(latitude,6);
    client.print("&lon="); //name variable
    client.print(longitude,6);
    client.print("&alt="); //name variable
    client.print(altura);
    client.print("&id="); //name variable
    client.print(sensorId);
    client.println(" HTTP/1.1");
    client.println("Host: 192.168.0.1"); // ip server
    client.println("User-Agent: Arduino");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  }
  else {
    //Serial.println(freeRam ());
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void lerDht() {
  
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();
  if (isnan(temperatura) || isnan(umidade)) {
    Serial.println("Failed to read from DHT");
  } else {
//    Serial.print("Humidity: "); 
//    Serial.print(umidade);
//    Serial.print(" %\t");
//    Serial.print("Temperature: "); 
//    Serial.print(temperatura);
//    Serial.println(" *C");
  }


}

void lerGPS() {

  //Serial.println(gps.location.lat());
  //Serial.println(gps.location.lng(),6);
 //Serial.println(gps.altitude.meters(), 2);
  latitude = gps.location.lat();
  longitude = gps.location.lng();
  altura = gps.altitude.meters();
  
  smartDelay(1000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

// this function is for check the memory
int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


