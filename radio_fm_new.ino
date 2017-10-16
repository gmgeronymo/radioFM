#include "U8glib.h"
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>

// display LCD Nokia
U8GLIB_PCD8544 u8g(8, 9, 11, 10, 12);		//CLK=8, DIN=9, CE=11, DC=10, RST=12

const uint8_t signal5[] U8G_PROGMEM={
0xC1, 0xC3, 0xC5, 0xF9, 0xC5, 0xC3, 0xC1, 0xC0, 0xE0, 0xC0, 0xF0, 0xC0, 0xF8, 0xC0, 0xFC, 0xC0,   // 0x0010 (16) pixels
0xFE, 
};

const uint8_t signal4[] U8G_PROGMEM={
0xC1, 0xC3, 0xC5, 0xF9, 0xC5, 0xC3, 0xC1, 0xC0, 0xE0, 0xC0, 0xF0, 0xC0, 0xF8, 0xC0, 0xFC, 0xC0,   // 0x0010 (16) pixels
0xC0, 
};

const uint8_t signal3[] U8G_PROGMEM={
0xC1, 0xC3, 0xC5, 0xF9, 0xC5, 0xC3, 0xC1, 0xC0, 0xE0, 0xC0, 0xF0, 0xC0, 0xF8, 0xC0, 0xC0, 0xC0,   // 0x0010 (16) pixels
0xC0, 
};

const uint8_t signal2[] U8G_PROGMEM={
0xC1, 0xC3, 0xC5, 0xF9, 0xC5, 0xC3, 0xC1, 0xC0, 0xE0, 0xC0, 0xF0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0,   // 0x0010 (16) pixels
0xC0, 
};

const uint8_t signal1[] U8G_PROGMEM={
0xC1, 0xC3, 0xC5, 0xF9, 0xC5, 0xC3, 0xC1, 0xC0, 0xE0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0,   // 0x0010 (16) pixels
0xC0, 
};

int channel;
int channel_min = 875;
int channel_max = 1080;

char frequencyString[10];
int inByte;
int level;

/************************************************** ************************************************** */
byte frequencyH = 0;
byte frequencyL = 0;
unsigned int frequencyB;
double frequency = 0;
/************************************************** ************************************************** */



void draw(void) {
  // graphic commands to redraw the complete screen should be placed here
  //u8g.setFont(u8g_font_profont15);
  u8g.setFont(u8g_font_courB18);
  u8g.setPrintPos(15, 25); 
  u8g.print(frequency,1);
  
  // draw level
  
  if(level >=15)
  {
    u8g.drawBitmapP( 0, 0, 1, 8, signal5);
  }
  else if(level >=11 && level <15)
  {
    u8g.drawBitmapP( 0, 0, 1, 8, signal4);
  }
  else if(level >=9 && level <11)
  {
    u8g.drawBitmapP( 0, 0, 1, 8, signal3);
  }
   else if(level >=7 && level <9)
  {
    u8g.drawBitmapP( 0, 0, 1, 8, signal2);
  }
   else if(level <7)
  {
    u8g.drawBitmapP( 0, 0, 1, 8, signal1);
  }
  
  //u8g.setFont(u8g_font_profont12);
  //u8g.setPrintPos(20, 40);
  //u8g.print(level);
}

/************************************************** ************************************************** */
void setFrequency()
{
  frequency = (double)channel/10;
  frequencyB = 4 * (frequency * 1000000 + 225000) / 32768;
  frequencyH = frequencyB >> 8;
  frequencyL = frequencyB & 0XFF;
  
  Wire.beginTransmission(0x60);
  Wire.write(frequencyH);  // first byte
  Wire.write(frequencyL);  // second byte
  Wire.write(0xB2); // third byte - mono and mute right channel
  Wire.write(0x10); // fourth data - stero noise cancelling on
  Wire.write((byte)0x00); //fifth data
  Wire.endTransmission();
  
}
/************************************************** ************************************************** */

/************************************************** ************************************************** */
void getSignalLevel()
{
  unsigned char buffer[5];
  
  Wire.requestFrom(0x60,5); //reading TEA5767

  if (Wire.available()) 
  {
    for (int i=0; i<5; i++) {
      buffer[i]= Wire.read();
    }

    level = buffer[3]>>4;
  }
  
}
/************************************************** ************************************************** */

void setup(void) {
  // flip screen, if required
  // u8g.setRot180();
  
  Wire.begin();
  
  channel = 931;
  setFrequency();
  
  Serial.begin(9600);
  
}

void loop(void) {
  // serial + ou -
  if (Serial.available()>0) {
    inByte = Serial.read();
      if (inByte == '+') {
        if (channel < channel_max) {
          channel++;
          setFrequency();
        }
    }
    if (inByte == '-') {
        if (channel > channel_min) {
          channel--;
          setFrequency();
        }
    } 
    
  }
  
  delay(50);
  
  getSignalLevel();
  
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  
  // rebuild the picture after some delay
  delay(500);
}
