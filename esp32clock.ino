/*********
Rafael Brandão Bastos
Puc Minas - 2023
*********/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"
#include <DHT.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define selectButton 35
#define prevButton 32
#define nextButton 34
#define setButton 26
#define undoButton 25
#define buzzer 5
#define DHTPIN 4
#define DHT_SENSOR_PIN  19 // ESP32 pin GPIO21 connected to DHT11 sensor
#define DHT_SENSOR_TYPE DHT11
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

//NETWORK SETTINGS
const char* ssid     = "YOUR-WIFI-SSID";
const char* password = "YOUR-WIFI-PASSWORD";
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = -10800;

bool alarmLoop = false;
bool alarmTrigger = false;
int piezoController = 0;
int loadingWifi = 0;
int buttonHolold = 0;
int menu = 1;
int setAlert = false;
int setAux = 0;
int undoAlert = false;
int undoAux = 0;
int setAlarmMinuts = 0;
int setAlarmHours = 0;
int alarmMinuts = 0;
int alarmHours = 0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup() {
  Serial.begin(115200);
  dht_sensor.begin();
  pinMode(selectButton, INPUT);
  pinMode(prevButton, INPUT);
  pinMode(nextButton, INPUT);
  pinMode(setButton, INPUT);
  pinMode(undoButton, INPUT);
  pinMode(buzzer, OUTPUT);
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
  while (WiFi.status() != WL_CONNECTED || loadingWifi < 5 && loadingWifi < 10) {
    delay(500);
    display.print(".");
    display.display();
    loadingWifi++;
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
  struct tm {
   int tm_sec;         /* seconds,  range 0 to 59          */
   int tm_min = 1;         /* minutes, range 0 to 59           */
   int tm_hour;        /* hours, range 0 to 23             */
   int tm_mday;        /* day of the month, range 1 to 31  */
   int tm_mon;         /* month, range 0 to 11             */
   int tm_year;        /* The number of years since 1900   */
   int tm_wday;        /* day of the week, range 0 to 6    */
   int tm_yday;        /* day in the year, range 0 to 365  */
   int tm_isdst;       /* daylight saving time             */
  };
}

void loop() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  // check alarm variables
  /*Serial.println("");
  Serial.print("Hours ");
  Serial.print(timeinfo.tm_hour);
  Serial.print(" | ");
  Serial.print(timeinfo.tm_min);
  Serial.print(" Alarme: ");
  Serial.print(alarmHours);
  Serial.print(" | ");
  Serial.print(alarmMinuts);
  Serial.print(" AlarmTrigger: ");
  Serial.print(alarmTrigger);
  Serial.print(" AlarmLoop: ");
  Serial.print(alarmLoop);
  Serial.print(" piezoController: ");
  Serial.print(piezoController);
  Serial.print(" setAlert: ");
  Serial.print(setAlert);
  Serial.print(" setAux: ");
  Serial.print(setAux);*/
  if((alarmHours == timeinfo.tm_hour && alarmMinuts == timeinfo.tm_min) && alarmTrigger == true) {
    alarmLoop = true;
    alarmTrigger = false;
    alarmHours = 0;
    alarmMinuts = 0;
  }
  if (alarmLoop == true) {
    if(piezoController < 30 || (piezoController > 60 && piezoController < 90) || (piezoController > 120 && piezoController < 150)) {
      tone(buzzer, 1500);
    } else {
      tone(buzzer, 0);
    }
    piezoController = piezoController + 1;
    if(piezoController == 210) {
      piezoController = 0;
    }
  }
  if(digitalRead(undoButton) == HIGH && alarmLoop == true) {
    alarmLoop = false;
    piezoController = 0;
    tone(buzzer, 0);
  }
  if(digitalRead(selectButton) == HIGH) {
      if (menu < 3) {
        menu = menu + 1;
      } else {
        menu = 1;
      }
      display.setTextSize(2);
      display.setCursor(5, 10);
      display.println("loading");
      display.setTextSize(1);
      display.display();
      delay(1000);
      display.clearDisplay();
  }
  if(menu == 1) {
    printLocalTime(timeinfo);
  }
  if(menu == 2) {
    printWeather();
  }
  if(menu == 3) {
    printAlarm();
  }
}

//MAIN SCREEN
void printLocalTime(struct tm showtimeinfo)
{
  display.setTextSize(4);
  display.setCursor(7, 0);
  if(showtimeinfo.tm_hour < 10) {
    display.print("0");
    display.print(showtimeinfo.tm_hour);
    } else {
    display.print(showtimeinfo.tm_hour);
  }
  display.print(":");
  if(showtimeinfo.tm_min < 10) {
    display.print("0");
    display.println(showtimeinfo.tm_min);
    } else {
    display.println(showtimeinfo.tm_min);
  }
  display.setTextSize(2);
  display.setCursor(5, 35);
  if(showtimeinfo.tm_mday < 10) {
    display.print("0");
    display.print(showtimeinfo.tm_mday);
    } else {
    display.print(showtimeinfo.tm_mday);
  }
  display.print("/");
  if(1 + showtimeinfo.tm_mon < 10) {
    display.print("0");
    display.print(1 + showtimeinfo.tm_mon);
    } else {
    display.print(1 + showtimeinfo.tm_mon);
  }
  display.print("/");
  display.print(1900 + showtimeinfo.tm_year);
  display.setTextSize(1);
  display.setCursor(35, 55);
  display.print("Alarm ");
  if (alarmTrigger == false) {
    display.println("OFF");
  } else {
    if(alarmHours < 10) {
      display.print("0");
      display.print(alarmHours);
      } else {
      display.print(alarmHours);
    }
    display.print(":");
    if(alarmMinuts < 10) {
      display.print("0");
      display.print(alarmMinuts);
    } else {
      display.print(alarmMinuts);
    }
  }
  display.display();
  display.clearDisplay();
}

