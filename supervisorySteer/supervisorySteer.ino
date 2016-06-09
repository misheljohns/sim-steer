/*
 * simSteerSketch: code to accept steering wheel angle over Serial and display it using the LED strip. Also measures capacitance on the steering, and sends it over serial.
 * 
 * LED strip:
 * Option 1: Set LEDs tofixed colour, change only when the steering angle changes by a fixed amount - efficient
 * Option 2: evenwhen the change in angle is small, use changes in relative brightness to make changes in between the discrete steps - might work better visually
 * 
 * implementing Option1 for now
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
#define ZEROPIXEL      65 //pixel at 0 degrees 12 o'clock
#define DELAYVAL       1 //delay in the loop

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXPIN, NEO_GRB + NEO_KHZ800);

/*
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 */
//CapacitiveSensor   steerTouch = CapacitiveSensor(CAPOUTPIN,CAPSENSPIN);        // 2M resistor between pins 8 & 7, pin 8 is sensor pin, add a wire and or foil if desired

char inChar;
char inData[10]; //Serial buffer
char inCount = 0;
float steerVal = 0.0;
int LEDVal = 0;
uint32_t colorPattern[NUMPIXELS];


void setup() {
  Serial.begin(115200);
  Serial.println("Starting arduino.."); //the pi is  always running, so this will interfere

  pixels.begin(); // This initializes the NeoPixel library.


  // HARDCODING PATTERN HERE
  for(int i = 0;i < 2; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Dead Zone, OFF
  }
  for(int i = 2;i < 10; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,150)); // Moderately bright blue color - move in lane
  }
  pixels.setPixelColor(10, pixels.Color(0,150,0)); // green fade out - change lanes
  pixels.setPixelColor(11, pixels.Color(0,100,0)); // 
  pixels.setPixelColor(12, pixels.Color(0,60,0)); // 
  pixels.setPixelColor(13, pixels.Color(0,30,0)); // 
  
  for(int i = 14;i < NUMPIXELS - 14; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // OFF - lane changes
  }
  
  pixels.setPixelColor(NUMPIXELS - 14, pixels.Color(0,30,0)); // green fade out
  pixels.setPixelColor(NUMPIXELS - 13, pixels.Color(0,60,0)); // 
  pixels.setPixelColor(NUMPIXELS - 12, pixels.Color(0,100,0)); // 
  pixels.setPixelColor(NUMPIXELS - 11, pixels.Color(0,150,0)); // 
  
  for(int i = NUMPIXELS - 10;i < NUMPIXELS - 2; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,150)); // Moderately bright blue color - move in lane
  }
  for(int i = NUMPIXELS - 2;i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // Dead Zone, OFF
  }

  pixels.setBrightness(154);//60%; max brightness - 256
  pixels.show();
  
  for(int i = 0;i < NUMPIXELS; i++) {
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

  int tmp = (round(steerVal*NUMPIXELS/(2*3.14)) + ZEROPIXEL) % NUMPIXELS;
  tmp = (tmp < 0) ? tmp + NUMPIXELS : tmp;
  if(tmp != LEDVal) { //we need to update the LEDs only if the input has changed, otherwise we can jump to the loop - needed to keep update freq at 100Hz
    LEDVal = tmp;
    int ledno=0;
    for(int i=LEDVal;i<NUMPIXELS;i++,ledno++){
      pixels.setPixelColor(i, colorPattern[ledno]); // Pattern
    }
    for(int i=0;i<LEDVal;i++,ledno++){
      pixels.setPixelColor(i, colorPattern[ledno]); // Pattern
    }
    pixels.setPixelColor(ZEROPIXEL, pixels.Color(200,0,0)); // Show center position - bright red
    pixels.show(); // This sends the updated pixel color to the hardware.
  }

  //can't keep writing to pixels that fast? - why do we need a delay? - we don't; tested.
  //delay(DELAYVAL); // Delay for a period of time (in milliseconds).

  while(Serial.available()) {
    // read the character we recieve
    inChar = (char)Serial.read();
    if(inChar != '\n') {
        inData[inCount] = inChar; //will cause problems if input string is too long (>10)
        inCount += 1;
        //Serial.print(inChar);
    }
    else { //one number input complete
        //Serial.print(inString);
        //Serial.print('\n');
        //steerVal = inString.toFloat();
        steerVal = atof(inData);
        //Serial.print(steerVal);
        //Serial.print(touchVal);        
        //Serial.print('\n');
        memset(inData, 0, sizeof(inData)); //filling inData with 0
        inCount = 0; //reset count
    }
  }
}
