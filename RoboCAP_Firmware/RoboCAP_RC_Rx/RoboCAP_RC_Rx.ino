/*
 * www.Makerspace-Az.org
 * Arduino Sketch: 4X4 Robot masin, Radio Receiver Robot Control (Rx)
 * Kommunikasiya: nRF24L01 2.4GHz
 * son modifikasia tarixi: 21 Iyul 2016
 * [azerimaker]
*/

/*-----( Kitabxana elavesi)-----*/
#include <SPI.h>
#include <RF24.h>
//#include <SoftwareSerial.h>


// eyni anda yalniz bir rejimi aktivlesdirin
//#define BLUETOOTH_MODE
//#define nRF_MODE


/*-----(  Sabitler ve Pin Nomreleri )-----*/
#define MAX_SPEED 255 // analogWrite maks qiymeti

// nRF24L01 modulu uchun
#define CE_PIN   8  
#define CSN_PIN 10

// L298n Motor Surucu pinleri
#define ENA A2      // Motor A Enable 
#define motor1A 9     // INA 
#define motor1B 6     // INB
#define motor2A 5     // INC
#define motor2B 3     // IND
#define ENB A3		  // Motor B Enable

// HC-06 Bluetooth Modul pinleri
//#define Rx 4          // -> HC-06 TxD    
//#define Tx 7 		  // -> HC-06 RxD

#define X_min 470
#define X_max 530
#define Y_min 470
#define Y_max 530

 // deyishenlar ve sabitler
//char BT_RECEIVED;   // BT-dan gelen simvol
//char PC_RECEIVED;   // PC-den gelen simvol

/*-----( Radio Obyektinin yaradilmasi )-----*/
RF24 radio(CE_PIN, CSN_PIN);
// Hardware serial PC ile kommunikasiya ucun lazim oldughu ucun SoftwareSerial ishledirik
//SoftwareSerial BTserial(Rx, Tx);

/*-----( Funksiya Prototipleri )-----*/
void calcDirection(int X_pos, int Y_pos);
void calcSpeed(int X_pos, int Y_pos);
void driveRobot(char DIRECTION, unsigned char SPEED);
void setSpeedRight(unsigned char SPEED, boolean DIR);
void setSpeedLeft(unsigned char SPEED, boolean DIR);
void motorTormozla(void);

/*-----( Deyishenler )-----*/
byte addresses[][6] = {"1Node", "2Node"}; //  nRF ucun "Pipe" adlari


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
               

char Joystick_Dir;     // Joystick kordinatlarini hereket istiqametine ve 
unsigned char Joystick_Speed = 0;   // surete cevirmek
int X_map, Y_map;
unsigned int X_sq, Y_sq;     // suret vektorunun hesablamaq uchun

boolean FORWARD = true; // ireli
boolean BACKWARD = false; // geri

