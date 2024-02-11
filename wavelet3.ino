// include the library for OLED display
#include <Adafruit_SSD1306.h>

// define the display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// create an object for the display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// define the fractal parameters
#define N 4 // order of Daubechies wavelet
#define D 1.4 // fractal dimension
#define M 10 // number of iterations
#define S 0.5 // scaling factor

// define the seed points
float x0 = 0.5;
float y0 = 0.5;

// define the wavelet coefficients
float h[N] = {0.1629, 0.5055, 0.4461, -0.0198};
float g[N] = {-0.0075, 0.0233, 0.0218, -0.1323};

// define a function to draw a fractal on the display
void drawFractal(int x, int y, int w, int h, uint16_t color) {
  // initialize the x and y coordinates of the seed points
  float xprim = x0;
  float yprim = y0;
  
  // iterate the wavelet transform M times
  for (int i = 0; i < M; i++) {
    // calculate the new x and y coordinates using the wavelet coefficients
    float xnew = 0;
    float ynew = 0;
    for (int j = 0; j < N; j++) {
      xnew += h[j] * cos(2 * PI * j * xprim) - g[j] * sin(2 * PI * j * yprim);
      ynew += h[j] * sin(2 * PI * j * xprim) + g[j] * cos(2 * PI * j * yprim);
    }
    // scale the coordinates by the factor S
    xnew *= S;
    ynew *= S;
    // update the coordinates
    xprim = xnew;
    yprim = ynew;
    // map the coordinates to the display pixels
    int px = map(xnew, 0, 1, x, x + w);
    int py = map(ynew, 0, 1, y, y + h);
    // draw a pixel on the display
    display.drawPixel(px, py, color);
  }
}

// setup function
void setup() {
  // initialize the display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

// loop function
void loop() {
  // draw a fractal on the display
  drawFractal(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  // update the display
  display.display();
  // delay for 1 second
  delay(1000);
  // clear the display
  display.clearDisplay();
}
