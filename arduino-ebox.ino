#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>         // Core graphics library
#include <Adafruit_ILI9341.h>     // Hardware-specific library
#include <Adafruit_ImageReader.h> // Image-reading functions
//#include <Fonts/FreeSans18pt7b.h> // Sans serif font
#include <Fonts/FreeSans12pt7b.h> // Sans serif font
#include <stdio.h>

#define SD_CS     4 // SD card select pin
#define TFT_CS    10 // TFT select pin
#define TFT_DC    9 // TFT display/command pin
#define TFT_RST   8 // TFT reset 

#define TXT_ORANGE  0xFC00
#define TXT_BLUE  0x0E1F
#define BG_GREY   0x3186
#define TXT_GREY  0xCE79
#define BLACK     0x0000
#define WHITE     0xFFFF

int values[4] = {0,0,0,0}; // v1, i1, v2, i2

float r1 = 1000.0; //kohms
float r2 = 33.2; //kohms
float v_ref = 4.96;
float bits = 1023.0;

float vv1 = 0.0;
float r_vv1 = 0.0;
float vi1 = 0.0;
float r_vi1 = 0.0;
float vv2 = 0.0;
float r_vv2 = 0.0;
float vi2 = 0.0;
float r_vi2 = 0.0;

char buffer_vv1[4];
char buffer_vi1[4];
char buffer_vv2[4];
char buffer_vi2[4];

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
Adafruit_ImageReader reader;     // Class w/image-reading functions

void setup() {


  tft.begin();
  if(!SD.begin(SD_CS)) {
    for(;;); // Loop here forever
  }
//   Setup tft propterties
  tft.setRotation(3);

//   Loading Screen
  showInitScreen();
  delay(2000);

  // Prepare UI
  tft.fillScreen(BG_GREY);
  drawBlank();

}

void loop() {
  
  getNewValues();
  drawValues();

}

void drawValues() {

  tft.setFont();
  tft.setTextSize(6);

// Channel 1
  tft.setTextColor(TXT_ORANGE,BG_GREY);
  
  tft.setCursor(12,90);
  sprintf(buffer_vv1, "%4d", values[0]);
  tft.println(buffer_vv1); //vv1
  
  tft.setCursor(12+36,180);
  sprintf(buffer_vi1, "%2d", values[1]);
  tft.println(buffer_vi1); //vi1
  
  // Channel 2
  tft.setTextColor(TXT_BLUE,BG_GREY);
  
  tft.setCursor(160+12,90);
  sprintf(buffer_vv2, "%4d", values[2]);
  tft.println(buffer_vv2); //vv2
  
  tft.setCursor(160+36+12,180);
  sprintf(buffer_vi2, "%2d", values[3]);
  tft.println(buffer_vi2); //vi2
 
}


void getNewValues(){

  // Read in all values and convert it to voltages
  r_vv1 = analogRead(0);
  vv1 = (r_vv1 * v_ref)/bits;
  r_vi1 = analogRead(1);
  vi1 = (r_vi1 * v_ref)/bits;
  r_vv2 = analogRead(2);
  vv2 = (r_vv2 * v_ref)/bits;
  r_vi2 = analogRead(3);
  vi2 = (r_vi2 * v_ref)/bits;

  // Calculate real voltages
  vv1 = ((vv1-2.5) * 4 / (r2/(r1+r2))); 
  vi1 = (vi1-2.5) * 20; // 10 V translate to 50 mA
  vv2 = ((vv2-2.5) * 4 / (r2/(r1+r2)));
  vi2 = (vi2-2.5) * 20;

  // Calibrate voltages
  vv1 = 9.0 + (vv1/1.03865); // Values from linear fit
  vv2 = 9.0 + (vv2/1.03495); // Values from linear fit
  
  values[0] = vv1;
  values[1] = abs(vi1);
  values[2] = vv2;
  values[3] = abs(vi2);
  
}

void drawBlank(){

  tft.setTextColor(TXT_GREY);
  tft.setFont(&FreeSans12pt7b);
  tft.setTextSize(1);
  tft.setCursor(28,40);
  tft.println("Channel 1");
  tft.setCursor(160+28,40);
  tft.println("Channel 2");

  tft.setCursor(10,80);
  tft.println("V");
  tft.setCursor(10,170);
  tft.println("mA");

  tft.setCursor(170,80);
  tft.println("V");
  tft.setCursor(170,170);
  tft.println("mA"); 
  
}

void showInitScreen() {
  
  ImageReturnCode stat; // Status from image-reading functions
  stat = reader.drawBMP("/dogo.bmp", tft, 0, 0); // Load full-screen BMP file 'dogo.bmp'
  
}
