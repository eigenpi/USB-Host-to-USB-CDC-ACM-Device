// cris ababei
// this is a sketch which will run on Arduino Uno board that plays the role of
// an CDC-ACM device; I am sending a fixed hard coded count of 100 numbers;
// I send them one by one in scientific format; each number takes exactly 10
// characters or bits in this format; I send a space (i.e., ' ') between numbers;
// so, in total sending a number takes 11 bytes (including the separator space);
// NOTES:
// I am using utilities (i.e., "MathHelpers.h") to convert to scientific format 
// of same length all numbers:
// https://github.com/RobTillaart/Arduino/blob/master/libraries/MathHelpers/MathHelpers.h

#include "MathHelpers.h"

#define BUFFER_SIZE 100

float data_buffer[] = { // 100 random float numbers to send to USB Host device;
  1.85769287, 9.91666667, 0.70801953, 0.15812045, 0.05682766, 0.15136703, 0.05509788, 0.27653167, 0.90251432, 0.05562347, 
  0.72383908, 0.65864495, 0.2040728,  0.67086314, 0.2138099,  0.2569055,  0.66249458, 0.45141513, 0.90088181, 0.7110021,  
  0.04635209, 0.7217374,  0.04984498, 0.22107996, 0.13310408, 1.,         0.78204594, 0.23074787, 0.,         0.21222381, 
  0.,         0.70404984, 0.17764025, 1.,         0.93062862, 0.58160558, 0.,         0.56453664, 0.,         0.54257529, 
  0.49988083, 0.5,        0.55323014, 0.4087375,  0.34127499, 0.40996662, 0.34037559, 0.27455867, 0.71061486, 0.,         
  0.64553609, 0.71566087, 0.3525231,  0.72646638, 0.36442555, 0.18442368, 0.19252182, 1.,         0.784733,   0.21223621, 
  0.,         0.19578763, 0.,         0.48535826, 0.00955669, 0.33348151, 0.24603565, 0.99851907, 0.97305398, 0.99818789, 
  0.98198246, 0.57455867, 0.99750436, 0.33348151, 0.60031034, 0.59940763, 0.46327369, 0.61478302, 0.48357693, 0.11557632, 
  0.53459876, 0.33348151, 0.46599553, 0.74639023, 0.725163,   0.75593705, 0.74782532, 0.3549325,  0.12364497, 0.,        
  0.49294176, 0.91484635, 0.7227836,  0.91654745, 0.73476836, 0.47715472, 0.99523324, 0.33353708, 3.65923627, 7.58867086};

bool received_start_cmd = false;

///////////////////////////////////////////////////////////////////////////////
// 
// setup
//
///////////////////////////////////////////////////////////////////////////////

void setup()
{
  // I use 9600 because 115200 seemed to be too fast for the USB Host device
  // on the other side;
  Serial.begin(9600);
}

///////////////////////////////////////////////////////////////////////////////
// 
// main
//
///////////////////////////////////////////////////////////////////////////////
 
void loop(void)
{
  // (1) wait for "start" command;
  received_start_cmd = false;
  while (!Serial.available()) {} // wait until something, anything is received;
  if (Serial.available()) {
    String receivedString = Serial.readStringUntil('\n'); // read until newline
    receivedString.trim(); // remove leading/trailing whitespace
    if (receivedString == "start") {
      received_start_cmd = true;
    }
  }

  // (2) if command "start" was received send the hard coded array of float numbers;
  // TODO: maybe send numbers in groups/packets of say COUNT_IN_A_PACKET=5 numbers; 
  // each number uses 10 chars/bytes;  with a space in between, we could build 
  // packets of 10*5+1*5=55 chars; for now, I send one number followed by ' ';
  // when done sending all numbers I send '\0', which on the USB Host device will
  // be used to identify when I am done with sending from this side;
  if (received_start_cmd == true) {
    for (int i = 0; i < BUFFER_SIZE; i++) { 
      Serial.print( sci(data_buffer[i], 4) ); // use sci() method from local library "MathHelpers.h"; example: 9.9167E+00
      Serial.print(' '); // separate numbers with a space char;
    }
    Serial.print('\0'); // done sending whole array;
    received_start_cmd = false; // reset and prepare the wait for another "start" command; 
  }
}
