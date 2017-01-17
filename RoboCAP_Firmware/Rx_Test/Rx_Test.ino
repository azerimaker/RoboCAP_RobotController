/*
 * www.Makerspace-Az.org
 * Arduino Sketch: Controller (Rx) Test proqrami
 * Kommunikasiya: nRF24L01 2.4GHz
 * son modifikasia tarixi: 18 Sentyabr 2016
 * [azerimaker]
*/

/*-----( Kitabxana elavesi)-----*/

#include <SPI.h>   // Comes with Arduino IDE
#include "RF24.h"  // Download and Install 

#define  CE_PIN  8   // The pins to be used for CE and SN
#define  CSN_PIN 10

/*-----( Declare objects )-----*/
/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus (usually) pins 7 & 8 (Can be changed) */
RF24 radio(CE_PIN, CSN_PIN);

/*-----( Declare Variables )-----*/
byte addresses[][6] = {"1Node", "2Node"}; // These will be the names of the "Pipes"
boolean debugMode = true; 

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
  Serial.begin(115200);   // MUST reset the Serial Monitor to 115200 (lower right of window )
  radio.begin();          // Initialize the nRF24L01 Radio
  radio.setChannel(108);  // 2.508 Ghz - Above most Wifi Channels
  radio.setDataRate(RF24_250KBPS); // Fast enough.. Better range

  // Set the Power Amplifier Level low to prevent power supply related issues since this is a
  // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
  // PALevelcan be one of four levels: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH and RF24_PA_MAX
  radio.setPALevel(RF24_PA_LOW);
  //radio.setPALevel(RF24_PA_MAX);

  // Open a writing and reading pipe on each radio, with opposite addresses
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);

  radio.startListening();   // Start the radio listening for data
//  radio.printDetails(); //Uncomment to show LOTS of debugging information
  delay(100);
  Serial.println(F("RoboCAP Rx Test Started..."));
}


void loop()  
{
  if ( radio.available()){
    while (radio.available())   // While there is data ready to be retrieved from the receive pipe
    {
      radio.read( &myData, sizeof(myData) );             // Get the data
    }    
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

    
//    radio.stopListening();                               // First, stop listening so we can transmit
//    radio.write( &myData, sizeof(myData) );              // Send the received data back.
//    radio.startListening();  

  }
                       
}//--(end main loop )---

