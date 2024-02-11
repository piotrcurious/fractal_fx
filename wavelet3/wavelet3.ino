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
#define D 1.4 // fractal dimension
#define M 1500 // number of iterations
float S = 0.1; // scaling factor

// define the seed points
float xseed0 = 1;
float yseed0 = 1;

uint32_t lastUpdate = 0 ; 
#define UPDATE_RATE 100 //100ms update rate to not waste cycles on each pixel

// define the wavelet coefficients
float w_h[N] = {0.1629, 0.5055, 0.4461, -0.0198};
float w_g[N] = {-0.0075, 0.0233, 0.0218, -0.1323};

float fmap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// define a function to draw a fractal on the display
void fractal_effect(int x, int y, int w, int h, uint16_t color) {
  // initialize the x and y coordinates of the seed points
  float xprim = random(xseed0);
  float yprim = random(yseed0);
  
  // iterate the wavelet transform M times
  for (int i = 0; i < M; i++) {
    // calculate the new x and y coordinates using the wavelet coefficients
    float xnew = 0;
    float ynew = 0;
    for (uint8_t j = 0; j < N; j++) {
      xnew += w_h[j] * cos(2 * PI * j * xprim) - w_g[j] * sin(2 * PI * j * yprim);
      ynew += w_h[j] * sin(2 * PI * j * xprim) + w_g[j] * cos(2 * PI * j * yprim);
    }
    // scale the coordinates by the factor S
    xnew *= S;
    ynew *= S;
    // update the coordinates
    xprim = xnew;
    yprim = ynew;
    // map the coordinates to the display pixels
    int px = fmap(xnew, 0, 1, x, x + w);
    int py = fmap(ynew, 0, 1, y, y + h);

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
  for (int S_factor = 0; S_factor < 10; S_factor++) {
  display.clearDisplay();
  S = S + 0.1; 
  fractal_effect(0, 0, 128, 64,WHITE);
  // Display the buffer
  display.display();
  delay(500);
  }
  delay(1000);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("loop");
  display.display();
  delay(200);
  // Do nothing
}
