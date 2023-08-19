// LED, 팬, 온도, 습도 정보 가져오기 // new blynk 2022년 5월 1일  
// 고정 wifi주소를 입력하는 방식

// 각자 자신의 정보를 입력해야 한다. 아래 3줄
#define BLYNK_TEMPLATE_ID "TMPLfEfczu3T"
#define BLYNK_DEVICE_NAME "SmartAirPurifier"
#define BLYNK_AUTH_TOKEN "CA3fRW9Y6iwHfdraMVYdjxCVLcdUlFc6"

#define BLYNK_PRINT Serial

#define BLYNK_FIRMWARE_VERSION         "0.1.0"

#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <time.h> 
#include <DHT.h>
#include <GP2YDustSensor.h>


char auth[] = BLYNK_AUTH_TOKEN;

// 여기에 와이파이 정보를 넣는다. 2.4GHZ로 5GHz는 안됨
char ssid[] = "Shinbg";
char pass[] = "*bingo0712";

BlynkTimer timer;

int FAN_A = D2;
int FAN_B = D3;// 선풍기 돌리기
int fan_speed  = 1;

int set_led = 0;
int brightness = 0;
int led_color = 0;

int humid = 0;
int temperature = 0;
double dust = 0;

int smart_mode = 0;
int sw = 0;

int init_first = 10;

const uint8_t SHARP_LED_PIN = D8;   // Sharp Dust/particle sensor Led Pin
const uint8_t SHARP_VO_PIN = A0;    // Sharp Dust/particle analog out pin used for reading 

unsigned long previousMillis = 0;
const long interval = 1000; 

int dd = 0;       // 구조체 내 해당 값 가져오기 
int MM = 0;
int yy = 0;
int ss = 0;
int mm = 0;
int hh = 0;

int hour=0;
int minuate=0; 
int sec=0;
int hour2=0;
int minuate2=0; 
int sec2=0;
boolean state=false;


#define LED_PIN D7      // 네오픽셀 D핀과 연결한 아두이노 핀 번호
#define LED_COUNT 38  // 네오픽셀 LED 개수
#define BRIGHTNESS 50  // 네오픽셀 LED 밝기(0 ~ 255) *RGBW만

#define DHTPIN D6          // 온습도 센서가 D6로 연결
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
GP2YDustSensor dustSensor(GP2YDustSensorType::GP2Y1014AU0F, SHARP_LED_PIN, SHARP_VO_PIN);
GP2YDustSensor setBaseline(float zeroDustVoltage);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

enum RGB_Color
{
  RED,
  GREEN,
  BLUE,
  CYAN,
  MAGENTA,
  YELLOW,
  WHITE,
  OFF
};

void setup()
{
   Serial.begin(115200);
   delay(100);
   Blynk.begin(auth, ssid, pass);
   configTime(32400, 0, "pool.ntp.org", "time.nist.gov");
   Serial.println("\nWaiting for time"); 
   while (!time(nullptr)) { 
     Serial.print("."); 
     delay(1000); 
   }
   Serial.println(""); 
   
   dustSensor.setBaseline(0.7);
   dustSensor.begin();

   dht.begin();
   
   strip.begin();                    // 네오픽셀 초기화(필수)
   strip.show(); 
   //strip.setBrightness(BRIGHTNESS);  // 네오픽셀 밝기 설정 *RGBW만
   rgb_LED(0,OFF, 0);
   rgb_LED(1,OFF, 0);

   pinMode(FAN_A, OUTPUT);
   pinMode(FAN_B, OUTPUT);
   
   digitalWrite(FAN_A, LOW); 
   digitalWrite(FAN_B, HIGH);
   delay(1000);
   digitalWrite(FAN_B, LOW);
   
   
   timer.setInterval(1000L, sendSensor);   
   Blynk.config(auth);
   Blynk.syncVirtual(V0);
   Blynk.syncVirtual(V1);
   Blynk.syncVirtual(V2);
   Blynk.syncVirtual(V3);
   Blynk.syncVirtual(V4);   
   Blynk.syncVirtual(V8);   
   Blynk.syncVirtual(V15);   
}

