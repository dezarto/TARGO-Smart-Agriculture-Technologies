#include <SoftwareSerial.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// RE and DE Pins set the RS485 module
// to Receiver or Transmitter mode
#define RE 8
#define DE 7

#define ONE_WIRE_BUS 3      // DS18B20 sensor pin
#define DHTPIN 2           // DHT sensor pin
#define DHTTYPE DHT11     // DHT11 type
#define GAS_MQ A2        // MQ135 gas sensor
#define WATER A0        // Water Rain sensor
#define SOIL_WATER A3  // Soil water sensor
#define PUMP 24       // PUMP

#define i2c_Address 0x3c // OLED ekran adresi

#define SCREEN_WIDTH 128 // OLED ekran genişliği, piksel cinsinden
#define SCREEN_HEIGHT 64 // OLED ekran yüksekliği, piksel cinsinden
#define OLED_RESET -1    // QT-PY / XIAO

// Modbus RTU requests for reading NPK values
const byte nitro[] = {0x01, 0x03, 0x00, 0x1E, 0x00, 0x01, 0xE4, 0x0C};
const byte phos[] = {0x01, 0x03, 0x00, 0x1F, 0x00, 0x01, 0xB5, 0xCC};
const byte pota[] = {0x01, 0x03, 0x00, 0x20, 0x00, 0x01, 0x85, 0xC0};

byte values[7]; // A variable used to store NPK values

String networkAddressName = "Wowa";                 // Network adress name    
String networkPassword = "deneme12345";             // Network password
String ipAddress = "184.106.153.149";             // Thingspeak ip address

float weatherTemperature, insideSoilTemperature, moistureSensorData, gasSensorData, waterRainSensorData, nitrogenData, phosphorousData, potassiumData, soilWaterSensorData;
int yPos = 0, sending = 0;
bool check = false, getDatasSW = true;

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial mod(12, 13);
OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire device
DallasTemperature sensors(&oneWire); // Pass oneWire reference to DallasTemperature library
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);  // Seri port communication started
  Serial.println("Started");

  /-------------- OLED Ekranı Başlat --------------/
  delay(250); // OLED'in açılması için bekleyin
  if (!display.begin(i2c_Address, true)) { // Adresi 0x3C olarak başlat
    Serial.println(F("OLED ekran bulunamadi"));
    while (1);
  }

  Serial.println(F("OLED ekran basariyla baslatildi"));

  display.clearDisplay();
  /*-------------- OLED Ekranı Bitiş-------------- */

  // Initialize ESP8266 using hardware serial
  Serial1.begin(115200);
  Serial1.println("AT");
  Serial.println("Sending AT command to check ESP8266");

  display.clearDisplay();
  display.setTextSize(1);             // text size '1'
  display.setTextColor(SH110X_WHITE); // text color
  display.setCursor(0, yPos);            // Started position
  yPos += 16;
  display.println(F("Sending AT command to check ESP8266"));
  display.display();

  while (!Serial1.find("OK")) {
    Serial1.println("AT");
    Serial.println("ESP8266 not found. Retrying...");
    display.setTextSize(1);            
    display.setTextColor(SH110X_WHITE); 
    display.setCursor(0, yPos);            
    display.println(F("ESP8266 not found. Retrying..."));
    display.display();
    yPos += 16; 
    if (yPos > SCREEN_HEIGHT - 10) {
      yPos = 0;
      display.clearDisplay();
    }
    delay(1000);
  }

  yPos = 0;
  display.clearDisplay();
  Serial.println("ESP8266 found and ready");
  display.setTextSize(1);             
  display.setTextColor(SH110X_WHITE); 
  display.setCursor(0, yPos);           
  display.println(F("ESP8266 found and ready"));
  display.display();
  yPos += 16;

  Serial1.println("AT+CWMODE=1");
  while (!Serial1.find("OK")) {
    Serial1.println("AT+CWMODE=1");
    Serial.println("Setting ESP8266 mode to client...");
    display.setTextSize(1);             
    display.setTextColor(SH110X_WHITE); 
    display.setCursor(0, yPos);            
    display.println(F("Setting ESP8266 mode to client..."));
    display.display();
    yPos += 16; 
    if (yPos > SCREEN_HEIGHT - 10) {
      yPos = 0;
      display.clearDisplay();
    }
    delay(1000);
  }
  Serial.println("ESP8266 set to client mode");
  
  yPos = 0;
  display.clearDisplay();

  display.setTextSize(1);             
  display.setTextColor(SH110X_WHITE); 
  display.setCursor(0, yPos);            
  display.println(F("ESP8266 set to client mode"));
  display.display();
  
  Serial.println("Connecting to Wi-Fi...");

  display.setTextSize(1);            
  display.setTextColor(SH110X_WHITE); 
  display.setCursor(0, yPos);   
  display.println(F("Connecting to Wi-Fi..."));
  display.display();
  yPos += 16; 

  Serial1.println("AT+CWJAP=\"" + networkAddressName + "\",\"" + networkPassword + "\"");
  while (!Serial1.find("OK")) {
    Serial.println("Failed to connect to Wi-Fi. Retrying...");
    display.setTextSize(1);             
    display.setTextColor(SH110X_WHITE); 
    display.setCursor(0, yPos);          
    display.println(F("Failed to connect to Wi-Fi. Retrying..."));
    display.display();
    yPos += 16;
    if (yPos > SCREEN_HEIGHT - 10) {
      yPos = 0;
      display.clearDisplay();
    }
    delay(2000);
  }

  Serial.println("Connected to Wi-Fi");
  yPos = 0;
  display.clearDisplay();

  display.setTextSize(1);             
  display.setTextColor(SH110X_WHITE); 
  display.setCursor(0, yPos);            
  display.println(F("Connected to Wi-Fi"));
  display.display();
  yPos += 16; 

  dht.begin();
  Serial.println("DHT sensor initialized.");

  display.setTextSize(1);        
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, yPos);           
  display.println(F("DHT sensor initialized."));
  display.display();
  yPos += 16;

  mod.begin(9600);
  pinMode(RE, OUTPUT);
  pinMode(DE, OUTPUT);
  Serial.println("NPK sensor initialized.");
  display.setTextSize(1);        
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, yPos);           
  display.println(F("NPK sensor initialized."));
  display.display();
  yPos += 16;

  sensors.begin();
  Serial.println("DS18B20 sensor initialized.");
  display.setTextSize(1);             
  display.setTextColor(SH110X_WHITE); 
  display.setCursor(0, yPos);           
  display.println(F("DS18B20 sensor initialized."));
  display.display();
  
  yPos = 0;
  display.clearDisplay();

  pinMode(SOIL_WATER, INPUT);
  Serial.println("SOIL_WATER sensor initialized.");
  display.setTextSize(1);             
  display.setTextColor(SH110X_WHITE); 
  display.setCursor(0, yPos);           
  display.println(F("SOIL_WATER sensor initialized."));
  display.display();
  yPos += 16; 

  // pinMode(PUMP, OUTPUT);
  // Serial.println("PUMP sensor initialized.");
  // digitalWrite(PUMP, HIGH);
  // display.setTextSize(1);             
  // display.setTextColor(SH110X_WHITE); 
  // display.setCursor(0, yPos);           
  // display.println(F("PUMP sensor initialized."));
  // display.display();
  // yPos += 16;

  Serial.println("All sensors initialized.");
  
  display.setTextSize(1);             
  display.setTextColor(SH110X_WHITE); 
  display.setCursor(0, yPos);            
  display.println(F("All sensors initialized."));
  display.display();

  delay(2000);
  display.clearDisplay();
}

