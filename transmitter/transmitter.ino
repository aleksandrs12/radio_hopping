#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#include "AESLib.h"

RF24 radio(9, 8); // CE, CSN

const byte address[6] = "00001";
int channel = 0;
uint8_t data[64] = {2, 4, 8, 16, 32, 64, 128, 255, 69, 42, 1, 1, 2, 2, 3, 3, 2, 4, 8, 16, 32, 64, 128, 255, 69, 42, 1, 1, 2, 2, 3, 3, 2, 4, 8, 16, 32, 64, 128, 255, 69, 42, 1, 1, 2, 2, 3, 3, 2, 4, 8, 16, 32, 64, 128, 255, 69, 42, 1, 1, 2, 2, 3, 3};
const int channelCap = 30;
int channelAr[channelCap];
int channelArTemplate[channelCap];
int seed = 30001;
unsigned int arraysShuffled = 0;
unsigned int channelID = 0;

uint8_t firstInt = 133;

uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};


void shuffle_array(int *array, int size, int random_number, int *arrayT) {
    srand(random_number); // Seed the random number generator
    for (int i = 0; i < size; i++){
      array[i] = arrayT[i];
    }
    
    // Fisher-Yates Shuffle
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
    /*
    Serial.println();
    for (int i = 0; i < size; i++){
      Serial.print(array[i]);
      Serial.print(" ");
    }
    */
    
    
}


void setup() {
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  for (int i = 0; i < channelCap; i++){
      channelAr[i] = i;
      channelArTemplate[i] = i;
  }
  

  Serial.begin(9600);
  for (int i = 0; i < channelCap; i++){
    Serial.print(channelArTemplate[i]);
    Serial.print(" ");
  }
  
  radio.begin();
  radio.powerUp();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.setRetries(0, 0);
  radio.setAutoAck(1);
  radio.stopListening();
  pinMode(2, INPUT);
  pinMode(3, INPUT);

  //crypt
  aes_init(); // generate random IV, should be called only once? causes crash if repeated...

}

void loop() {
  if (channelID >= channelCap){
    arraysShuffled++;
    channelID = 0;
    shuffle_array(channelAr, channelCap, seed - arraysShuffled, channelArTemplate);

    //Serial.println(arraysShuffled);
    
    
  }

  data[0] = firstInt;

  //uint8_t lowByte = (byte) (65533 & 0xff);
  //uint8_t highByte = (byte) ((65533 >> 8) & 0xff);
  data[1] = (byte) (arraysShuffled & 0xff);
  data[2] = (byte) ((arraysShuffled >> 8) & 0xff)

  data[3] = uint8_t(channelAr[channelID]);

  data[4] = uint8_t(digitalRead(2));
  data[5] = uint8_t(digitalRead(3));
  
  radio.setChannel(channelAr[channelID]);
  radio.write(&data, sizeof(data));
  //Serial.println(channelAr[channelID]);
  channelID++;
  
}











//                                    ENCRYPTION CODE
// Generate IV (once)
void aes_init() {
  aesLib.gen_iv(aes_iv);
  aesLib.set_paddingmode((paddingMode)0);
}

uint16_t encrypt_to_ciphertext(char * msg, uint16_t msgLen, byte iv[]) {
  Serial.println("Calling encrypt (string)...");
  // aesLib.get_cipher64_length(msgLen);
  int cipherlength = aesLib.encrypt((byte*)msg, msgLen, (byte*)ciphertext, aes_key, sizeof(aes_key), iv);
                   // uint16_t encrypt(byte input[], uint16_t input_length, char * output, byte key[],int bits, byte my_iv[]);
  return cipherlength;
}

uint16_t decrypt_to_cleartext(byte msg[], uint16_t msgLen, byte iv[]) {
  Serial.print("Calling decrypt...; ");
  uint16_t dec_bytes = aesLib.decrypt(msg, msgLen, (byte*)cleartext, aes_key, sizeof(aes_key), iv);
  Serial.print("Decrypted bytes: "); Serial.println(dec_bytes);
  return dec_bytes;
}

AESLib aesLib;

#define INPUT_BUFFER_LIMIT (128 + 1) // designed for Arduino UNO, not stress-tested anymore (this works with readBuffer[129])

unsigned char cleartext[INPUT_BUFFER_LIMIT] = {0}; // THIS IS INPUT BUFFER (FOR TEXT)
unsigned char ciphertext[2*INPUT_BUFFER_LIMIT] = {0}; // THIS IS OUTPUT BUFFER (FOR BASE64-ENCODED ENCRYPTED DATA)

unsigned char readBuffer[18] = "username:password";

// AES Encryption Key (same as in node-js example)
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// General initialization vector (same as in node-js example) (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };

unsigned long loopcount = 0;

// Working IV buffer: Will be updated after encryption to follow up on next block.
// But we don't want/need that in this test, so we'll copy this over with enc_iv_to/enc_iv_from
// in each loop to keep the test at IV iteration 1. We could go further, but we'll get back to that later when needed.

// General initialization vector (same as in node-js example) (you must use your own IV's in production for full security!!!)
byte enc_iv[N_BLOCK] =      { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
byte enc_iv_to[N_BLOCK]   = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
byte enc_iv_from[N_BLOCK] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };




