void loop() {
  Blynk.run();
  timer.run();
  onoff(sw);
  if (sw==1){
    humid = dht.readHumidity();
    temperature = dht.readTemperature();
    float realtime_dust = dustSensor.getDustDensity();
    dust = dustSensor.getRunningAverage();
    Serial.print("Humid: ");
    Serial.println(humid);
    Serial.println("--------------------------");
    Serial.print("Temperature: ");
    Serial.println(temperature);
    Serial.println("--------------------------");
    Serial.print("Dust density: ");
    Serial.println(dust);
    dust = dustCalibration(humid,temperature,dust);
    Serial.print("Dust Calibration: ");
    Serial.println(dust);
    Serial.println("--------------------------");
    if (smart_mode==0){
    SmartAction(temperature,dust);
    }
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    time_t now;
    struct tm * timeinfo;
    time(&now);
    timeinfo = localtime(&now);
    //Serial.println(ctime(&now)); 
    dd = timeinfo->tm_mday;       // 구조체 내 해당 값 가져오기 
    MM = timeinfo->tm_mon + 1; 
    yy = timeinfo->tm_year +1900; 
    ss = timeinfo->tm_sec; 
    mm = timeinfo->tm_min; 
    hh = timeinfo->tm_hour;
    Serial.print(yy); Serial.print(". "); 
    Serial.print(MM); Serial.print(". "); 
    Serial.print(dd); Serial.print(" "); 
    Serial.print(hh); Serial.print(": "); 
    Serial.print(mm); Serial.print(": "); 
    Serial.println(ss); 
  if (state==1){
   onCompareTime();
   offCompareTime();
  }
  }
  if (init_first>0){
    state = 0;
    init_first--;
  }
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
  Blynk.syncVirtual(V2);
  Blynk.syncVirtual(V3);
  Blynk.syncVirtual(V4);
  Blynk.syncVirtual(V8);
  Blynk.syncVirtual(V15);
} 


int dustCalibration(int humid, int temperature, double dust){
  float t = 0;
  if (dust>200){
    dust = 200;
  }
  if (temperature > 25){
    t = dust - dust*0.8875;
    if (humid <= 30){
      return dust+t;
    }
    else if (humid > 30 and humid < 70){
      return dust;
    }
    else{
      return dust-t;
    }  
  }
  else{
    t = dust - dust*0.8625;
    if (humid <= 30){
      return dust+t;
    }
    else if (humid > 30 and humid < 70){
      return dust;
    }
    else{
      return dust-t;
    }
  }
}

void onoff(int sw){
  if(sw==0){
   //strip.setBrightness(BRIGHTNESS);  // 네오픽셀 밝기 설정 *RGBW만
   rgb_LED(0,OFF, 0);
   rgb_LED(1,OFF, 0);
   digitalWrite(FAN_A, LOW); 
   digitalWrite(FAN_B, LOW);
  }
}

void onCompareTime(){
  if(hh==hour){
    if(mm==minuate){
      if(ss==sec){
        Serial.println("trigger");
        sw=1;
        smart_mode=0;
      }
    }
  }
}

void offCompareTime(){
  if(hh==hour2){
    if(mm==minuate2){
      if(ss==sec2){
        Serial.println("trigger");
        sw=0;
        smart_mode=1;
        state=false;
      }
    }
  }
}

