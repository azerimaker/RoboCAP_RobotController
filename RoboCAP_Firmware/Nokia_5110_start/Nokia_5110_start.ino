// Nokia 5110 Screen test code (without library) for RoboCAP Tx

// 84 x 48
#define RST 6
#define CE 10
#define DC 9
#define DIN 11
#define CLK 13

void LcdWriteCmd(byte cmd)
{
//digitalWrite(DC, LOW); //DC pin is low for commands
digitalWrite(CE, LOW); // active low
shiftOut(DIN, CLK, MSBFIRST, cmd); //transmit serial data
digitalWrite(CE, HIGH);
}

void LcdWriteData(byte data)
{
digitalWrite(DC, HIGH);     //DC pin is high for data
digitalWrite(CE, LOW);      // active low
shiftOut(DIN, CLK, MSBFIRST, data); //transmit serial data
digitalWrite(CE, HIGH);
}


void setup()
{
pinMode(RST, OUTPUT);
pinMode(CE, OUTPUT);
pinMode(DC, OUTPUT);
pinMode(DIN, OUTPUT);
pinMode(CLK, OUTPUT);
digitalWrite(RST, LOW);   // active low
digitalWrite(RST, HIGH); 

digitalWrite(DC, LOW); //DC pin is low for commands

LcdWriteCmd(0x21); // LCD extended commands
LcdWriteCmd(0xB8); // set LCD Vop (contrast)
//LcdWriteCmd(0x20); // LCD normal commands
LcdWriteCmd(0x04); // set temp coefficent
LcdWriteCmd(0x14); // LCD bias mode 1:40
LcdWriteCmd(0x20); // LCD basic commands
LcdWriteCmd(0x09); // LCD all segments on
LcdWriteCmd(0x0C); // display control set, normal video mode

for(int i = 0; i < 42; i++){
    LcdWriteData(0xAA); // AA, 55
    delay(50);
    LcdWriteData(0x55);
}
for(int i = 0; i < 42; i++){
    LcdWriteData(0xFF); 
    delay(50);
    LcdWriteData(0x00);
}
for(int i = 0; i < 42; i++){
    LcdWriteData(0xAA); // AA, 55
    delay(50);
    LcdWriteData(0x55);
}
for(int i = 0; i < 42; i++){
    LcdWriteData(0xFF); 
    delay(50);
    LcdWriteData(0x00);
}
for(int i = 0; i < 42; i++){
    LcdWriteData(0xAA); // AA, 55
    delay(50);
    LcdWriteData(0x55);
}

for(int i = 0; i < 42; i++){
    LcdWriteData(0xAA); // AA, 55
    delay(50);
    LcdWriteData(0x55);
}



}

void loop()
{
}
