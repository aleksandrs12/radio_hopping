#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 8); // CE, CSN

const byte address[6] = "00001";
int data[64];
int lastValue = 0;
int totalDataLost;
int dataLost = 0;

const int excluded = 5;
int excludedChannels[excluded] = {35, 36, 37, 38, 39};
const int channelCap = 40;
int channelAr[channelCap-excluded];
int channelArTemplate[channelCap-excluded];
int seed = 496219323000;
unsigned int arraysShuffled = 0;
unsigned int channelID = 0;


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
  int a = 0;
  int id = 0;
  for (int i = 0; i < channelCap; i++){
    if (i != excludedChannels[a]){
      channelAr[id] = i;
      channelArTemplate[id] = i;
      id++;
    }
    else{
      a++;
    }
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
    //Serial.println(micros()-t);
    //t = micros();
    radio.read(&data, sizeof(data));
    if (data[3] != 0){

      channelID = data[0];
      if (channelID -lastValue != 1 && channelID != 0){
        dataLost++;
      }
      lastValue = channelID;
      if (data[1] != arraysShuffled){
        arraysShuffled = data[1];
        shuffle_array(channelAr, channelCap-excluded, seed - arraysShuffled, channelArTemplate);
      }
      channelID++;
      if (channelID >= channelCap-excluded){
        channelID = 0;
        arraysShuffled++;
        shuffle_array(channelAr, channelCap-excluded, seed - arraysShuffled, channelArTemplate);
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

  //Serial.println(channelAr[channelID]);
}