void SmartAction(int temperature,double dust){
  int t = temperature*0.1;
  if (dust<=30){
    if (temperature<25){
      rgb_LED(0, CYAN, 30);
      analogWrite(FAN_B, 0);
    }
    else {
      rgb_LED(0, CYAN, 30);
      analogWrite(FAN_B,65+t);
    }
  }
  else if (dust>30 and dust <=80){
    
    if (temperature<25){
      rgb_LED(0, GREEN, 30);
      analogWrite(FAN_B, 127);
    }
    else {
      rgb_LED(0, GREEN, 30);
      analogWrite(FAN_B,150+t);
    }
  }
  
  else if (dust>80 and dust <=150){
    
    if (temperature<25){
      rgb_LED(0, YELLOW, 30);
      analogWrite(FAN_B, 191);
    }
    else {
      rgb_LED(0, YELLOW, 30);
      analogWrite(FAN_B,200+t);
    }
  }
  else{
    rgb_LED(0, RED, 30);
    analogWrite(FAN_B,255);
  }
}


void sendSensor(){
  float h = humid;
  float t = temperature;
  float d = dust;
  if (sw==1){
  if (d<=30){
      rgb_LED(0, CYAN, 30);
    }
  else if (dust>30 and dust <=80){
    rgb_LED(0, GREEN, 30);
  }
  
  else if (dust>80 and dust <=150){
    rgb_LED(0, YELLOW, 30);
  }
  else{
    rgb_LED(0, RED, 30);
  }
  Blynk.virtualWrite(V5, h);  // 습도가 V5
  Blynk.virtualWrite(V6, t);  // 온도가 V6
  Blynk.virtualWrite(V7, d);  // 온도가 V6
  Blynk.virtualWrite(V9, yy);  // 온도가 V6
  Blynk.virtualWrite(V10,MM);  // 온도가 V6
  Blynk.virtualWrite(V11, dd);  // 온도가 V6
  Blynk.virtualWrite(V12, hh);  // 온도가 V6
  Blynk.virtualWrite(V13, mm);  // 온도가 V6
  Blynk.virtualWrite(V14, ss);  // 온도가 V6
  Blynk.virtualWrite(V4, sw);
  Blynk.virtualWrite(V3, smart_mode);
  //Blynk.virtualWrite(V0, led_color);
  if (state==1){
    Blynk.virtualWrite(V16, "예약ON");
  }
  else{
    Blynk.virtualWrite(V16,"예약OFF");
   
  }
  }
  else{
    Blynk.virtualWrite(V4, sw);
    if (state==1){
    Blynk.virtualWrite(V16, "예약ON");
  }
  else{
    Blynk.virtualWrite(V16,"예약OFF");
   
  }
  }
}

void rgb_LED(int set_led, RGB_Color color, int brightness)
{
  int R_value = brightness;
  int G_value = brightness;
  int B_value = brightness;
   switch (color)
  {
  case RED:
    RGB_color(set_led, R_value, 0, 0);
    break; // keypad_process; operation_mode: 0; keypad released when normal mode
  case GREEN:
    RGB_color(set_led, 0, G_value, 0);
    break; // after reset; operation_mode: 0; normal idle state
  case BLUE:
    RGB_color(set_led, 0, 0, B_value);
    break; // keypad_store_data; operation_mode: 1; keypad pressed when gesture_setup mode
  case CYAN:
    RGB_color(set_led, 0, G_value, B_value);
    break; // after reset; operation_mode: 1; gesture_setup idle state
  case MAGENTA:
    RGB_color(set_led, R_value, 0, B_value);
    break; // keypad_process; operation_mode: 1; keypad released when gesture_setup mode
  case YELLOW:
    RGB_color(set_led, R_value, G_value, 0);
    break; // keypad_store_data; operation_mode: 0; keypad pressed when normal mode
  case WHITE:
    RGB_color(set_led, R_value, G_value, B_value);
    break; // general error; no gesture found
  case OFF:
    RGB_color(set_led, 0, 0, 0);
    break; // general error; no gesture found     
  default:
    break;
  } 
}
void RGB_color(int set_led, int red_light_value, int green_light_value, int blue_light_value)
{
  if (set_led == 0){
  for (int i = 0; i < 8; i++) {
    strip.setPixelColor(i, red_light_value, green_light_value, blue_light_value);
    strip.show();
    }
  }
 else if (set_led ==1){
 for (int j = 8; j < 38; j++) {
  strip.setPixelColor(j, red_light_value, green_light_value, blue_light_value);
  strip.show();
  }
 }
}

