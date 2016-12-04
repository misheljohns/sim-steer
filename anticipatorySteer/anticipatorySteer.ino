/*
   simSteerSketch: code to accept steering wheel angle over Serial and display it using the LED strip. Also measures capacitance on the steering, and sends it over serial.

   LED strip:
   Option 1: Set LEDs tofixed colour, change only when the steering angle changes by a fixed amount - efficient
   Option 2: evenwhen the change in angle is small, use changes in relative brightness to make changes in between the discrete steps - might work better visually

   implementing Option1 for now
*/

//TODO:
// * capacitive sensing, message to Pi
//


#include <CapacitiveSensor.h>
#include <math.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define NEOPIXPIN      A0
#define CAPSENSPIN     8
#define CAPOUTPIN      7
#define CAPSAMPLES     5


#define NUMPIXELS      179 // How many NeoPixels make the full 360 degrees (2*PI)?
#define ZEROPIXEL      45 //pixel at 0 degrees 12 o'clock
#define DELAYVAL       1 //delay in the loop

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXPIN, NEO_GRB + NEO_KHZ800);

/*
   Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
   Receive pin is the sensor pin - try different amounts of foil/metal on this pin
*/
//CapacitiveSensor   steerTouch = CapacitiveSensor(CAPOUTPIN,CAPSENSPIN);        // 2M resistor between pins 8 & 7, pin 8 is sensor pin, add a wire and or foil if desired

char inChar;
char inData[10]; //Serial buffer
char inCount = 0;
float steerVal = 0.0;
int LEDVal = 0;
uint32_t colorPattern[NUMPIXELS];

int offCount = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting arduino.."); //the pi is  always running, so this will interfere

  pixels.begin(); // This initializes the NeoPixel library.


  // HARDCODING PATTERN HERE
  for (int i = 0; i < 4; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0, 150, 0)); // Moderately bright green color - use primary colors because mixed colours might come ouot different based on position of punched hole over LED
  }
  pixels.setPixelColor(4, pixels.Color(0, 90, 0)); // Moderately bright green color fade
  pixels.setPixelColor(5, pixels.Color(0, 30, 0)); // Moderately bright green color fade
  for (int i = 6; i < 39; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // OFF
  }
  pixels.setPixelColor(39, pixels.Color(0, 30, 0)); // Moderately bright green color fade
  pixels.setPixelColor(40, pixels.Color(0, 90, 0)); // Moderately bright green color fade
  for (int i = 41; i < 49; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0)); // Moderately bright green color.
  }
  pixels.setPixelColor(49, pixels.Color(0, 90, 0)); // Moderately bright green color fade
  pixels.setPixelColor(50, pixels.Color(0, 30, 0)); // Moderately bright green color fade
  for (int i = 51; i < NUMPIXELS - 51; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // OFF
  }
  pixels.setPixelColor(NUMPIXELS - 51, pixels.Color(0, 30, 0)); // Moderately bright green color fade
  pixels.setPixelColor(NUMPIXELS - 50, pixels.Color(0, 90, 0)); // Moderately bright green color fade
  for (int i = NUMPIXELS - 49; i < NUMPIXELS - 41; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0)); // Moderately bright green color.
  }
  pixels.setPixelColor(NUMPIXELS - 41, pixels.Color(0, 90, 0)); // Moderately bright green color fade
  pixels.setPixelColor(NUMPIXELS - 40, pixels.Color(0, 30, 0)); // Moderately bright green color fade
  for (int i = NUMPIXELS - 39; i < NUMPIXELS - 6; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0)); // OFF
  }
  pixels.setPixelColor(NUMPIXELS - 6, pixels.Color(0, 30, 0)); // Moderately bright green color fade
  pixels.setPixelColor(NUMPIXELS - 5, pixels.Color(0, 90, 0)); // Moderately bright green color fade
  for (int i = NUMPIXELS - 4; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 150, 0)); // Moderately bright green color.
  }

  pixels.setBrightness(200);// max brightness - 256
  pixels.show();

  for (int i = 0; i < NUMPIXELS; i++) {
    colorPattern[i] = pixels.getPixelColor(i); //store into array
  }

}

void loop() {

  //long touchVal =  steerTouch.capacitiveSensorRaw(CAPSAMPLES);//gives absolute capacitance
  //long touchVal =  steerTouch.capacitiveSensor(CAPSAMPLES);//gives added capacitance from baseline
  //baseline recalibrated at interval set by set_CS_AutocaL_Millis() default 200000 (20s)
  //CAPSAMPLES sets number of sampeld over which capacitance is calcuilated -> lower number =  faster but less accurate
  //reset_CS_AutoCal() force an immediate calibration
  //set_CS_Timeout_Millis(unsigned long timeout_millis)  how long the method will take to timeout, if the receive (sense) pin fails to toggle in the same direction as the send pin
  
  if(offCount == 2) {//sometimes noise gives faulty readings that make the LED flash. Here I make sure I get 2 readings before it turns off
    //really weird - if i use offCount >= 2, it reads input truncated the next round - some side effect of callig setbrightness(0) twice in a row?
      pixels.setBrightness(0);//turn off
      pixels.show();
      LEDVal = NUMPIXELS + 1;//setting to an impossible value so that tmp!= LEDVal is triggered when offCount goes to 0 and the LEDs are written to again
  }
  else if(steerVal < 800.0) {
    int tmp = (round(steerVal * NUMPIXELS / (2 * 3.14)) + ZEROPIXEL) % NUMPIXELS;
    tmp = (tmp < 0) ? tmp + NUMPIXELS : tmp;
    if (tmp != LEDVal) { //we need to update the LEDs only if the input has changed, otherwise we can jump to the loop
      LEDVal = tmp;
      int ledno = 0;
      for (int i = LEDVal; i < NUMPIXELS; i++, ledno++) {
        pixels.setPixelColor(i, colorPattern[ledno]); // Pattern
      }
      for (int i = 0; i < LEDVal; i++, ledno++) {
        pixels.setPixelColor(i, colorPattern[ledno]); // Pattern
      }
      pixels.setBrightness(200);//back on
      pixels.show(); // This sends the updated pixel color to the hardware.
    }
  }

  //can't keep writing to pixels that fast? - why do we need a delay?
  //delay(DELAYVAL); // Delay for a period of time (in milliseconds).
  
  while (Serial.available()) {
    // read the character we recieve
    inChar = (char)Serial.read();
    //Serial.print(inChar);
    if (inChar != '\n') {
      inData[inCount] = inChar; //will cause problems if input string is too long (>10)
      inCount += 1;      
    }
    else { //one number input complete
      //Serial.print(inString);
      //Serial.print('\n');
      //steerVal = inString.toFloat();
      steerVal = atof(inData);
      //Serial.println(offCount);
      //Serial.println(steerVal);
      //Serial.print(touchVal);
      //Serial.print('\n');
      memset(inData, 0, sizeof(inData)); //filling inData with 0
      inCount = 0; //reset count

      if(steerVal >= 800.0) {
        offCount = offCount + 1;
        if(offCount > 10)
          offCount = 10;
      }
      else {
        offCount = 0;
      }
    }
  }
}
