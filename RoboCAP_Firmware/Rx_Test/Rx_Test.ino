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


struct dataStruct {
  unsigned long micro_sec;  // to save response times
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

  // Start the radio listening for data
  radio.startListening();
//  radio.printDetails(); //Uncomment to show LOTS of debugging information
}


void loop()  
{
  if ( radio.available()){
    while (radio.available())   // While there is data ready to be retrieved from the receive pipe
    {
      radio.read( &myData, sizeof(myData) );             // Get the data
    } 

    radio.stopListening();                               // First, stop listening so we can transmit
    radio.write( &myData, sizeof(myData) );              // Send the received data back.
    radio.startListening();                              // Now, resume listening so we catch the next packets.

   // Serial.print(F("Packet Received - Sent response "));  // Print the received packet data
   // Serial.print(myData.micro_sec);
   // Serial.println(F("uS"));
    
    Serial.print("X: ");
    Serial.print(myData.Xposition);
    Serial.print(", Y: ");
    Serial.print(myData.Yposition);
    Serial.print(" ,SW: ");
    Serial.print(myData.switchOn);
    Serial.print(" ,A6: ");
    Serial.print(myData.TL_BTN);
    Serial.print(" , D3: ");
    Serial.print(myData.TR_BTN);
    Serial.print(" , D4: ");
    Serial.print(myData.BL_BTN);
    Serial.print(" , D5: ");
    Serial.println(myData.BR_BTN);

  } // END radio available
 
}//--(end main loop )---

