// Include the Adafruit SSD1306 library
#include <Adafruit_SSD1306.h>

// Define the OLED display width and height
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Create an OLED display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Define the number of Daubechies wavelet coefficients
#define N 4

// Define the wavelet coefficients array
double h[N] = {0.4829629131445341, 0.8365163037378079, 0.2241438680420134, -0.1294095225512604};

// Define the number of iterations for the fractal generation
#define ITER 5

// Define the random seed points array
double s[SCREEN_WIDTH];

// Define the fractal array
double f[SCREEN_WIDTH];

// Define the hidden layer dimension set of xprim and yprim
double xprim[SCREEN_WIDTH];
double yprim[SCREEN_WIDTH];

// Define the color for the fractal pixels
#define COLOR WHITE

// Define a function to generate a random number between 0 and 1
double random01() {
  return (double) random(0, 10000) / 10000.0;
}

// Define a function to apply the Daubechies wavelet transform to an array
void wavelet(double* input, double* output, int length) {
  // Check if the length is divisible by 2
  if (length % 2 != 0) {
    // Return an error message
    Serial.println("Length must be divisible by 2");
    return;
  }
  // Loop through the output array
  for (int i = 0; i < length / 2; i++) {
    // Initialize the output values
    output[i] = 0;
    output[i + length / 2] = 0;
    // Loop through the wavelet coefficients
    for (int j = 0; j < N; j++) {
      // Calculate the index for the input array
      int k = (2 * i + j) % length;
      // Update the output values
      output[i] += h[j] * input[k];
      output[i + length / 2] += h[N - j - 1] * input[k];
    }
  }
}

// Define a function to apply the inverse Daubechies wavelet transform to an array
void inverse_wavelet(double* input, double* output, int length) {
  // Check if the length is divisible by 2
  if (length % 2 != 0) {
    // Return an error message
    Serial.println("Length must be divisible by 2");
    return;
  }
  // Loop through the output array
  for (int i = 0; i < length; i++) {
    // Initialize the output value
    output[i] = 0;
    // Loop through the wavelet coefficients
    for (int j = 0; j < N; j++) {
      // Calculate the index for the input array
      int k = (i - j + length) % length;
      // Update the output value
      output[i] += h[j] * input[k / 2] + h[N - j - 1] * input[k / 2 + length / 2];
    }
  }
}

// Define a function to generate a fractal array from a random seed array
void fractal(double* seed, double* fractal, int length, int iter) {
  // Check if the iteration is zero
  if (iter == 0) {
    // Copy the seed array to the fractal array
    for (int i = 0; i < length; i++) {
      fractal[i] = seed[i];
    }
    // Return from the function
    return;
  }
  // Create a temporary array
  double temp[length];
  // Apply the wavelet transform to the seed array
  wavelet(seed, temp, length);
  // Recursively generate the fractal array from the low frequency part of the temporary array
  fractal(temp, fractal, length / 2, iter - 1);
  // Copy the high frequency part of the temporary array to the fractal array
  for (int i = 0; i < length / 2; i++) {
    fractal[i + length / 2] = temp[i + length / 2];
  }
  // Apply the inverse wavelet transform to the fractal array
  inverse_wavelet(fractal, temp, length);
  // Copy the temporary array to the fractal array
  for (int i = 0; i < length; i++) {
    fractal[i] = temp[i];
  }
}

// Define a function to calculate the frequency symmetry of an array
double symmetry(double* array, int length) {
  // Initialize the sum of the absolute values of the array elements
  double sum = 0;
  // Loop through the array
  for (int i = 0; i < length; i++) {
    // Update the sum
    sum += abs(array[i]);
  }
  // Check if the sum is zero
  if (sum == 0) {
    // Return zero
    return 0;
  }
  // Initialize the sum of the absolute values of the array elements in the first half
  double sum1 = 0;
  // Loop through the first half of the array
  for (int i = 0; i < length / 2; i++) {
    // Update the sum
    sum1 += abs(array[i]);
  }
  // Initialize the sum of the absolute values of the array elements in the second half
  double sum2 = 0;
  // Loop through the second half of the array
  for (int i = length / 2; i < length; i++) {
    // Update the sum
    sum2 += abs(array[i]);
  }
  // Calculate the frequency symmetry as the ratio of the two sums
  double sym = sum1 / sum2;
  // Return the frequency symmetry
  return sym;
}

