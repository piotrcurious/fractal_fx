// Include the Adafruit SSD1306 library
#include <Adafruit_SSD1306.h>
#include <math.h>


// Define the OLED display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Create an OLED display object
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Declaration for SSD1306 display connected using software SPI (default case):
/*
#define OLED_MOSI   D7 
#define OLED_CLK    D5
#define OLED_DC     D2
#define OLED_CS     D8
#define OLED_RESET  D3
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
*/
///*
// Comment out above, uncomment this block to use hardware SPI
#define OLED_DC     D2
#define OLED_CS     D8
#define OLED_RESET  D3
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RESET, OLED_CS);

// define the fractal parameters
#define N 4 // order of Daubechies wavelet
#define D 1.3 // fractal dimension
#define M 400 // number of iterations
float S = 0.1; // scaling factor

// define the seed points
float xseed0 = 128;
float yseed0 = 64;

uint32_t lastUpdate = 0 ; 
uint32_t calc_time = 0; // time spent for drawing
#define UPDATE_RATE 50 //100ms update rate to not waste cycles on each pixel

// define the wavelet coefficients
float w_h[N] = {0.1629, 0.5055, 0.4461, -0.0198};
//float w_h[N] = {0.6830127, 1.1830127, 0.3169873, -0.1830127}; // from wikipedia
float w_g[N] = {-0.0075, 0.0233, 0.0218, -0.1323};

float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// define a function to draw a fractal on the display
void fractal_effect(int x, int y, int w, int h, uint16_t color) {
  // initialize the x and y coordinates of the seed points
  float xprim = random(xseed0)/xseed0;
  float yprim = random(yseed0)/yseed0;

//  float xprim = 0.0;
//  float yprim = 0.0;

  
  // iterate the wavelet transform M times
  for (int i = 0; i < M; i++) {
    // calculate the new x and y coordinates using the wavelet coefficients
//    float xnew = xprim;
//    float ynew = yprim;
    float xnew = 0;
    float ynew = 0;

    
    for (uint8_t j = 0; j < N; j++) {
      xnew += w_h[j] * cos(D * PI * j * xprim) - w_g[j] * sin(D * PI * j * yprim);
      ynew += w_h[j] * sin(D * PI * j * xprim) + w_g[j] * cos(D * PI * j * yprim);
    }
    // scale the coordinates by the factor S
//    xprim = xnew;
//    yprim = ynew;

    xnew *= S;
    ynew *= S;
    // update the coordinates
    xprim = xnew;
    yprim = ynew;
    // map the coordinates to the display pixels
//    int px = fmap(xnew, -1, 1, x, x + w);
//    int py = fmap(ynew, -1, 1, y, y + h);
//    int px = map((xnew+abs(S))*128, 0, 128, x, x + w);
//    int py = map((ynew+abs(S))*64, 0, 64, y, y + h);
    int px = map(xnew*128, -128, 128, x, x + w);
    int py = map(ynew*64, -64, 64, y, y + h);


//  display.setCursor(0, 0);
//  display.println(xprim);

    // draw a pixel on the display
    display.drawPixel(px, py, color);
   // update rate check
    if (millis() - lastUpdate >= UPDATE_RATE) {
      // Update the last update time
      display.display();
      lastUpdate = millis();

    }
  }
  
}
// Define the setup function
void setup() {
  // Initialize the serial communication
  Serial.begin(115200);
  // Initialize the OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // Clear the display buffer
  display.clearDisplay();
  // Set the text size and color
  display.setTextSize(1);
  display.setTextColor(WHITE);
  // Display a message on the screen
  display.setCursor(0, 0);
  display.println("Fractal Visual Effect");
  display.display();
  // Wait for 2 seconds
  delay(1000);
  // Clear the display buffer
  display.clearDisplay();
}

// Define the loop function
void loop() {
  S = -0.5;
  for (int S_factor = 0; S_factor < 60; S_factor++) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(S);
  S = S + 0.02; 
  calc_time = millis();
  fractal_effect(0, 0, 128, 64,WHITE);
  // Display the buffer

  display.setCursor(64, 0);
  uint32_t calc_time_result = millis()-calc_time;
  display.print(calc_time_result);
  display.display();
 
//  delay(50);
  }

  S = +0.5;
  for (int S_factor = 0; S_factor < 60; S_factor++) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(S);
  S = S - 0.02; 
  calc_time = millis();
  fractal_effect(0, 0, 128, 64,WHITE);
  // Display the buffer

  display.setCursor(64, 0);
  uint32_t calc_time_result = millis()-calc_time;
  display.print(calc_time_result);
  display.display();
  //delay(50);
  }

//  delay(100);
//  display.clearDisplay();
//  display.setCursor(0, 0);
//  display.println("loop");
//  display.display();
//  delay(200);
  // Do nothing
}
