// LED, 팬, 온도, 습도 정보 가져오기 // new blynk 2022년 5월 1일  
// smartcong로 성공  // smartconfig 정보가 전원을 끄면 사라짐, 다시 정보 입력해야 함

// 각자 자신의 정보를 입력해야 한다. 아래 3줄
#define BLYNK_TEMPLATE_ID "TMPLkVCUOL11"
#define BLYNK_DEVICE_NAME "Haewon"
#define BLYNK_AUTH_TOKEN ""

#define BLYNK_PRINT Serial
#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

char auth[] = BLYNK_AUTH_TOKEN;

BlynkTimer timer;

#define DHTPIN D2          // 온습도 센서가 D2로 연결
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

int LD = D0 ;  // LED켜기
int FAN = D7 ;  // 선풍기 돌리기

void setup()
{
   Serial.begin(115200);
   delay(100);
   
   WiFi.mode(WIFI_STA);

   int cnt = 0;
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (cnt++ >= 10) {
      WiFi.beginSmartConfig();
      while (1) {
        delay(1000);
        if (WiFi.smartConfigDone()) {
          Serial.println();
          Serial.println("SmartConfig: Success");
          break;
        }
        Serial.print("|");
      }
    }
  }

  WiFi.printDiag(Serial);

  dht.begin(); 
  timer.setInterval(1000L, sendSensor);   
   
  pinMode(LD, OUTPUT);
  pinMode(FAN, OUTPUT);
  digitalWrite(LD, LOW);
  digitalWrite(FAN, LOW); 

  Blynk.config(auth);
    
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);  
}

void loop() {
  Blynk.run();
  timer.run();
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V0);
  Blynk.syncVirtual(V1);
} 

void sendSensor(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
    
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V5, h);  // 습도가 V5
  Blynk.virtualWrite(V6, t);  // 온도가 V6 
}

BLYNK_WRITE(V0) // LED출력
{
  int pinValue = param.asInt(); // assigning incoming value from pin V0 to a variable
  if (pinValue == 1) {
     digitalWrite(LD, HIGH);
  } else {
    digitalWrite(LD, LOW);
  }
}

BLYNK_WRITE(V1) // FAN출력
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (pinValue == 1) {
     digitalWrite(FAN, HIGH);
  } else {
    digitalWrite(FAN, LOW);
  }
} 
