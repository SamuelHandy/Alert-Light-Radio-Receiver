/* Receiver code for the Arduino Radio control with PWM output
 * Install the NRF24 library to your IDE
 * Upload this code to the Arduino UNO, NANO, Pro mini (5V,16MHz)
 * Connect a NRF24 module to it:
 
    Module // Arduino UNO,NANO
    
    GND    ->   GND
    Vcc    ->   3.3V
    CE     ->   D9
    CSN    ->   D10
    CLK    ->   D13
    MOSI   ->   D11
    MISO   ->   D12

This code receive 1 channels and prints the value on the serial monitor
Please, like share and subscribe : https://www.youtube.com/c/ELECTRONOOBS
*/


#include <SPI.h>
// nRF24L01 official libraries
#include <nRF24L01.h>
#include <RF24.h>

//Unique Pipe address
const uint64_t pipeInAddress = 0xE8E8F0F0E1LL; 

//CSN and CE pins of the NRF. Watch video for exact wire positions
RF24 radio(9, 10);  //CSN and CE pins if the NRF

// The Struct to hold the Received Data. This data will be the value of a potentiometer being turned sent to this device. The sizeof this struct should not exceed 32 bytes
struct Received_data {
  byte potentiometer_value;
};

//Declaration of the received_data variable
Received_data received_data;

//LED Pin Number
int LED = 3;

/*//////////////////////////////////////////////////////*/

//This function will only set the value to  0 if the connection is lost...
void resetData() 
{
  received_data.potentiometer_value = 0;  
}

/**************************************************/

void setup()
{
  pinMode(LED,OUTPUT);
  //Initializes the serial communication at a data rate of 9600 bits per second (bps).\
  //You should always have the same speed selected in the serial monitor
  Serial.begin(9600);
  Serial.println("STARTED Receiver");
  //----------
  //We reset the received values To 0, old vervion had 127 instead of 0
  resetData();
  //----------
  //Begins and radio configuration
  radio.begin();
  //----------
  //Sets the automatic acknowledgement of received packets between the transmitter and receiver
  radio.setAutoAck(false);
  //----------
  // sets the data rate to 250 kbps
  // The NRF24L01+ module supports four different data rates: 250 kbps, 1 Mbps (megabits per second), and 2 Mbps. The data rate determines the speed at which the NRF24L01+ module can transmit and receive data. 
  // A higher data rate allows for faster transmission but may also require a stronger signal and more power.
  // This data rate is sufficient for many simple wireless applications
  radio.setDataRate(RF24_250KBPS);  
  //----------
  //The NRF24L01+ module has six data pipes that can be used to receive data. Each data pipe can have its own address, and the module can listen on multiple data pipes at the same time. 
  //The openReadingPipe() function is used to set the address of a data pipe that the module should listen to.
  //The data pipes are numbered from 0 to 5
  radio.openReadingPipe(1,pipeInAddress);
  
  //We start the radio comunication
  radio.startListening();
  
}

/**************************************************/
/******Reset the received data to 0 if connection is lost******/

unsigned long last_Time = 0;

//We create the function that will read the data each certain time
void receive_the_data()
{
  while ( radio.available() ) 
  {
    //In the provided code, &received_data is a pointer to a variable of type Received_data, which is a struct containing a single byte of data. 
    //sizeof(Received_data) is the size of the Received_data struct in bytes, which is one byte.
    radio.read(&received_data, sizeof(Received_data));
    //millis() is a function in Arduino that returns the number of milliseconds that have elapsed since the board was last reset or powered on. 
    //It is used to measure elapsed time without blocking the program execution, 
    //last_time is used to see what the time was when the radio last was connected. it will continue to increase until radio.available is false.
    last_Time = millis();
  }
}


/**************************************************/

void loop()
{
  //Receive the radio data
  receive_the_data();

  //Here we check if we've lost signal, if we did we reset the values. we lose signal if radio.available is lost  for more than 1 second
  unsigned long now = millis();
  if ( now - last_Time > 1000 ) {
  // Signal lost, reset
    resetData();
  }

  Serial.print("Potentiometer: "); Serial.println(received_data.potentiometer_value);  
  
  //Sets the PWM cycle of pin 4(LED) to be the potentiometer value.
  analogWrite(LED,received_data.potentiometer_value);

  /*
  ch1_value = map(received_data.ch1,0,255,1000,2000);
  Serial.println(ch1_value);
  */

  // Maybe create a text file for uvada to process the voice command.(realized the Small Uvada devices Talk to the Main one through ports with WIFI. This receiver would be for the small misc devices(ex: plant water device)/the main Uvada to communicate with and that relationship will just be the Main Uvada sending data to the other devices mainly. only ones that might talk back are temperature sensors. so they might need to have text socuments made for them for Uvada to read so that it may tell me the temperature. uvada will be constantly checking for multiple different files with its looping process. each file could be part of one big IF)  
  
  
}//Loop end
