#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#define STORBE_PIN   4
#define RESET_PIN    3
#define OUT_PIN     A5
#define WS2812_PIN_R 7
#define WS2812_PIN_L 8

int level[7];

#define ASCII_OFFSET 48

#define LED_MAX_VALUE  240
#define LED_MIN_VALUE  5
#define LED_VALUE_STEP 5

int8_t color_red   = 10;
int8_t color_green = 10;
int8_t color_blue  = 10;

SoftwareSerial BTserial(0,1);

Adafruit_NeoPixel stripR = Adafruit_NeoPixel(91, WS2812_PIN_R, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripL = Adafruit_NeoPixel(91, WS2812_PIN_L, NEO_GRB + NEO_KHZ800);

void setup()
{
  Serial.begin  (9600);
  BTserial.begin(9600);

  stripR.begin();
  stripR.show();

  stripL.begin();
  stripL.show();

  pinMode       (STORBE_PIN, OUTPUT);
  pinMode       (RESET_PIN,  OUTPUT);
  pinMode       (OUT_PIN,    INPUT);

  digitalWrite  (RESET_PIN,  LOW);
  digitalWrite  (STORBE_PIN, LOW);
  delay         (1);

  digitalWrite  (RESET_PIN,  HIGH);
  delay         (1);
  digitalWrite  (RESET_PIN,  LOW);
  digitalWrite  (STORBE_PIN, HIGH);
  delay         (1);
}

void setBar(byte i, int level)
{
  byte level_mapped = map(level, 100, 950, 0, 13);
    
  for (int y = 0; y < level_mapped; y++)
  {
    stripR.setPixelColor((i*13)+y, stripR.Color(color_red, color_green, color_blue));
    stripL.setPixelColor((i*13)+y, stripL.Color(color_red, color_green, color_blue));
  }
}

void changeColor()
{
  if(Serial.available() > 0) {
    int input = Serial.read();
  
    if(color_green <= LED_MAX_VALUE && input == 1)
    {
      color_green += LED_VALUE_STEP;
    }
    else if(color_red <= LED_MAX_VALUE && input == 2)
    {
      color_red += LED_VALUE_STEP;
    }
    else if(color_blue <= LED_MAX_VALUE && input == 3)
    {
      color_blue += LED_VALUE_STEP;
    }
    else if(color_green >= LED_MIN_VALUE && input == 4)
    {
      color_green -= LED_VALUE_STEP;
    }
    else if(color_red >= LED_MIN_VALUE && input == 5)
    {
      color_red -= LED_VALUE_STEP;
    }
    else if(color_blue >= LED_MIN_VALUE && input == 6)
    {
      color_blue -= LED_VALUE_STEP;
    }
  }
}

void loop()
{  
  changeColor();
  
  for (int i = 0; i < 7; i++) {
    digitalWrite       (STORBE_PIN, LOW);
    delayMicroseconds  (100);

    level[i] =         analogRead (OUT_PIN);
    
    digitalWrite       (STORBE_PIN, HIGH);
    delayMicroseconds  (100); 
  }

  stripR.clear();
  stripL.clear();

  for (int i = 0; i < 7; i++)
  {
    setBar(i, level[i]);
  }

  stripR.show();
  stripL.show();

  delayMicroseconds(10000);
}