// Define a function to calculate the fractal dimension of an array
double dimension(double* array, int length) {
  // Initialize the minimum and maximum values of the array elements
  double min = array[0];
  double max = array[0];
  // Loop through the array
  for (int i = 1; i < length; i++) {
    // Update the minimum and maximum values
    if (array[i] < min) {
      min = array[i];
    }
    if (array[i] > max) {
      max = array[i];
    }
  }
  // Calculate the range of the array elements
  double range = max - min;
  // Check if the range is zero
  if (range == 0) {
    // Return zero
    return 0;
  }
  // Initialize the sum of the absolute differences of the array elements
  double sum = 0;
  // Loop through the array
  for (int i = 0; i <  length - 1; i++) {
    // Update the sum
    sum += abs(array[i + 1] - array[i]);
  }
  // Calculate the average of the absolute differences of the array elements
  double avg = sum / (length - 1);
  // Calculate the fractal dimension as the logarithm of the ratio of the range and the average
  double dim = log(range / avg) / log(2);
  // Return the fractal dimension
  return dim;
}

// Define a function to create a fractal visual effect on the OLED display
void fractal_effect(int x, int y, int width, int height) {
  // Check if the width and height are valid
  if (width <= 0 || height <= 0) {
    // Return an error message
    Serial.println("Width and height must be positive");
    return;
  }
  // Check if the width and height are divisible by 2
  if (width % 2 != 0 || height % 2 != 0) {
    // Return an error message
    Serial.println("Width and height must be divisible by 2");
    return;
  }
  // Check if the width and height are within the screen bounds
  if (x + width > SCREEN_WIDTH || y + height > SCREEN_HEIGHT) {
    // Return an error message
    Serial.println("Width and height must be within the screen bounds");
    return;
  }
  // Generate random seed points for the x and y coordinates
  for (int i = 0; i < width; i++) {
    s[i] = random01();
  }
  // Generate the fractal array for the x coordinates
  fractal(s, f, width, ITER);
  // Copy the fractal array to the xprim array
  for (int i = 0; i < width; i++) {
    xprim[i] = f[i];
  }
  // Generate the fractal array for the y coordinates
  fractal(s, f, width, ITER);
  // Copy the fractal array to the yprim array
  for (int i = 0; i < width; i++) {
    yprim[i] = f[i];
  }
  // Calculate the frequency symmetry of the xprim and yprim arrays
  double symx = symmetry(xprim, width);
  double symy = symmetry(yprim, width);
  // Calculate the fractal dimension of the xprim and yprim arrays
  double dimx = dimension(xprim, width);
  double dimy = dimension(yprim, width);
  // Normalize the xprim and yprim arrays to fit the width and height of the rectangle
  for (int i = 0; i < width; i++) {
    xprim[i] = x + xprim[i] * width;
    yprim[i] = y + yprim[i] * height;
  }
  // Draw the fractal pixels on the display
  for (int i = 0; i < width - 1; i++) {
    // Draw a line between two consecutive points
    display.drawLine(xprim[i], yprim[i], xprim[i + 1], yprim[i + 1], COLOR);
  }
  // Display the frequency symmetry and the fractal dimension on the serial monitor
  Serial.print("Frequency symmetry of x: ");
  Serial.println(symx);
  Serial.print("Frequency symmetry of y: ");
  Serial.println(symy);
  Serial.print("Fractal dimension of x: ");
  Serial.println(dimx);
  Serial.print("Fractal dimension of y: ");
  Serial.println(dimy);
}

// Define the setup function
void setup() {
  // Initialize the serial communication
  Serial.begin(9600);
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
  delay(2000);
  // Clear the display buffer
  display.clearDisplay();
  // Create a fractal visual effect on the display
  fractal_effect(0, 0, 128, 64);
  // Display the buffer
  display.display();
}

// Define the loop function
void loop() {
  // Do nothing
}
