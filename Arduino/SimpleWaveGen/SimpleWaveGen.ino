#include <AD9833.h>     // Include AD9833 library (LINK: https://github.com/Billwilliams1952/AD9833-Library-Arduino/tree/master)

#include <EasyButton.h> // Include EasyButton library (LINK: https://github.com/Billwilliams1952/AD9833-Library-Arduino/tree/master)

#include <Wire.h> // Include I2C library

#include <Adafruit_SSD1306.h> // Include SSD1306 library (LINK: https://github.com/adafruit/Adafruit_SSD1306)

#define FNC_PIN A3       // Can be any digital IO pin, used for AD9833

#define SCREEN_ADDRESS 0x3C // Address: 0x3D for 128x64, 0x3C for 128x32

Adafruit_SSD1306 display(128, 32, &Wire, -1); // Initialize Screen: X size, Y size, I2C, reset (see Adafruit library) 

const byte pinA = 4, pinB = 5, encoderBPin = 6, leftBPin = 7, rightBPin = 8, modeBPin = 9; //Encoder pin a and b, and pins of the various buttons

bool aVal = 0, aLast = 0, update = 0; //aVal and aLast used for rotary encoder reading, update used for waveform update
byte mode = 0, pos = 0; //mode used for the types of waveforms, pos is the position of the digit we are working on

int encoder = 0; //encoder variable

byte digit[] = {0, 0, 0, 1, 0, 0}; //digits stored on array
String stringMode[] = {"Square", "Triangular", "Sine"}; //Arrsy used to print waveform type on SSD1306 based on mode

unsigned long freq = 1000; //frequency

EasyButton encoderButton(encoderBPin); //Initialize the various buttons connected to respective pins
EasyButton leftButton(leftBPin);
EasyButton rightButton(rightBPin);
EasyButton modeButton(modeBPin);

AD9833 gen(FNC_PIN); //Initialize AD9833

void setup() {

  Wire.begin(); //I2C begin

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { //If display is not connected loop forever
    for(;;); 
  }

  display.clearDisplay(); //Clear display
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setTextSize(2); //Text size

  pinMode(4, INPUT); //Encoder pin a and b as INPUT so we can read when it's rotating
  pinMode(5, INPUT);

  aLast = digitalRead(pinA); //Assign latest value to aLast

  encoderButton.onPressed(digitResest); //When one of the button is pressed do respective function (see various functions below)
  leftButton.onPressed(left);
  rightButton.onPressed(right);
  modeButton.onPressed(modeChange);


  gen.Begin(); //Used to enable 
  gen.EnableOutput(true);   // Turn on output

  updateDisplay(); //Update display so we can see the writing on the display at the beginning
  gen.ApplySignal(SQUARE_WAVE, REG0, freq); //output signal at the beginning so we can see if it's all OK. 1kHz square wave as standard

}

void loop() {

  if(update) { //Any time we need to update the waveform we need to:

    freq = digit[0] + digit[1] * 10UL + digit[2] * 100UL + digit[3] * 1000UL + digit[4] * 10000UL + digit[5] * 100000UL; //Set the frequency
    
    if(mode == 0)       gen.ApplySignal(SQUARE_WAVE, REG0, freq); //Apply waveform type based on mode
    else if(mode == 1)  gen.ApplySignal(TRIANGLE_WAVE, REG0, freq);
    else                gen.ApplySignal(SINE_WAVE, REG0, freq);

    update = 0; //No more need to update

  }

  
  encoderButton.read(); //Read all the 4 buttons
  leftButton.read();
  rightButton.read();
  modeButton.read();

  readEncoder(); //Read any change in encoder position

}

void modeChange() { //Mode change function

  if(mode < 2) mode++; //We have 3 modes, Square, Triangle, Sine. So we increase only up to 2 (0, 1, 2)
  else mode = 0; //If mode is 2, we return to 0 instead of increasing mode

  updateDisplay(); //Every time mode is changed we need to update the display

  update = 1; //Every time mode is changed we need to update waveform output on AD9833

}

void left() { //Change position to the left function

  if(pos < 5) pos++; //If the position is less than 5 we can go to the left (0, 1, 2, 3, 4, 5)
  else pos = 0; //If the position is 5, we cannot go further to the left so we return to the 0 (first) digit

  encoder = digit[pos] * 2; //Each time we change position, the encoder position needs to be matched to the digit

  updateDisplay(); //Every time we change position we need to update position counter

}

void right() { //Change position to the right function

  if(pos > 0 && pos <= 5) pos--; //If the position is greater than 0 we can go to the right (5, 4, 3, 2, 1, 0)
  else pos = 5; //If position is less than 0 we cannot go further to the right so we return to the 5 (last) digit

  encoder = digit[pos] * 2; //Each time we change position, the encoder position needs to be matched to the digit

  updateDisplay(); //Every time we change position we need to update position counter

}

void digitResest() { //Digit reset function

  digit[pos] = 0; //Reset the digit at the correct position

  updateDisplay(); //Every time we reset the digit we need to update frequency

  update = 1; //Every time we reset the digit we need to update frequency output

}

void readEncoder() { //Encoder read function
 
 aVal = digitalRead(pinA); //Assign the value based on the readings
 
 if (aVal != aLast){  //If we fid that the value has changed then:

  if (digitalRead(pinB) != aVal) encoder--; //If we rotate counterclockwise we lower encoder counter
  else encoder++; //If we rotate clockwise we increase counter

  if(encoder > 19) encoder = 0; //If we rotate clocwise too much clockwise we don't want our dugit to go over encoder/2 or 19 (truncated by conversion), so we start back to 0
  else if(encoder < 0) encoder = 19; //If we rotate too much counterclockwise we don't want our digit to go under 0, so we start back to 19

  encoder = constrain(encoder, 0, 19); //Constrain encoder for better results

  digit[pos] = encoder / 2; //The digit needs to be encoder / 2 or from 0 to 9

  updateDisplay(); //Every time we change digit value frequency needs to be updated

  update = 1; //Every time we change digit value we need to update frequency output

 }
 
 aLast = aVal; //aLast now it's aVal so that we can see changes next time

}


void updateDisplay() { //Update display function

  display.clearDisplay(); //We clear all the display
  display.setCursor(0, 0); //Set the cursor to top-left corner
  for(int i = 0; i < 6; i++) display.print(digit[5-i]); //Display digits from the greatest to the lowest from left to right
  display.print(" P:"); //Print the position
  display.print(pos);
  display.setCursor(0, 16); //Set cursor to match "the second line"
  display.print(stringMode[mode]); //Print the waveform type based on the mode
  display.display(); //Save and update changes to display

}

