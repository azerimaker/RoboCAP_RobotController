/* 
 * www.Makerspace-Az.org
 * by [azerimaker]
 * ---------------------------------------------------------------------------- 
 * [az]/([en])
 * Arduino Sketch: Controller (Tx) Test proqrami
 * Kommunikasiya (Communication methods): nRF24L01+ / Smartphone + Bluetooth (HC-06)
 * Son modifikasia (last modified): 17.01.2017
 * Melumat paketi gonderilme tezliyi: -70 Hz
 * ----------------------------------------------------------------------------
 * 
*/

/*----- Lazimi Kitabxanalar (Required Libraries)-----*/
#include <SPI.h>   
#include "RF24.h"  // Download link --> https://github.com/TMRh20/RF24

/*----------------------------------------------------------------------------*/

#define  CE_PIN  8   // The pins to be used for CE and SN
#define  CSN_PIN 10

#define JOYSTICK_X   A0  // The Joystick potentiometers connected to Arduino Analog inputs
#define JOYSTICK_Y   A1
#define JOYSTICK_SW  A2  // The Joystick push-down switch, will be used as a Digital input

#define TOP_LEFT_BTN A6
#define TOP_RIGT_BTN 3
#define BOTTOM_LEFT_BTN 4
#define BOTTOM_RIGHT_BTN 5


/*-----( Declare objects )-----*/
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus (usually) pins 7 & 8 (Can be changed) */
RF24 radio(CE_PIN, CSN_PIN);

/*-----( Declare Variables )-----*/
byte addresses[][6] = {"1Node", "2Node"}; // These will be the names of the "Pipes"

unsigned long timeNow;  // Used to grab the current time, calculate delays
unsigned long started_waiting_at;
boolean timeout;       // Timeout? True or False
boolean debugMode = true; 
unsigned long counter = 0;

struct dataStruct {
  unsigned long count;  // to save response times
  int Xposition;          // The Joystick position values
  int Yposition;
  bool switchOn;          // The Joystick push-down switch
  bool TL_BTN; 
  bool TR_BTN;
  bool BL_BTN;
  bool BR_BTN;
} myData;                 


void setup()  
{
  Serial.begin(115200);  // MUST reset the Serial Monitor to 115200 (lower right of window )

  pinMode(JOYSTICK_SW, INPUT_PULLUP);
  pinMode(TOP_LEFT_BTN, INPUT);
  pinMode(TOP_RIGT_BTN, INPUT_PULLUP);
  pinMode(BOTTOM_LEFT_BTN, INPUT_PULLUP);
  pinMode(BOTTOM_RIGHT_BTN, INPUT_PULLUP);

  radio.begin();          // Initialize the nRF24L01 Radio
  radio.setChannel(108);  // Above most WiFi frequencies
  radio.setDataRate(RF24_250KBPS); // Fast enough.. Better range

  // Set the Power Amplifier Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  // PALevelcan be one of four levels: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
  //radio.setPALevel(RF24_PA_LOW);
  radio.setPALevel(RF24_PA_HIGH);
  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[0]);  // Oturucu Pipe (kanali) ishe sal
  radio.openReadingPipe(1, addresses[1]); // Qebul Pipe (kanali) ishe sal
//  radio.startListening();

//  radio.printDetails(); //Uncomment to show LOTS of debugging information
  
  delay(100);
  Serial.println(F("RoboCAP Tx Test Started..."));
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
//	radio.stopListening();                                    // First, stop listening so we can talk.

	/*********************( Read Controller Buttons and Joystick )***********************/
	myData.Xposition = analogRead(JOYSTICK_X);
	myData.Yposition = analogRead(JOYSTICK_Y);
	myData.switchOn  = !digitalRead(JOYSTICK_SW);  // Invert the pulldown switch

	int A6_value = analogRead(TOP_LEFT_BTN); // A6 Analog IN only
	if(A6_value >0){myData.TL_BTN = 0;}
	else{myData.TL_BTN = 1;}

	myData.TR_BTN = !digitalRead(TOP_RIGT_BTN);
	myData.BL_BTN = !digitalRead(BOTTOM_LEFT_BTN);
	myData.BR_BTN = !digitalRead(BOTTOM_RIGHT_BTN);
	myData.count = counter++;  

  if(!radio.write( &myData, sizeof(myData) )) {  // Send data, checking for error ("!" means NOT)
    Serial.println(F("Transmit failed "));
  }
  
	/*********************(Print ALL if debugMode == true)********************/
	if(debugMode == true){
    Serial.print(F("Count: "));
    Serial.print(myData.count);
		Serial.print(F(" X: "));
		Serial.print(myData.Xposition);
		Serial.print(F(" Y: "));
		Serial.print(myData.Yposition);
		Serial.print(F(" SW: "));
		Serial.print(myData.switchOn);
		Serial.print(F(" A6: "));
		Serial.print(myData.TL_BTN);
		Serial.print(F(" D3: "));
		Serial.print(myData.TR_BTN);
		Serial.print(F(" D4: "));
		Serial.print(myData.BL_BTN);
		Serial.print(F(" D5: "));
		Serial.println(myData.BR_BTN);
	}

	//radio.startListening();                                    // Now, continue listening
  //radio.read( &myData, sizeof(myData) );
  delay(10);
}



