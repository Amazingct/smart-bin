#include <TinyGPS++.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "WiFi.h"
#include <SoftwareSerial.h>
#include <Servo.h>
#include <FirebaseESP32.h>
#define FIREBASE_HOST "finalbin-35e3c-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "6qYcuchjFTNvvB4dyCg7qqxOc3l0tXQUvMW9Vqr1"
String path = "bins/";
String bin_id = "Bin001";
//Define FirebaseESP32 data object
FirebaseData firebaseData;
FirebaseJson json;
Servo myservo;  // create servo object to control a servo
int open_bin = 0;  
int close_bin = 180;
// Create a TinyGPS++ object
TinyGPSPlus gps;
SoftwareSerial ss(17, 16); // RX | TX
const char* ssid = "Amazing";
const char* password = "ogunlolu1";
int lcdColumns = 16;
int lcdRows = 2;
const int trigPin = 5;
const int echoPin = 22;
// defines variables
long duration;
int distance, level;
int ir = 21;
float lat, lng;
String latitude = "0";
String longitude = "0";
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  


//void IRAM_ATTR isr() 
void open_lid()
{
  Serial.println("OPen Bin");
  myservo.write(open_bin);
  delay(3000);
  myservo.write(close_bin);
  Serial.println("close Bin");
}
void printResult(FirebaseData &data)
{

  if (data.dataType() == "int")
    Serial.println(data.intData());
  else if (data.dataType() == "float")
    Serial.println(data.floatData(), 5);
  else if (data.dataType() == "double")
    printf("%.9lf\n", data.doubleData());
  else if (data.dataType() == "boolean")
    Serial.println(data.boolData() == 1 ? "true" : "false");
  else if (data.dataType() == "string")
    Serial.println(data.stringData());
  else if (data.dataType() == "json")
  {
    Serial.println();
    FirebaseJson &json = data.jsonObject();
    //Print all object data
    Serial.println("Pretty printed JSON data:");
    String jsonStr;
    json.toString(jsonStr, true);
    Serial.println(jsonStr);
    Serial.println();
    Serial.println("Iterate JSON data:");
    Serial.println();
    size_t len = json.iteratorBegin();
    String key, value = "";
    int type = 0;
    for (size_t i = 0; i < len; i++)
    {
      json.iteratorGet(i, type, key, value);
      Serial.print(i);
      Serial.print(", ");
      Serial.print("Type: ");
      Serial.print(type == FirebaseJson::JSON_OBJECT ? "object" : "array");
      if (type == FirebaseJson::JSON_OBJECT)
      {
        Serial.print(", Key: ");
        Serial.print(key);
      }
      Serial.print(", Value: ");
      Serial.println(value);
    }
    json.iteratorEnd();
  }
  else if (data.dataType() == "array")
  {
    Serial.println();
    //get array data from FirebaseData using FirebaseJsonArray object
    FirebaseJsonArray &arr = data.jsonArray();
    //Print all array values
    Serial.println("Pretty printed Array:");
    String arrStr;
    arr.toString(arrStr, true);
    Serial.println(arrStr);
    Serial.println();
    Serial.println("Iterate array values:");
    Serial.println();
    for (size_t i = 0; i < arr.size(); i++)
    {
      Serial.print(i);
      Serial.print(", Value: ");

      FirebaseJsonData &jsonData = data.jsonData();
      //Get the result data from FirebaseJsonArray object
      arr.get(jsonData, i);
      if (jsonData.typeNum == FirebaseJson::JSON_BOOL)
        Serial.println(jsonData.boolValue ? "true" : "false");
      else if (jsonData.typeNum == FirebaseJson::JSON_INT)
        Serial.println(jsonData.intValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_FLOAT)
        Serial.println(jsonData.floatValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_DOUBLE)
        printf("%.9lf\n", jsonData.doubleValue);
      else if (jsonData.typeNum == FirebaseJson::JSON_STRING ||
               jsonData.typeNum == FirebaseJson::JSON_NULL ||
               jsonData.typeNum == FirebaseJson::JSON_OBJECT ||
               jsonData.typeNum == FirebaseJson::JSON_ARRAY)
        Serial.println(jsonData.stringValue);
    }
  }
  else if (data.dataType() == "blob")
  {

    Serial.println();

    for (int i = 0; i < data.blobData().size(); i++)
    {
      if (i > 0 && i % 16 == 0)
        Serial.println();

      if (i < 16)
        Serial.print("0");

      Serial.print(data.blobData()[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  else if (data.dataType() == "file")
  {

    Serial.println();

    File file = data.fileStream();
    int i = 0;

    while (file.available())
    {
      if (i > 0 && i % 16 == 0)
        Serial.println();

      int v = file.read();

      if (v < 16)
        Serial.print("0");

      Serial.print(v, HEX);
      Serial.print(" ");
      i++;
    }
    Serial.println();
    file.close();
  }
  else
  {
    Serial.println(data.payload());
  }
}

void display_info(String info)
{
  Serial.println(info);
  lcd.setCursor(0,1);
  lcd.print("              ");
  lcd.setCursor(0,1);
  lcd.print(info);
}

void post(int level, String gps)
{
  json.set("Level", level);
  json.set("gps", gps);

  if (Firebase.updateNode(firebaseData, path + bin_id, json))
  {
    Serial.println("PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
 
    Serial.print("VALUE: ");
    printResult(firebaseData);
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
}

void setup()
{
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");
  Serial.println(open_bin);
  Serial.println(close_bin);
  myservo.attach(18);
  myservo.write(close_bin);
  delay(1000);
  pinMode(ir, INPUT);
  //attachInterrupt(ir, isr, FALLING);
  Wire.begin(33, 32);
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  lcd.init();                     
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("   SMART BIN   ");
  lcd.setCursor(0,1);
  lcd.print("loading....");

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
    lcd.setCursor(0,1);
    lcd.print("connecting wifi..");
  }
 
  lcd.setCursor(0,1);
  Serial.println("WiFi connnected..");
  lcd.print("wifi connected...");
  delay(1000);
  ss.begin(9600);
}


int get_level()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  distance = duration*0.034/2;
  if (distance > 100){distance = 1;}
  else if (distance < 100 && distance > 15){distance = 2;}
  else if (distance < 15){distance = 3;}
  return distance;
}

void loop()
{
  level = get_level();
  display_info("LEVEL:" + String(level));
  printFloat(gps.location.lat(), gps.location.isValid(), 6, 1);
  printFloat(gps.location.lng(), gps.location.isValid(), 6, 0);
  smartDelay(1000);
  post(level, latitude + "," + longitude);
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

static void printFloat(float val, bool valid, int prec, int a)
{
  if (!valid)
  {
  Serial.println("no sats");
  }
  else
  {
    Serial.print(val, prec);
  }
  smartDelay(0);
  if (a ==  1) latitude = String(val, prec);
  if (a ==  0) longitude = String(val, prec);
  
  
}