void loop() {
  Serial1.println("AT+CIPSTART=\"TCP\",\"" + ipAddress + "\",80");
  if (Serial1.find("Error")) {
    Serial.println("Failed to start TCP connection");
    delay(2000);
    return;
  }

  if(getDatasSW == false){
    Serial.print(getDatas()); 
    getDatasSW = true;
  }

  sensors.requestTemperatures();
  weatherTemperature = dht.readTemperature();
  moistureSensorData = dht.readHumidity();
  gasSensorData = analogRead(GAS_MQ);
  waterRainSensorData = analogRead(WATER);
  insideSoilTemperature = sensors.getTempCByIndex(0);
  soilWaterSensorData = analogRead(SOIL_WATER);

  nitrogenData = nitrogen();
  delay(250);
  phosphorousData = phosphorous();
  delay(250);
  potassiumData = potassium();
  delay(250);

  yPos = 0;
  display.setTextSize(1);             // Metin boyutu 1
  display.setTextColor(SH110X_WHITE); // Beyaz metin rengi
  display.setCursor(0, 0);            // Başlangıç pozisyonu
  display.print(F("Temp : "));
  display.println(weatherTemperature);
  display.print(F("ISoil: "));
  display.println(insideSoilTemperature);
  display.print(F("Mois : "));
  display.println(moistureSensorData);
  display.print(F("Rain : "));
  display.println(waterRainSensorData);
  display.print(F("WSoil: "));
  display.println(soilWaterSensorData);
  display.print(F("Gas  : "));
  display.println(gasSensorData);

  display.setCursor(80, 0);            
  display.print(F("N: "));
  display.println(nitrogenData);
  display.setCursor(80, 14); 
  display.print(F("P: "));
  display.println(phosphorousData);
  display.setCursor(80, 28); 
  display.print(F("K: "));
  display.println(potassiumData);

  Serial.println("--------------------------");
  Serial.print("Weather Temperature: ");
  Serial.print(weatherTemperature);
  Serial.println("C ");
  Serial.print("Inside Soil Temperature: ");
  Serial.print(insideSoilTemperature);
  Serial.println("C ");
  Serial.print("Moisture Sensor Data: ");
  Serial.println(moistureSensorData);
  Serial.print("Gas Sensor Data: ");
  Serial.println(gasSensorData);
  Serial.print("Water Rain Sensor Data: ");
  Serial.println(waterRainSensorData);
  Serial.print("Soil Water Sensor Data: ");
  Serial.println(soilWaterSensorData);
  Serial.println("--------------------------");
  Serial.print("Nitrogen: ");
  Serial.print(nitrogenData);
  Serial.println(" kg/ha");
  Serial.print("Phosphorous: ");
  Serial.print(phosphorousData);
  Serial.println(" kg/ha");
  Serial.print("Potassium: ");
  Serial.print(potassiumData);
  Serial.println(" kg/ha");

  String datas = "GET https://api.thingspeak.com/update?api_key=VOM7OMIE8X1V1F0M";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz.
  datas += "&field1=";
  datas += String(weatherTemperature);
  datas += "&field2=";
  datas += String(moistureSensorData);  
  datas += "&field3=";
  datas += String(soilWaterSensorData);
  datas += "&field4=";
  datas += String(waterRainSensorData); 
  datas += "&field5=";
  datas += String(insideSoilTemperature);
  datas += "&field6=";
  datas += String(nitrogenData);
  datas += "&field7=";
  datas += String(phosphorousData);
  datas += "&field8=";
  datas += String(potassiumData);
  datas += "\r\n\r\n"; 


  Serial1.print("AT+CIPSEND=");
  Serial1.println(datas.length() + 2);
  delay(2000);
  if (Serial1.find(">")) {
    Serial1.print(datas);
    Serial.println("Data sent to Thingspeak: " + datas);
    
    if(check){
      sending = 0;
    }
    else {
      sending = 1;
    }
    check = !check;

    display.setTextSize(1);            
    display.setTextColor(SH110X_WHITE); 
    display.setCursor(0, 55);  
    display.print(F("Send: "));
    display.println(sending);
    display.setCursor(64, 55);  
    display.print(F("Wifi"));
    display.display();
    display.clearDisplay();
    delay(1000);
  } else {
    Serial.println("ESP8266 not ready to send data");
  
    display.setTextSize(1);             
    display.setTextColor(SH110X_WHITE); 
    display.setCursor(20, 55);  
    display.print(F("Wifi problem..."));
    display.display();
    display.clearDisplay();
  }

  Serial.println("Closing connection");
  Serial1.println("AT+CIPCLOSE");

  if(soilWaterSensorData >= 700){
    digitalWrite(PUMP, LOW);
    //delay(2000);
  }

  delay(2000);
}