//ALARM SCREEN
void printAlarm() {
  display.setTextSize(2);
  display.setCursor(10, 5);
  display.print("Set Alarm");
  display.setTextSize(3);
  display.setCursor(20, 30);
  if(setAlarmHours < 10) {
    display.print("0");
    display.print(setAlarmHours);
  } else {
    display.print(setAlarmHours);
  }
  display.print(":");
  if(setAlarmMinuts < 10) {
    display.print("0");
    display.print(setAlarmMinuts);
  } else {
    display.print(setAlarmMinuts);
  }
  if(digitalRead(nextButton) == HIGH) {
    if(buttonHolold < 5) {
      delay(300);
    }
    buttonHolold = buttonHolold + 1;
    if(setAlarmHours < 23 || setAlarmMinuts < 59) {
      if(setAlarmMinuts < 59) {
        setAlarmMinuts = setAlarmMinuts + 1;
      } else {
        setAlarmMinuts = 0;
        setAlarmHours = setAlarmHours + 1;
      }
    } else {
      setAlarmHours = 0;
      setAlarmMinuts = 0;
    }
  } else if (digitalRead(prevButton) == LOW) {
    buttonHolold = 0;
  }
  if(digitalRead(prevButton) == HIGH) {
    if(buttonHolold < 5) {
      delay(300);
    }
    buttonHolold = buttonHolold + 1;
    if(setAlarmHours > 0 || setAlarmMinuts > 0) {
      if(setAlarmMinuts > 1) {
        setAlarmMinuts = setAlarmMinuts - 1;
      } else if (setAlarmHours > 0) {
        setAlarmMinuts = 60;
        setAlarmHours = setAlarmHours - 1;
      } else {
        setAlarmHours = 0;
        setAlarmMinuts = 0;
      }
    } else {
      setAlarmHours = 23;
      setAlarmMinuts = 59;
    }
  } else if(digitalRead(nextButton) == LOW) {
    buttonHolold = 0;
  }
  if(digitalRead(setButton) == HIGH) {
    undoAlert = false;
    alarmTrigger = true;
    setAlert = true;
    alarmHours = setAlarmHours;
    alarmMinuts = setAlarmMinuts;
  }
  if(setAlert == true && setAux < 50) {
    display.setTextSize(1);
    display.setCursor(10, 56);
    display.print("ALARM SET TO ");
    if(setAlarmHours < 10) {
    display.print("0");
    display.print(setAlarmHours);
  } else {
    display.print(setAlarmHours);
  }
  display.print(":");
  if(setAlarmMinuts < 10) {
    display.print("0");
    display.print(setAlarmMinuts);
  } else {
    display.print(setAlarmMinuts);
  }
    Serial.print("SET");
    setAux = setAux + 1;
  } else {
    setAlert = false;
    setAux = 0;
  }
  if(digitalRead(undoButton) == HIGH) {
    setAlert = false;
    Serial.print("UNDO");
    alarmTrigger = false;
    undoAlert = true;
    alarmHours = 0;
    alarmMinuts = 0;
  }
  if(undoAlert == true && undoAux < 50) {
    display.setTextSize(1);
    display.setCursor(25, 56);
    display.print("ALARM DELETED");
    Serial.print("UNDO");
    undoAux = undoAux + 1;
  } else {
    undoAlert = false;
    undoAux = 0;
  }
  display.setTextSize(1);
  display.display();
  display.clearDisplay();
}

//WEATHER SCREEN
void printWeather()
{
 // read humidity
  int humi  = trunc(dht_sensor.readHumidity());
  // read temperature in Celsius
  int tempC = trunc(dht_sensor.readTemperature());
  // read temperature in Fahrenheit
  int tempF = trunc(dht_sensor.readTemperature(true));
  // check whether the reading is successful or not
  /*if ( isnan(tempC) || isnan(tempF) || isnan(humi)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");
    Serial.print("  |  ");
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print("°C  ~  ");
    Serial.print(tempF);
    Serial.println("°F");
  }*/
  display.setTextSize(2);
  display.setCursor(5, 10);
  display.print("Temp:");
  display.print(tempC);
  display.println("C");
  display.setCursor(5, 40);
  display.print("Hum:");
  display.print(humi);
  display.println("%");
  display.setTextSize(1);
  display.display();
  display.clearDisplay();
}

