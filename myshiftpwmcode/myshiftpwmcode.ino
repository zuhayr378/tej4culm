//#define SHIFTPWM_USE_TIMER3
#include <ShiftPWM.h>
//#include <SPI.h>

//Apparently using the define command makes life easier.


int A1State = 0;
int A1Pin = 35;

int A2State = 0;
int A2Pin = 29;

int A3State = 0;
int A3Pin = 27;

int B1State = 0;
int B1Pin = 37;

int B2State = 0;
int B2Pin = 31;

int B3State = 0;
int B3Pin = 25;

int C1State = 0;
int C1Pin = 39;

int C2State = 0;
int C2Pin = 33;

int C3State = 0;
int C3Pin = 23;

int buznum = 0;
//int buzread = 0;

//For now this is a buzzer but I'll change it up later
const int buzzer = 2;


//SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE0));

int lightmode = 0;

/*
 * ShiftPWM non-blocking RGB fades example, (c) Elco Jacobs, updated August 2012.
 *
 * This example for ShiftPWM shows how to control your LED's in a non-blocking way: no delay loops.
 * This example receives a number from the serial port to set the fading mode. Instead you can also read buttons or sensors.
 * It uses the millis() function to create fades. The block fades example might be easier to understand, so start there.
 *
 * Please go to www.elcojacobs.com/shiftpwm for documentation, fuction reference and schematics.
 * If you want to use ShiftPWM with LED strips or high power LED's, visit the shop for boards.
 */

// ShiftPWM uses timer1 by default. To use a different timer, before '#include <ShiftPWM.h>', add
// #define SHIFTPWM_USE_TIMER2  // for Arduino Uno and earlier (Atmega328)
// #define SHIFTPWM_USE_TIMER3  // for Arduino Micro/Leonardo (Atmega32u4)

// Clock and data pins are pins from the hardware SPI, you cannot choose them yourself.
// Data pin is MOSI (Uno and earlier: 11, Leonardo: ICSP 4, Mega: 51, Teensy 2.0: 2, Teensy 2.0++: 22) 
// Clock pin is SCK (Uno and earlier: 13, Leonardo: ICSP 3, Mega: 52, Teensy 2.0: 1, Teensy 2.0++: 21)

// You can choose the latch pin yourself.
const int ShiftPWM_latchPin = 8;

// ** uncomment this part to NOT use the SPI port and change the pin numbers. This is 2.5x slower **
//#define SHIFTPWM_NOSPI
//const int ShiftPWM_dataPin = 11;
//const int ShiftPWM_clockPin = 13;


// If your LED's turn on if the pin is low, set this to true, otherwise set it to false.
const bool ShiftPWM_invertOutputs = true;

// You can enable the option below to shift the PWM phase of each shift register by 8 compared to the previous.
// This will slightly increase the interrupt load, but will prevent all PWM signals from becoming high at the same time.
// This will be a bit easier on your power supply, because the current peaks are distributed.
const bool ShiftPWM_balanceLoad = false;


// Function prototypes (telling the compiler these functions exist).
void oneByOne(void);
void inOutTwoLeds(void);
void inOutAll(void);
void alternatingColors(void);
void hueShiftAll(void);
void randomColors(void);
void fakeVuMeter(void);
void rgbLedRainbow(unsigned long cycleTime, int rainbowWidth);
void printInstructions(void);


// Here you set the number of brightness levels, the update frequency and the number of shift registers.
// These values affect the load of ShiftPWM.
// Choose them wisely and use the PrintInterruptLoad() function to verify your load.
unsigned char maxBrightness = 255;
unsigned char pwmFrequency = 100;
unsigned int numRegisters = 4;
unsigned int numOutputs = 27;
unsigned int numRGBLeds = 9;
//unsigned int fadingMode = 2; //start with all LED's off.

unsigned long startTime = 0; // start time for the chosen fading mode

unsigned long previousMillis=0;



void setup(){

  pinMode(buzzer, OUTPUT);
  
  while(!Serial){
    delay(100); 
  }
  Serial.begin(9600);

  // Sets the number of 8-bit registers that are used.
  ShiftPWM.SetAmountOfRegisters(numRegisters);

  // SetPinGrouping allows flexibility in LED setup. 
  // If your LED's are connected like this: RRRRGGGGBBBBRRRRGGGGBBBB, use SetPinGrouping(4).
  ShiftPWM.SetPinGrouping(1); //This is the default, but I added here to demonstrate how to use the funtion
  
  ShiftPWM.Start(pwmFrequency,maxBrightness);
}