BLYNK_WRITE(V0) // LED출력
{
  int pinValue = param.asInt(); // assigning incoming value from pin V0 to a variable
  led_color = pinValue;
  Serial.println(led_color);
  if (pinValue == 0) {
     Serial.println("OFF");
     rgb_LED(1, OFF, brightness);
  }
  else if (pinValue == 1) {
    Serial.println("RED");
     rgb_LED(1, RED, brightness);
  }
  else if (pinValue == 2) {
    Serial.println("GREEN");
     rgb_LED(1, GREEN, brightness);
  }
  else if (pinValue == 3) {
    Serial.println("BLUE");
     rgb_LED(1, BLUE, brightness);
  }
  else if (pinValue == 4) {
    Serial.println("CYAN");
     rgb_LED(1, CYAN, brightness);
  }
  else if (pinValue == 5) {
    Serial.println("MAGENTA");
     rgb_LED(1, MAGENTA, brightness);
  }
  else if (pinValue == 6) {
    Serial.println("YELLOW");
     rgb_LED(1, YELLOW, brightness);
  }
  else {
    Serial.println("WHITE");
    rgb_LED(1, WHITE, brightness);
  }
}

BLYNK_WRITE(V1) // FAN출력
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  fan_speed = map(pinValue, 0, 100, 0, 256);
  analogWrite(FAN_B, fan_speed);
}

BLYNK_WRITE(V2) // LED출력
{
  int pinValue = param.asInt(); // assigning incoming value from pin V0 to a variable
  brightness = map(pinValue,0,100,0,255);
  
  Serial.println(brightness);
  if (led_color == 0) {
     rgb_LED(1, OFF, brightness);
  }
  else if (led_color == 1) {
     rgb_LED(1, RED, brightness);
  }
  else if (led_color == 2) {
     rgb_LED(1, GREEN, brightness);
  }
  else if (led_color == 3) {
     rgb_LED(1, BLUE, brightness);
  }
  else if (led_color == 4) {
     rgb_LED(1, CYAN, brightness);
  }
  else if (led_color == 5) {
     rgb_LED(1, MAGENTA, brightness);
  }
  else if (led_color == 6) {
     rgb_LED(1, YELLOW, brightness);
  }
  else {
    rgb_LED(1, WHITE, brightness);
  }
}

BLYNK_WRITE(V3) // FAN출력
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  smart_mode = pinValue;
}

BLYNK_WRITE(V4) // FAN출력
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  sw = pinValue;
}

BLYNK_WRITE(V8) // FAN출력
{
  String inString = param.asString(); // assigning incoming value from pin V1 to a variable
  int index1 = inString.indexOf(':'); 
  int index2 = inString.indexOf(':',index1+1);   
  int index3 = inString.length();
  hour = inString.substring(0, index1).toInt();
  minuate = inString.substring(index1+1,index2).toInt();
  sec = inString.substring(index2+1,index3).toInt();
  Serial.println(hour);
  Serial.println(minuate);
  Serial.println(sec);
  state=true;
}

BLYNK_WRITE(V15) // FAN출력
{
  String inString = param.asString(); // assigning incoming value from pin V1 to a variable
  int index1 = inString.indexOf(':'); 
  int index2 = inString.indexOf(':',index1+1);   
  int index3 = inString.length();
  hour2 = inString.substring(0, index1).toInt();
  minuate2 = inString.substring(index1+1,index2).toInt();
  sec2 = inString.substring(index2+1,index3).toInt();
  Serial.println(hour2);
  Serial.println(minuate2);
  Serial.println(sec2);
  state=true;
}
