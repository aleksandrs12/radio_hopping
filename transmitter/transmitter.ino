#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 8); // CE, CSN

const byte address[6] = "00001";
int channel = 0;
int data[64] = {2, 4, 8, 16, 32, 64, 128, 256, 69, 42, 1, 1, 2, 2, 3, 3, 2, 4, 8, 16, 32, 64, 128, 256, 69, 42, 1, 1, 2, 2, 3, 3, 2, 4, 8, 16, 32, 64, 128, 256, 69, 42, 1, 1, 2, 2, 3, 3, 2, 4, 8, 16, 32, 64, 128, 256, 69, 42, 1, 1, 2, 2, 3, 3};
const int channelCap = 30;
int channelAr[channelCap];
int channelArTemplate[channelCap];
int seed = 30001;
unsigned int arraysShuffled = 0;
unsigned int channelID = 0;


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
}

void loop() {
  if (channelID >= channelCap){
    arraysShuffled++;
    channelID = 0;
    shuffle_array(channelAr, channelCap, seed - arraysShuffled, channelArTemplate);

    //Serial.println(arraysShuffled);
    
    
  }

  data[0] = channelID;
  data[1] = arraysShuffled;
  data[2] = channelAr[channelID];

  data[4] = digitalRead(2);
  data[5] = digitalRead(3);
  
  radio.setChannel(channelAr[channelID]);
  radio.write(&data, sizeof(data));
  //Serial.println(channelAr[channelID]);
  channelID++;
  
}




















