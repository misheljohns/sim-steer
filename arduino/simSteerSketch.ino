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


//#include <CapacitiveSensor.h>
#include <math.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define NEOPIXPIN      A0
#define CAPSENSPIN     2
#define CAPOUTPIN      13
#define CAPSAMPLES      30


#define NUMPIXELS      144 // How many NeoPixels make the full 360 degrees (2*PI)?
#define ZEROPIXEL      0 //pixel at 0 dgrees 12 o'clock
#define DELAYVAL       1 //delay in the loop

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXPIN, NEO_GRB + NEO_KHZ800);

/*
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 */
//CapacitiveSensor   steerTouch = CapacitiveSensor(CAPOUTPIN,CAPSENSPIN);        // 10M resistor between pins 13 & 2, pin 2 is sensor pin, add a wire and or foil if desired

char inChar;
char inData[10]; //Serial buffer
char inCount = 0;
float steerVal = 0.0;
int LEDVal = 0;
uint32_t colorPattern[NUMPIXELS];


void setup() {
  Serial.begin(115200);
  Serial.println("Starting arduino..");

  pixels.begin(); // This initializes the NeoPixel library.


  // HARDCODING PATTERN HERE
  for(int i = 0;i < 5; i++) {
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
  }
  for(int i = 5;i < 40; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // OFF
  }
  for(int i = 40;i < 50; i++) {
    pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
  }
  for(int i = 50;i < NUMPIXELS - 50; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // OFF
  }
  for(int i = NUMPIXELS - 50;i < NUMPIXELS - 40; i++) {
    pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
  }
  for(int i = NUMPIXELS - 40;i < NUMPIXELS - 5; i++) {
    pixels.setPixelColor(i, pixels.Color(0,0,0)); // OFF
  }
  for(int i = NUMPIXELS - 5;i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0,150,0)); // Moderately bright green color.
  }

  pixels.setBrightness(128);//50%; max brightness - 256
  pixels.show();
  
  for(int i = 0;i < NUMPIXELS; i++) {
    colorPattern[i] = pixels.getPixelColor(i);
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
  if(tmp != LEDVal) { //we need to update the LEDs only if the input has changed, otherwise we can jump to the loop
    LEDVal = tmp;
    int ledno=0;
    for(int i=LEDVal;i<NUMPIXELS;i++,ledno++){
      pixels.setPixelColor(i, colorPattern[ledno]); // Moderately bright green color.
    }
    for(int i=0;i<LEDVal;i++,ledno++){
      pixels.setPixelColor(i, colorPattern[ledno]); // Moderately bright green color.
    }
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
  

  //can't keep writing to pixels that fast? - why do we need a delay?
  delay(DELAYVAL); // Delay for a period of time (in milliseconds).

  while(Serial.available()) {
    // read the character we recieve
    inChar = (char)Serial.read();
    if(inChar != '\n') {
        inData[inCount] = inChar; //will cause problems if input string is too long (>10)
        inCount += 1;
    }
    else { //one number input complete
        //Serial.print(inString);
        //Serial.print('\n');
        //steerVal = inString.toFloat();
	steerVal = atof(inData);
        //Serial.print(steerVal);
	//Serial.print(LEDVal);
        memset(inData, 0, sizeof(inData)); //filling inData with 0
        inCount = 0; //reset count
    }
  }
}

