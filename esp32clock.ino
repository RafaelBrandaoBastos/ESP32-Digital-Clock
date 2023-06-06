/*********
Rafael Brandão Bastos
Aluno Puc Minas - 2023
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//NETWORK SETTINGS
const char* ssid     = "2GClaroAp202";
const char* password = "vlmrap202";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = -10800;

bool hourLoop = false;
bool weatherLoop = false;
bool alarmLoop = false;
bool alarmTrigger = false;
int aux1 = 0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);

  //SETING DISPLAY
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.print("Hello, world!");
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Clock By:");
  display.println("Rafael Brandao Bastos");
  display.display();
  delay(2000);
  display.clearDisplay();



  // Connect to Wi-Fi
  display.setCursor(0, 10);
  display.println("Connecting to: ");
  display.display();
  delay(1000);
  display.println(ssid);
  display.display();
  delay(1000);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED || aux1 < 5 && aux1 < 10) {
    delay(500);
    display.print(".");
    display.display();
    aux1++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    display.println("");
    display.println("WiFi connected.");
  } else {
    display.println("");
    display.println("WiFi ERROR.");
  }
  display.display();
  delay(500);
  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  display.clearDisplay();

  hourLoop = true;
}

void loop() {
  if(hourLoop == true) {
    printLocalTime();
  }
}


void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  display.setTextSize(4);
  display.setCursor(5, 10);
  display.println(&timeinfo, "%H:%M");
  display.setTextSize(1);
  display.println(&timeinfo, "%A, %d %B %Y");
  display.setCursor(35, 55);
  display.print("Alarm ");
  if (alarmTrigger == false) {
    display.println("OFF");
  } else {
    display.println("ON");
  }
  display.display();
  Serial.println("time");
  Serial.println(&timeinfo, "%A %B %d %Y %H:%M:%S");
  delay(60000);
  display.clearDisplay();
}