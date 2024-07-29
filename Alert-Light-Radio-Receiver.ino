/*******************INCLUDES***********************/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

/*******************VARIABLES**********************/

//Unique Pipe address
const uint64_t pipeInAddress = 0xE8E8F0F0E1LL; 

//CSN and CE pins of the NRF
RF24 radio(9, 10); 

//0 = OFF
//1 = ON
struct Received_data {
  byte light_status = 0;
};
Received_data received_data;

//Pin 3
int BUTTON_LED_POWER_PIN = 3;

//Pin 4
int BUTTON_DIGITAL_INPUT_PIN = 4;

//The ON/OFF state of the button
int BUTTON_STATE = 0;

/*********************SETUP************************/

void setup()
{
  //Setup Pins to Output
  pinMode(BUTTON_LED_POWER_PIN,OUTPUT);
  pinMode(BUTTON_DIGITAL_INPUT_PIN,INPUT_PULLUP);
  //Setup Radio
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1,pipeInAddress);
  radio.startListening();
}

/*******************resetData()********************/

void resetData() 
{
  received_data.light_status = 0;  
}

/***************receive_the_data()*****************/

void receive_the_data()
{
  while ( radio.available() && received_data.light_status == 0) 
  {
    //Reads the data and sets it to the received_data struct
    radio.read(&received_data, sizeof(Received_data));

    //Checks the value received and acts depending on the value received, if none are valid then it resets the incoming information
    if(received_data.light_status == 1)
      digitalWrite(BUTTON_LED_POWER_PIN, received_data.light_status); //1 == HIGH
    else
      resetData();
  }
}

/**********************LOOP************************/

void loop()
{
  //Receive the radio data
  receive_the_data();
 
  //If light_status is ON, then enable the button to turn it off
  if(received_data.light_status == 1)
  {
    BUTTON_STATE = digitalRead(BUTTON_DIGITAL_INPUT_PIN);
    if(BUTTON_STATE == LOW) // button is pressed
    {
      resetData();
      digitalWrite(BUTTON_LED_POWER_PIN, received_data.light_status); //0 == LOW
    }
  }
}
