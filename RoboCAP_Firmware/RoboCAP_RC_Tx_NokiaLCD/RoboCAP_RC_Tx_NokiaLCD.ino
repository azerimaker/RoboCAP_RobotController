/*
 * www.Makerspace-Az.org
 * Arduino Sketch: 4X4 Robot masin, Radio Transmitter (Tx)
 * Kommunikasiya: nRF24L01 2.4GHz
 * son modifikasia tarixi: 16 May 2016
 * [azerimaker]
*/

/*-----( Kitabxana elavesi)-----*/
#include <SPI.h>
#include <RF24.h>

/*-----(  Sabitler ve Pin Nomreleri )-----*/
//nRF24L01
#define nRF_CE_PIN   8
#define nRF_CSN_PIN 10

// Joystick
#define JOYSTICK_X   A1  // Joystick VRx Analoq pin A0-a baglanir
#define JOYSTICK_Y   A0  // Joystick VRy Analoq pin A1-e baglanir
#define JOYSTICK_SW  A2  //  Joystick merkeze basildiqda achar
// Buttons
#define TOP_LEFT_BTN A6
#define TOP_RIGT_BTN 3
#define BOTTOM_LEFT_BTN 4
#define BOTTOM_RIGHT_BTN 5

#define VBAT_SENSE A3


/*-----( Radio Obyektinin yaradilmasi )-----*/
RF24 radio(nRF_CE_PIN, nRF_CSN_PIN); 
#define LCD_C     LOW
#define LCD_D     HIGH

#define LCD_X     84
#define LCD_Y     48


/*-----( Deyishenler )-----*/
byte addresses[][6] = {"1Node", "2Node"}; //  "Pipe" adi, nrf24l01 uchun

unsigned long timeNow;  // Used to grab the current time, calculate delays
unsigned long started_waiting_at;
boolean timeout;       // Timeout? True or False

// Proqramla Analoq qiymetleri filterleme uchun deyishenler
int X_qiymeti = 0;
int Y_qiymeti = 0;        
float X_exp_emsal = 1;      // exponensial emsal qiymeti, 1-olduqda filtirleme deaktiv olur
float Y_exp_emsal = 1;      // bu emsali isteyinize gore deyishe bilersiniz (0-1] aralighinda
int X_exp_cem = 0;            // exponensial cemin bashlangic qiymeti
int Y_exp_cem = 0;     

// Joystick qiymetleri olan deyishenler strukturu

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


 /****** SETUP: Bir defe icra olunacaq ******/
void setup()  
{
  Serial.begin(115200);
  pinMode(JOYSTICK_SW, INPUT_PULLUP);  // Kenar rezistor ishletmeye ehtiyac yoxdu
                                      //  daxili Pull-up rezistorunu ishe saliriq
  pinMode(TOP_LEFT_BTN, INPUT);       // Analog pin oldughu ucun PULLUP yoxdur
  pinMode(TOP_RIGT_BTN, INPUT_PULLUP);
  pinMode(BOTTOM_LEFT_BTN, INPUT_PULLUP);
  pinMode(BOTTOM_RIGHT_BTN, INPUT_PULLUP);            
  
  radio.begin();          // nRF24L01 Radio transiveri ishe sal
  radio.setChannel(108);  //  WiFi siqnallarindan yuksek bir kanal deyeri sech
  radio.setDataRate(RF24_250KBPS); // Data gonderme sureti
  radio.setPALevel(RF24_PA_HIGH); // Oturme gucu: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
 
  radio.openWritingPipe(addresses[0]); // Gonderme Pipe (kanali) ishe sal
  radio.openReadingPipe(1, addresses[1]);
  radio.startListening();
//  radio.printDetails(); //kommenti sileniz coxlu Debug informasiyasi print olunacaq

  // Filterleme ucun bashlangic hesablama
  X_exp_cem =  analogRead(JOYSTICK_X); 
  Y_exp_cem =  analogRead(JOYSTICK_Y); 


  
  delay(100);
  
} /****** SETUP sonu ******/


 /****** LOOP: Daimi tekrarlanir ******/
void loop()  
{

  /*********************( Read Controller Buttons and Joystick )***********************/
  X_qiymeti = analogRead(JOYSTICK_X);
  Y_qiymeti = analogRead(JOYSTICK_Y);
  myData.switchOn  = !digitalRead(JOYSTICK_SW);  // Achar basildiqda 0V oldughu ucun, inversiya etmeliyik

  int A6_value = analogRead(TOP_LEFT_BTN); // A6 Analog IN only
  if(A6_value >0){
    myData.TL_BTN = 0;
  }else{
    myData.TL_BTN = 1;
   }

  myData.TR_BTN = !digitalRead(TOP_RIGT_BTN);
  myData.BL_BTN = !digitalRead(BOTTOM_LEFT_BTN);
  myData.BR_BTN = !digitalRead(BOTTOM_RIGHT_BTN);
  myData.micro_sec = micros();  // Send back for timing


/* 
 * Bu emeliyyat zamani X_exp_emsal qiymeti ne qeder kicik olarsa, 
 * yeni oxunan X_qiymeti yekun X_exp_cem qiymetine bir o qeder az 
 * tesir edecek, neticede sert kecidler, yumshaq olacaq
 * 
 */
  X_exp_cem = (X_exp_emsal * X_qiymeti) + ((1-X_exp_emsal)*X_exp_cem); // Exponensial filterleme emeliyyati
  Y_exp_cem = (Y_exp_emsal * Y_qiymeti) + ((1-Y_exp_emsal)*Y_exp_cem); 
        
  myData.Xposition = X_exp_cem; // Filter olunmush qiymetleri umumi struktura yaz
  myData.Yposition = Y_exp_cem;

  // Bu Serial Print-leri komment ede bilersiniz. 
  Serial.print("Indi gonderilir, X= ");
  Serial.print(myData.Xposition);
  Serial.print(", Y= ");
  Serial.print(myData.Yposition);
  if ( myData.switchOn == 1)
  {
      Serial.println(", Achar ON");
  }else
  {
      Serial.println(", Achar OFF");
  }

  if(!radio.write( &myData, sizeof(myData) )) {  // Melumati gonder ve xeta olarsa print
     Serial.println("Gonderilme ugursuz oldu");
  }

  radio.startListening();                                    // Now, continue listening
  started_waiting_at = micros();               // timeout period, get the current microseconds
  timeout = false;                            //  variable to indicate if a response was received or not

  while ( !radio.available() ){                            // While nothing is received
    if (micros() - started_waiting_at > 200000 ){           // If waited longer than 200ms, indicate timeout and exit while loop
      timeout = true;
      break;
      }
  }

  if ( timeout ){ // Describe the results
    Serial.println(F("Response timed out -  no Acknowledge.")); // passing flash-memory based strings to save SRAM
  }else
  {
    // Grab the response, compare, and send to Serial Monitor
    radio.read( &myData, sizeof(myData) );
    timeNow = micros();

    Serial.print(F("Sent "));
    Serial.print(timeNow);
    Serial.print(F(", Got response "));
    Serial.print(myData.micro_sec);
    Serial.print(F(", Round-trip delay "));
    Serial.print(timeNow - myData.micro_sec);
    Serial.println(F(" microseconds "));

  }

  
  delay(50); // Delay olmasi stabilliyi artirir. 
  

} /****** LOOP sonu ******/



