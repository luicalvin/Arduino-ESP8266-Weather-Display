#include <Adafruit_GFX.h>    // Core graphics library by Adafruit
#include <Arduino_ST7789.h> // Hardware-specific library for ST7789 (with or without CS pin)
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <SPI.h>
#include <time.h>
#include<TimeLib.h>

#define TFT_DC    D1
#define TFT_RST   D0
#define TFT_MOSI  D7   // for hardware SPI data pin (all of available pins)
#define TFT_SCLK  D5   // for hardware SPI sclk pin (all of available pins)

//You can use different type of hardware initialization
Arduino_ST7789 tft = Arduino_ST7789(TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK); //for display without CS pin

int timezone = 5;
const char* ssid     = "";      // SSID of local network
const char* password = "";   // Password on network
String APIKEY = ""; //api weather key here
String CityID = "6066513"; //Markham
WiFiClient client;
char servername[] = "api.openweathermap.org"; // remote server we will connect to
String result = "";
int  counter = 60;
String weatherDescription = "";
String weatherLocation = "";
String Country;
float Temperature;
float Humidity;
float Pressure;
float Windspeed;
//int rotation=2;


void setup() {
  Serial.begin(115200);
  //TIME
  configTime(-5 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  
  tft.init(240, 240);   // initialize a ST7789 chip, 240x240 pixels
  //void setRotation(uint8_t rotation);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  Serial.println("Connected to wifi \n");
  tft.fillScreen(BLACK);
  tft.setTextSize(3);
  tft.setTextColor(GREEN);
  tft.println("Wifi \n connected!");
  tft.setTextColor(WHITE);
  tft.println(ssid);
  delay(1500);
}

void loop() {
  time_t now = time(nullptr);
  //Serial.println("Time:");
  //Serial.println(hourFormat12());
  Serial.println(ctime(&now));
  delay(1500);
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setTextSize(4);
  tft.setCursor(0, 0);
  tft.println(ctime(&now));
  delay(1000);
  if (counter >= 30 ) //Get new data every 10 minutes (was 60)
  {
    counter = 0;
    displayGettingData();
    delay(1000);
    getWeatherData();
    delay(1000);
    Serial.println("Got new data");
  } else{
    counter++;
    displayWeather(weatherLocation, weatherDescription);
    delay(5000);
    displayConditions(Temperature, Humidity, Pressure, Windspeed);
    delay(5000);
    Serial.println(counter);
  }
}

void getWeatherData() //client function to send/receive GET request data.
{
  result = "";
  Serial.println("reset results");
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println("GET /data/2.5/weather?id=" + CityID + "&units=metric&APPID=" + APIKEY);
    client.println("Host: api.openweathermap.org");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  }
  else {
    Serial.println("connection failed"); //error message if no client connect
    tft.println("failed connection");
//    Serial.println();
  }

  while (client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    result = result + c;
  }

  client.stop(); //stop client
  result.replace('[', ' ');
  result.replace(']', ' ');
  Serial.println(result);

  char jsonArray [result.length() + 1];
  result.toCharArray(jsonArray, sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';

  StaticJsonBuffer<1024> json_buf;
  JsonObject &root = json_buf.parseObject(jsonArray);
  if(!root.success())
  {
    Serial.println("parseObject() failed");
  }
  String location = root["name"];
  String country = root["sys"]["country"];
  float temperature = root["main"]["temp"];
  float humidity = root["main"]["humidity"];
  String weather = root["weather"]["main"];
  String description = root["weather"]["description"];
  float pressure = root["main"]["pressure"];
  float wind = root["wind"]["speed"];

  weatherDescription = description;
  weatherLocation = location;
  Country = country;
  Temperature = temperature;
  Humidity = humidity;
  Pressure = pressure;
  Windspeed = wind;
  Serial.println("The new temp is");
  Serial.println(Temperature);
}

void displayWeather(String location, String description)
{
  tft.fillScreen(BLACK);
  tft.setTextSize(4);
  tft.setCursor(0, 0);
  tft.print(location);
  tft.print(", \n");
  tft.println(Country);
  tft.setCursor(0, 60);
  tft.setTextSize(6);
  
  if (description == ("overcast clouds")) {
    tft.setTextColor(GRAY);
  }else if(description ==("mist")){
    tft.setTextColor(BLUE);
  }else {
    tft.setTextColor(WHITE);
  }
  tft.println(description);
}

void displayConditions(float Temperature, float Humidity, float Pressure, float Windspeed)
{
  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);
  tft.setCursor(0, 0);
  tft.setTextSize(3);
  tft.print("Temperature:");
  tft.setTextSize(5);
  tft.setCursor(0, 35);
  if(Temperature >= 5){
    tft.setTextColor(RED);
  } else if (Temperature < 5 && Temperature > 0) {
    tft.setTextColor(WHITE);
  } else if (Temperature <= 0){
    tft.setTextColor(BLUE);
  } else{
    tft.setTextColor(CYAN);
  }
  tft.print(Temperature, 1);
  tft.setTextSize(3);
  tft.print((char)247);
  tft.print("C \n");
  tft.setTextColor(WHITE);

//printlning Humidity
  tft.setCursor(0, 80);
   tft.setTextSize(3);
  tft.print("Humidity:");
  
  if(Humidity >= 75){
    tft.setTextColor(GREEN);
  } else if (Humidity <= 75 && Humidity > 40) {
    tft.setTextColor(WHITE);
  } else{
    tft.setTextColor(WHITE);
  }
    tft.setCursor(0, 110);
   tft.setTextSize(5);
  tft.print(Humidity, 0);
    tft.setTextSize(3);
  tft.print(" %\n ");
  tft.setTextColor(WHITE);

//printlning Windspeed
  tft.setCursor(0, 160);
   tft.setTextSize(3);
  tft.print("Wind: ");
  if (Windspeed >= 15) {
    tft.setTextColor(RED);
  } else if (Windspeed < 15 && Windspeed >= 5) {
    tft.setTextColor(WHITE);
  } else {
    tft.setTextColor(GREEN);
  }
  tft.setCursor(0, 190);
     tft.setTextSize(5);
  tft.print(Windspeed, 1);
     tft.setTextSize(3);
  tft.print(" km/h");
}

void displayGettingData()
{
   tft.setCursor(0, 0);
  tft.fillScreen(BLACK);
  tft.println("Getting data...");
  delay(1000);
}