void loop(){
  unsigned long currentMillis = millis();

  //maxBrightness = analogRead(0);
  //Serial.println(maxBrightness);
  //maxBrightness = map(maxBrightness, 0, 168, 0, 255);
  
  if (lightmode!=0){ 
    previousMillis = currentMillis;
   }
  else{
    startTime=millis();
  }

/*
note    frequency    period    timeHigh
c       261 Hz       3830       1915   
d       294 Hz       3400       1700  
e       329 Hz       3038       1519  
f       349 Hz       2864       1432  
g       392 Hz       2550       1275  
a       440 Hz       2272       1136  
b       493 Hz       2028       1014  
C       523 Hz       1912        956

*/
lightmode = 2;
C1State = digitalRead(C1Pin);
if (C1State == HIGH) {
  lightmode = 2;
  tone(buzzer, 261);
 }

C2State = digitalRead(C2Pin);
if (C2State == HIGH) {
  lightmode = 4;
  tone(buzzer, 294);
 }

C3State = digitalRead(C3Pin);
if (C3State == HIGH) {
  lightmode = 3;
  tone(buzzer, 329);
 }

B3State = digitalRead(B3Pin);
if (B3State == HIGH) {
  lightmode = 0;
  tone(buzzer, 349);
 }

A3State = digitalRead(A3Pin);
if (A3State == HIGH) {
  lightmode = 5;
 }

else{
  noTone(buzzer);
}

  if (lightmode == 0){
    ShiftPWM.SetAll(0);
  }

  if (lightmode == 1){
    startTime=0;
    oneByOne();
       
   }

  if (lightmode == 2){
    inOutTwoLeds();
  }

  if (lightmode == 3){
    startTime=0;
    inOutAll();
  }

  if (lightmode == 4){
    rgbLedRainbow(3000,numRGBLeds);
  }

  if (lightmode == 5){
    randomColors();
  }



}


void inOutAll(void){  // Fade in all outputs
  unsigned char brightness;
  unsigned long fadeTime = 2500; //default is 2000
  unsigned long time = millis()-startTime;
  unsigned long currentStep = time%(fadeTime*2);

  if(currentStep <= fadeTime ){
    brightness = currentStep*maxBrightness/fadeTime; ///fading in
  }
  else{
    brightness = maxBrightness-(currentStep-fadeTime)*maxBrightness/fadeTime; ///fading out;
  }
  ShiftPWM.SetAll(brightness);
}


void oneByOne(void){ // Fade in and fade out all outputs one at a time
  unsigned char brightness;
  unsigned long fadeTime = 500;
  unsigned long loopTime = numOutputs*fadeTime*2;
  unsigned long time = millis()-startTime;
  unsigned long timer = time%loopTime;
  unsigned long currentStep = timer%(fadeTime*2);

  int activeLED = timer/(fadeTime*2);

  if(currentStep <= fadeTime ){
    brightness = currentStep*maxBrightness/fadeTime; ///fading in
  }
  else{
    brightness = maxBrightness-(currentStep-fadeTime)*maxBrightness/fadeTime; ///fading out;
  }
  ShiftPWM.SetAll(0);
  ShiftPWM.SetOne(activeLED, brightness);
  ShiftPWM.SetOne(activeLED+1, brightness);
  ShiftPWM.SetOne(activeLED+2, brightness);


  //ShiftPWM.SetRGB(,255,255,255);

}

void inOutTwoLeds(void){ // Fade in and out 2 outputs at a time
  
  unsigned long fadeTime = 500; //default is 500
  unsigned long loopTime = numOutputs*fadeTime;
  unsigned long time = millis()-startTime;
  unsigned long timer = time%loopTime;
  unsigned long currentStep = timer%fadeTime;

  int activeLED = timer/fadeTime;
  unsigned char brightness = currentStep*maxBrightness/fadeTime;

  ShiftPWM.SetAll(0);
  ShiftPWM.SetOne((activeLED+1)%numOutputs,brightness);
  //my code
  //ShiftPWM.SetGroupOf3((activeLED+1)%numOutputs,(activeLED+2)%numOutputs,(activeLED+3)%numOutputs,brightness);

  
  ShiftPWM.SetOne(activeLED,maxBrightness-brightness);

  //my code:
  //ShiftPWM.SetGroupOf3(activeLED,activeLED+1,activeLED+2,maxBrightness-brightness);
}

void hueShiftAll(void){  // Hue shift all LED's
  unsigned long cycleTime = 10000;
  unsigned long time = millis()-startTime;
  unsigned long hue = (360*time/cycleTime)%360;
  ShiftPWM.SetAllHSV(hue, 255, 255); 
}

void randomColors(void){  // Update random LED to random color. Funky!
  unsigned long updateDelay = 100;
  static unsigned long previousUpdateTime;
  if(millis()-previousUpdateTime > updateDelay){
    previousUpdateTime = millis();
    ShiftPWM.SetHSV(random(numRGBLeds),random(360),255,255);
  }
}

void rgbLedRainbow(unsigned long cycleTime, int rainbowWidth){
  // Displays a rainbow spread over a few LED's (numRGBLeds), which shifts in hue. 
  // The rainbow can be wider then the real number of LED's.
  unsigned long time = millis()-startTime;
  unsigned long colorShift = (360*time/cycleTime)%360; // this color shift is like the hue slider in Photoshop.

  for(unsigned int led=0;led<numRGBLeds;led++){ // loop over all LED's
    int hue = ((led)*360/(rainbowWidth-1)+colorShift)%360; // Set hue from 0 to 360 from first to last led and shift the hue
    ShiftPWM.SetHSV(led, hue, 255, 255); // write the HSV values, with saturation and value at maximum
  }
}


void printInstructions(void){
  Serial.println("---- ShiftPWM Non-blocking fades demo ----");
  Serial.println("");
  
  Serial.println("Type 'l' to see the load of the ShiftPWM interrupt (the % of CPU time the AVR is busy with ShiftPWM)");
  Serial.println("");

  Serial.println(" The purpose of this program is to just fade all the LEDs in and then out");
  
  Serial.println("");
  Serial.println("Type 'm' to see this info again");  
  Serial.println("");
  Serial.println("----");
}
