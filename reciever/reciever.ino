#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 8); // CE, CSN

const byte address[6] = "00001";
int data[64];
int lastValue = 0;
int totalDataLost;
int dataLost = 0;

const int channelCap = 30;
int channelAr[channelCap];
int channelArTemplate[channelCap];
int seed = 30001;
unsigned int arraysShuffled = 0;
unsigned int channelID = 0;
unsigned int channel = 0;
int firstInt = 20009;



void shuffle_array(int *array, int size, int random_number, int *arrayT) {
      Serial.print(arraysShuffled);
      Serial.print(" ");
      totalDataLost += dataLost;
      Serial.println(totalDataLost);
    
    dataLost = 0;
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
  pinMode(3, OUTPUT);
  pinMode(2, OUTPUT);
  for (int i = 0; i < channelCap; i++){
    channelAr[i] = i;
    channelArTemplate[i] = i;
  }
  Serial.begin(9600);
  radio.begin();
  radio.powerUp();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_2MBPS);
  radio.startListening();
  radio.setChannel(0);
}
long t = micros();

void loop() {
  
  if (radio.available()) {
    //Serial.println(micros()-t); //500 default
    
    radio.read(&data, sizeof(data));
    if (data[0] == firstInt){

      /*
      if (channelID -lastValue != 1 && channelID != 0){
        dataLost++;
      }
      */
      
      lastValue = channelID;
      if (data[1] != arraysShuffled && micros()-t > 10000){
        if (data[1] == 10000){
          Serial.println(micros()-t);
        }
        arraysShuffled = data[1];
        shuffle_array(channelAr, channelCap, seed - arraysShuffled, channelArTemplate);
      }
      t = micros();
      if (arraysShuffled == 10000){
        dataLost++;
      }
      channelID++;
      if (channelID >= channelCap){
        channelID = 0;
        arraysShuffled++;
        shuffle_array(channelAr, channelCap, seed - arraysShuffled, channelArTemplate);
      }
      radio.setChannel(channelAr[channelID]);

      
    }
  }
  if (data[4] == 0 || data[4] == 1){
    digitalWrite(2, data[4]);
  }
  if (data[5] == 0 || data[5] == 1){
    digitalWrite(3, data[5]);
  }
  /*
  if (micros()-t > 900){
    t = 0;
    channelID++;
      if (channelID >= channelCap){
        channelID = 0;
        arraysShuffled++;
        shuffle_array(channelAr, channelCap, seed - arraysShuffled, channelArTemplate);
      }
      radio.setChannel(channelAr[channelID]);
  }
  */

  //Serial.println(channelAr[channelID]);
}
