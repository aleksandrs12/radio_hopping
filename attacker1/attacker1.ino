#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 8); // CE, CSN

const byte address[6] = "00001";
int channel = 0;
int data[64] = {20009 ,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000,10000};


void setup() {
  

  Serial.begin(9600);
  radio.begin();
  radio.powerUp();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.setRetries(0, 0);
  radio.setAutoAck(1);
  radio.stopListening();
  radio.setChannel(15);
}

void loop() {
  
  
  radio.write(&data, sizeof(data));
  
}




















