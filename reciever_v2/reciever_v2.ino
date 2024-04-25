
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00001";
int data[32];
int lastValue = 0;
int dataLost = 0;

const int channelCap = 40;
int channelAr[channelCap];
int channelArTemplate[channelCap];
int seed = 496219323000;
int arraysShuffled = 0;
int channelID = 0;


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
}

void setup() {
  for (int i = 0; i < channelCap; i++){
    channelAr[i] = i;
    channelArTemplate[i] = i;
  }
  Serial.begin(9600);
  radio.begin();
  radio.powerUp();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  radio.setChannel(0);
}
long t = micros();

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));
    for (int i = 0; i < 5; i++){
      Serial.print(data[i]);
      Serial.print(" ");
    }
    Serial.println(" ");


      if (channelID != data[0]){
        channelID = data[0];
      }
      if (arraysShuffled != data[1]){
        arraysShuffled = data[1];
        shuffle_array(channelAr, channelCap, seed - arraysShuffled, channelArTemplate);
        Serial.println(arraysShuffled);
        Serial.println(" ");
        for (int i = 0; i < channelCap; i++){
          Serial.print(channelAr[i]);
          Serial.print(" ");
        }
        
        //Serial.println();
        //Serial.println(arraysShuffled);
      }
      channelID++;
      if (channelID >= channelCap){
        arraysShuffled++;
        channelID = 0;
        shuffle_array(channelAr, channelCap, seed - arraysShuffled, channelArTemplate);
        
        for (int i = 0; i < channelCap; i++){
          Serial.print(channelAr[i]);
          Serial.print(" ");
        }
        
        //Serial.println();
        //Serial.println(arraysShuffled);
        
        
      }
 
    t = micros();

    delayMicroseconds(0);
    
    radio.setChannel(channelAr[channelID]);
    
    
  }
}