/****** SETUP: Bir defe icra olunacaq ******/                       
void setup() 
{
  Serial.begin(115200);
  
  pinMode(motor1A, OUTPUT);
  pinMode(motor1B, OUTPUT);
  pinMode(motor2A, OUTPUT);
  pinMode(motor2B, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  // L298n aktivlesdir 
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  motorTormozla(); // tekerleri tormozla, ehtiyat ucun
  
  //BTserial.begin(9600);
  radio.begin();  
  radio.setChannel(108);  //   2.508 Ghz - WiFi siqnallarindan yuksek bir kanal deyeri sech
  radio.setDataRate(RF24_250KBPS); // Data gonderme sureti
  radio.setPALevel(RF24_PA_LOW); // Oturme gucu: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX

  radio.openWritingPipe(addresses[1]); // Oturucu Pipe (kanali) ishe sal
  radio.openReadingPipe(1, addresses[0]); // Qebul Pipe (kanali) ishe sal
  
  radio.startListening(); // Radio siqnallari dinlemeye bashla
  //radio.printDetails(); //kommenti sileniz coxlu Debug informasiyasi print olunacaq

} /****** SETUP sonu ******/


// implement MODE selection

 /****** LOOP: Daimi tekrarlanir ******/
void loop()
{
   if ( radio.available()){		
    while (radio.available())   // Qebul Pipe-da melumat olarsa
    {
      radio.read( &myData, sizeof(myData) ); // melumati oxu
    }
    //failsafe_count = 0; 
    radio.stopListening();                               // First, stop listening so we can transmit
    radio.write( &myData, sizeof(myData) );              // Send the received data back.
    radio.startListening();                              // Now, resume listening so we catch the next packets.
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

    // Yeni qebul olunmush melumati robotun hereket sxemine uyghunlashdir
    calcDirection(myData.Xposition, myData.Yposition); // S,F,B,L,R,I,G,H,J
    calcSpeed(myData.Xposition, myData.Yposition);  // 0-255
    driveRobot(Joystick_Dir, Joystick_Speed); 
  }  // END radio available
  
}  /****** LOOP sonu ******/

 /*
 * Bu funksiya X,Y qiymetlerine esasen qlobal 
 * Istiqamet deyishenini yenileyir 
 */ 

 void calcDirection(int X_pos, int Y_pos){
  
  if(X_pos >= X_min && X_pos <= X_max && Y_pos > Y_max)
  {
    Joystick_Dir = 'F';
  }
  else if(X_pos >= X_min && X_pos <= X_max && Y_pos < Y_min)
  {
    Joystick_Dir = 'B';
  }
  else if(Y_pos >= Y_min && Y_pos <= Y_max && X_pos < X_min)
  {
    Joystick_Dir = 'L';
  }
  else if(Y_pos >= Y_min && Y_pos <= Y_max && X_pos > X_max)
  {
    Joystick_Dir = 'R';
  }
  else if(X_pos > X_max && Y_pos > Y_max)
  {
    Joystick_Dir = 'I';
  }
  else if(X_pos < X_min && Y_pos > Y_max)
  {
    Joystick_Dir = 'G';
  }
  else if(X_pos < X_min && Y_pos < Y_min)
  {
    Joystick_Dir = 'H';
  }
  else if(X_pos > X_max && Y_pos < Y_min)
  {
    Joystick_Dir = 'J';
  }
  else{
      Joystick_Dir = 'S'; // deadzone
  }
}



/*
 * Bu funksiya X,Y qiymetlerine esasen qlobal 
 * Suret deyishenini yenileyir 
 */ 

 void calcSpeed(int X_pos, int Y_pos){

  X_map = map(X_pos, 0, 1023, -100, 100); // intervali kicilt
  Y_map = map(Y_pos, 0, 1023, -100, 100);
  
  X_map = abs(X_map); // musbet edede cevir
  Y_map = abs(Y_map);

  X_sq = sq(X_map); // kvadrata yukselt
  Y_sq = sq(Y_map);

  Joystick_Speed = (unsigned char)sqrt(X_sq + Y_sq); // Joystick suret vektorunu hesabla, Pifaqor teoremi :)
  if(Joystick_Speed>100){
    Joystick_Speed = 100;
  }
}

/*
 * Bu funksiya suret ve istiqamet deyishenine esasen motor suretini
 * teyin edir 
 */ 


void driveRobot(char DIRECTION, unsigned char SPEED)
{ 
  
  byte SET_SPEED = map(SPEED, 0,100, 0, MAX_SPEED);// SPEED 0-100, intervali deyish 0-255
  byte SPEED_MIX = SET_SPEED/2;
  
  switch(DIRECTION){
      case 'F': // Forward
          setSpeedRight(SET_SPEED, FORWARD);
          setSpeedLeft( SET_SPEED, FORWARD);
          break;
      case 'B': // Backward
          setSpeedRight(SET_SPEED, BACKWARD);
          setSpeedLeft( SET_SPEED, BACKWARD);
          break;
      case 'R': // Right
          setSpeedRight(SPEED_MIX, FORWARD);
          setSpeedLeft( SPEED_MIX, BACKWARD);
          break;
      case 'L': // Left
          setSpeedRight(SPEED_MIX, BACKWARD);
          setSpeedLeft( SPEED_MIX, FORWARD);
          break;
      case 'I': // Right + Forward
          setSpeedRight(SET_SPEED, FORWARD);
          setSpeedLeft( SPEED_MIX, FORWARD);
            break;
      case 'G': // Left + Forward
          setSpeedRight(SPEED_MIX, FORWARD);
          setSpeedLeft( SET_SPEED, FORWARD);
          break;
      case 'J': // Right + Backward
          setSpeedRight(SET_SPEED, BACKWARD);
          setSpeedLeft( SPEED_MIX, BACKWARD);
          break;  
      case 'H': // Left + Backward 
          setSpeedRight(SPEED_MIX, BACKWARD);
          setSpeedLeft( SET_SPEED, BACKWARD);
          break; 
      default: 
            motorTormozla(); // istenilen bashqa simvol olduqda tekerleri tormozla
            break;
    }
}


/*
 * sag ve sol tekerlerin suretini ve istiqametini 
 * teyin eden funksiyalar
 */ 
 
void setSpeedRight(unsigned char SPEED, boolean DIR)
{
  if(DIR == true){
      digitalWrite(motor2A, LOW);
      analogWrite (motor2B, SPEED);
  }
  else{
      analogWrite(motor2A, SPEED);
      digitalWrite(motor2B, LOW);
  }
  
}

void setSpeedLeft(byte SPEED, boolean DIR)
{
  if(DIR == true){
      digitalWrite(motor1A, LOW);
      analogWrite (motor1B, SPEED);
  }
  else{
      analogWrite(motor1A, SPEED);
      digitalWrite(motor1B, LOW);   
  }
  
}

void motorTormozla(void)
{
    digitalWrite(motor1A, LOW);
    digitalWrite(motor1B, LOW);
    digitalWrite(motor2A, LOW);
    digitalWrite(motor2B, LOW);
}


