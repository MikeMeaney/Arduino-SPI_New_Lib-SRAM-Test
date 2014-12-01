#include <SPI.h>

#define SS_PIN 2
//These are the settings that SRAM likes. It was trial and 
//error to find the proper SPI_MODE
SPISettings SRAMsettings(2000000, MSBFIRST, SPI_MODE0);

//As Per the MicroChip Data Shee, these are the command 
//set for the SRAM chip
const uint8_t WRITE_BYTE = B00000010;
const uint8_t READ_BYTE =  B00000011;

void setup(){
  Serial.begin(115200);
  SPI.begin();
  pinMode(SS_PIN, OUTPUT);
}

void loop(){

  //Fill the chip with random values
  for(uint16_t extAddress = 0; extAddress < 0x7FFF; extAddress++){
    uint8_t randInt = int(random(0, 255));
    Serial.print(F("0x"));
    Serial.println(extAddress, HEX); //Print the address to be manipulated
    Serial.println(extSRAM_readByte(extAddress)); // Read it's value
    Serial.println(extSRAM_writeByte(extAddress,randInt)); // Give it a random value
    Serial.println(extSRAM_readByte(extAddress)); // Read back that random value
    Serial.println(F(" "));
    delay(100); //Slow down there turbo!
  }  

}

// External SRAM functions

//Read a byte at an address and return it's value
uint8_t extSRAM_readByte(uint16_t address){
 
 //Begin SPI Transaction
  //This is the magic sauce that makes the new SPI libray so great.
  //This means that the SRAM's SPI settings we declared at the top will
  //only be used once and then reset to defaults, or what ever settings
  //others request using SPI.beginTransaction
  SPI.beginTransaction(SRAMsettings);
  
  digitalWrite(SS_PIN, LOW); // Pull CS Low to start
  uint8_t read_byte;
  SPI.transfer(READ_BYTE);
  
  //Because there are over 32,000 addresses a 16-bit number is needed
  //problem is that the SPI.transfer() only sends 8 bits at a time
  //so what we need todo is to send half the address and then the other
  //half in two different transfers. Ditto for writing data too
  SPI.transfer((char)(address >> 8));
  SPI.transfer((char)(address));
  read_byte = SPI.transfer(0);  
  digitalWrite(SS_PIN, HIGH); //Pull CS high to end
  SPI.endTransaction();
  //SPI Transaction Ended
  
 return read_byte;
}

//Write a byte to an address. return the written value
uint8_t extSRAM_writeByte(uint16_t address, uint8_t data){ 
  //Begin SPI Transaction
  SPI.beginTransaction(SRAMsettings);
  digitalWrite(SS_PIN, LOW); // Pull CS Low to start
  SPI.transfer(WRITE_BYTE);
  SPI.transfer((char)(address>> 8));
  SPI.transfer((char)(address));
  SPI.transfer(data);
  digitalWrite(SS_PIN, HIGH); //Pull CS high to end
  SPI.endTransaction();
  //SPI Transaction ended

  //Return a read of the newly written value
  return extSRAM_readByte(address);
}
