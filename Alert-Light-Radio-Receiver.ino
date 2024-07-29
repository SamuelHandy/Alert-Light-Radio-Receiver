#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//Unique Pipe address
const uint64_t pipeInAddress = 0xE8E8F0F0E1LL; 

//CSN and CE pins of the NRF. Watch video for exact wire positions
RF24 radio(9, 10);  //CSN and CE pins if the NRF

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

/**************************************************/

void setup()
{
  //Setup LED Pin to Output
  pinMode(BUTTON_LED_POWER_PIN,OUTPUT);
  pinMode(BUTTON_DIGITAL_INPUT_PIN,INPUT_PULLUP);
  //Setup Radio
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1,pipeInAddress);
  radio.startListening();
}

/**************************************************/

void resetData() 
{
  received_data.light_status = 0;  
}

/**************************************************/


/******Reset the received data to 0 if connection is lost******/
unsigned long last_Time = 0;


//We create the function that will read the data each certain time
void receive_the_data()
{
  while ( radio.available() && received_data.light_status == 0) 
  {
    //In the provided code, &received_data is a pointer to a variable of type Received_data, which is a struct containing a single byte of data. 
    //sizeof(Received_data) is the size of the Received_data struct in bytes, which is one byte.
    radio.read(&received_data, sizeof(Received_data));

    //Checks the value received and acts depending on the value received, if none are valid then it resets the incoming information
    if(received_data.light_status == 1)
    {
      digitalWrite(BUTTON_LED_POWER_PIN, received_data.light_status);
    }
    else
    {
      resetData();
    }
  }
}


/**************************************************/

void loop()
{
  //Receive the radio data
  receive_the_data();
  /*
  //If light_status is ON, then enable the button to turn it off
  if(received_data.light_status == 1)
  {
    BUTTON_STATE = digitalRead(BUTTON_DIGITAL_INPUT_PIN);
    if(BUTTON_STATE == LOW) // button is pressed
    {
      resetData();
      digitalWrite(BUTTON_LED_POWER_PIN, received_data.light_status);
    }
  }
  */
}
