#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>

#define STORBE_PIN   4
#define RESET_PIN    3
#define OUT_PIN     A5
#define WS2812_PIN_R 7
#define WS2812_PIN_L 8

#define ASCII_OFFSET 48

#define LED_MAX_VALUE  240
#define LED_MIN_VALUE  5
#define LED_VALUE_STEP 5

int currentLevel[7] = {0,0,0,0,0,0,0};

byte color_red = 255;
byte color_green = 0;
byte color_blue = 0;

bool isMobileColorActivated = false;

unsigned long lastHueChange = 0;
const unsigned long hueChangeInterval = 500; // ms
byte hue = 0;

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

void hsvToRgb(byte h, byte s, byte v, byte &r, byte &g, byte &b) {
  float hf = h / 256.0 * 6.0;
  int i = floor(hf);
  float f = hf - i;
  byte p = v * (255 - s) / 255;
  byte q = v * (255 - (s * f)) / 255;
  byte t = v * (255 - (s * (1 - f))) / 255;

  switch (i) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    default: r = v; g = p; b = q; break;
  }
}

void changeColor()
{
  if(Serial.available() > 0) {
    isMobileColorActivated = true;
    
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



void setBar(byte i, int level)
{
  byte level_mapped = map(level, 100, 950, 0, 13);
    
  for (int y = 0; y < level_mapped; y++)
  {
    stripR.setPixelColor((i*13)+y, stripR.Color(color_red, color_green, color_blue));
    stripL.setPixelColor((i*13)+y, stripL.Color(color_red, color_green, color_blue));
  }
}

void loop()
{  
  unsigned long now = millis();
  if (now - lastHueChange >= hueChangeInterval) {
    lastHueChange = now;
    hue++;
    if (hue > 255) hue = 0;
  }

  if (!isMobileColorActivated) {
    hsvToRgb(hue, 255, 255, color_red, color_green, color_blue);
  }
  
  int numSamples = 5;
  int levelSum[7] = {0};
  int levelAvg[7] = {0};   
  
  changeColor();

  for (int i = 0; i < numSamples; i++) {
    for (int j = 0; j < 7; j++) {
      digitalWrite(STORBE_PIN, LOW);
      delayMicroseconds(100);

      levelSum[j] += analogRead(OUT_PIN);

      digitalWrite(STORBE_PIN, HIGH);
      delayMicroseconds(100); 
    }
  }

  for (int i = 0; i < 7; i++) {
    levelAvg[i] = levelSum[i] / numSamples;
  }

  stripR.clear();
  stripL.clear();


  for (int i = 0; i < 7; i++) {
    setBar(i, levelAvg[i]);
  }


  stripR.show();
  stripL.show();

  delayMicroseconds(100); 
}
