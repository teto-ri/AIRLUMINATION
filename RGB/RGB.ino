#include <Adafruit_NeoPixel.h>

#define LED_PIN D7      // 네오픽셀 D핀과 연결한 아두이노 핀 번호
#define LED_COUNT 38  // 네오픽셀 LED 개수
#define BRIGHTNESS 50  // 네오픽셀 LED 밝기(0 ~ 255) *RGBW만
// RGB일 경우 NEO_GRBW 대신 NEO_GRBW 입력
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

enum RGB_Color
{
  OFF,
  RED,
  GREEN,
  BLUE,
  CYAN,
  MAGENTA,
  YELLOW,
  WHITE,
};

int set_led = 0;
int brightness = 255;

void setup() {
  strip.begin();                    // 네오픽셀 초기화(필수)
  strip.setBrightness(BRIGHTNESS);  // 네오픽셀 밝기 설정 *RGBW만
  rgb_LED(0,OFF, 0);
  rgb_LED(1,OFF, 0);
  
}

void loop() {

  rgb_LED(0, WHITE, 128);
  rgb_LED(1, OFF, 128);
  
  // 네오픽셀 설정값 적용하기
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