float nitrogen() {
  return readSensor(nitro);
}

float phosphorous() {
  return readSensor(phos);
}

float potassium() {
  return readSensor(pota);
}

float readSensor(const byte* command) {
  digitalWrite(DE, HIGH);
  digitalWrite(RE, HIGH);
  delay(10);

  mod.write(command, 8);

  digitalWrite(DE, LOW);
  digitalWrite(RE, LOW);
  delay(10);

  unsigned long startTime = millis();
  int index = 0;

  while (index < 7 && millis() - startTime < 1000) {
    if (mod.available()) {
      values[index++] = mod.read();
    }
  }

  if (index == 7) {
    for (int i = 0; i < 7; i++) {
      Serial.print(values[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
    return values[4] * 10.0;
  } else {
    Serial.println("Error: Incomplete response");
    return 0;
  }
}

String getDatas() {
  String rest = "AT+CIPSEND=90";
  rest += "\r\n";
  sendData(rest, 2000, 0); //Gönderilecek Karakter Sayısı. ( "AT+CIPSEND=90" )


  String hostt = "GET /channels/2576973/feeds.json?api_key=COGQ8EN5OM4Q0S67";
  hostt += "\r\n";
  hostt += "Host:api.thingspeak.com";
  hostt += "\r\n\r\n\r\n\r\n\r\n";
  String Altin = sendData(hostt, 2000, 1);  // GET request ( GET /apps/thinghttp/send_request?api_key=XXXXXXXXXXXXXXXX 
                                            //               Host: Host_server_name ) 
  
  
/********************** gelen verinin içinden sadece ilgili bölümü alıyoruz. ****************************/
 int baslangic=Altin.indexOf(':');
 Altin=Altin.substring(baslangic+1,baslangic+9);

  return (Altin);
}

String sendData(String command, const int timeout, boolean debug) {
  String response = "";
  Serial1.print(command);

  long int startTime = millis();
  while ((millis() - startTime) < timeout) {
    while (Serial1.available()) {
      char c = Serial1.read();
      response += c;
    }
  }

  if (debug) {
    Serial.println(response);
  }

  return response;
}