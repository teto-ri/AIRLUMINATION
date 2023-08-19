#include <Adafruit_NeoPixel.h>


#define PIN D7;
#define NUMPIXELS 8;
#define bright 255;
#define dly 10000;
Adafruit_NeoPixel neo(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    //neopixel 시작
    neo.begin();
    //neopixel 밝기 설정
    neo.setBrightness(bright);
    //neopixel 초기화
    neo.clear();
    //neopixel 적용
    //.show()를 사용하기 전까진 내부적으로만 설정이 적용되었다 생각하면 됩니다.
    //.show()가 실행되면 led에 내부적으로 적용된 명령들이 실행됩니다.
    neo.show();
}

void loop()
{
    //led 흰색 점등
    for (int i = 0; i < 15; i++)
    {
        neo.setPixelColor(i,255,255,255);
    }
    neo.show();
    delay(500);

    //led 적색 점등
    for (int i = 0; i < 15; i++)
    {
        neo.setPixelColor(i,255,0,0);
    }
    neo.show();
    delay(500);

    //led 녹색 점등
    for (int i = 0; i < 15; i++)
    {
        neo.setPixelColor(i,0,255,0);
    }
    neo.show();
    delay(500);

    //led 청색 점등
    for (int i = 0; i < 15; i++)
    {
        neo.setPixelColor(i,0,0,255);
    }
    neo.show();
    delay(500);
}
