// cris ababei
// this is a sketch which will run on Arduino board that plays the role of
// an USB Host; it needs an Arduino USB shield (must buy separately)!
// the sketch receives data sent from a CDC-ACM device, which is played by another
// Arduino from where I am sending (triggered by a "start" command) a fixed 
// 100 numbers in scientific format (each number takes exactly 10
// characters or bits in this format; I send a space (i.e., ' ') between numbers too);
//
// NOTES:
// --this sketch was developed starting from one of the examples that come
// with the library “USB Host Shield Library 2.0”, which needs to be installed
// in your Arduino IDE https://github.com/felis/USB_Host_Shield_2.0
// that example is called "acm_terminal", located at:
// C:\Users\Cristinel Ababei\Documents\Arduino\libraries\USB_Host_Shield_Library_2.0\examples\acm\acm_terminal
// or you can read it on GitHub:
// https://github.com/felis/USB_Host_Shield_2.0/blob/master/examples/acm/acm_terminal/acm_terminal.ino
// --the USB shield I used in my experiments is made 
// by HiLetgo and purchased on amazon:
// "USB Host Shield for Arduino UNO Support Google Android ADK USB HUB"
// https://www.amazon.com/dp/B01MTU9OLM?ref=ppx_yo2ov_dt_b_fed_asin_title
// this shield is essentially a breakout board for MAX3421E chip:
// https://www.analog.com/en/products/max3421e.html
// which is a single IC with USB functionality!
//
// NOTES:
// --the way data is received via USB is using a polling approach; this here, keeps iterating 
// the main loop and asking to read from cdc-acm device, but only 64 bytes maximum at a time;
// --that is not the best; it should read only when an event occurs that the cdc-acm is sending something...
// --also, digging into source code of “USB Host Shield Library 2.0”, I see poor coding
// with many comments pointing to things that may not work; not sure how things may go in 
// various scenarious; I am worried a bit about this library “USB Host Shield Library 2.0”...
// --the baudrate of 115200 to talk between the two Arduino boards seems to fast - another
// limitation of “USB Host Shield Library 2.0”? the lower 9600 baudrate seems to work fine;
// --the meaning of bytes_rcvd in the code below is that it is how much it is requested to read
// and place into packet_receive; so, it is used to pass this "requested"
// number, but, it also gets overwritten with how many bytes exactly have been received;
// see inside the below file, how that is done inside function: uint8_t USB::InTransfer(...)
// C:\Users\Cristinel Ababei\Documents\Arduino\libraries\USB_Host_Shield_Library_2.0\Usb.cpp

#include <cdcacm.h>
#include <usbhub.h>
#include "pgmstrings.h"
#include <SPI.h>

///////////////////////////////////////////////////////////////////////////////
// 
// ACMAsyncOper
//
///////////////////////////////////////////////////////////////////////////////

class ACMAsyncOper : public CDCAsyncOper
{
  public:
    uint8_t OnInit(ACM *pacm);
};

uint8_t ACMAsyncOper::OnInit(ACM *pacm)
{
  uint8_t rcode;
  // Set DTR = 1 RTS=1
  rcode = pacm->SetControlLineState(3);
  if (rcode) {
    ErrorMessage<uint8_t>(PSTR("SetControlLineState"), rcode);
    return rcode;
  }

  LINE_CODING	lc;
  lc.dwDTERate	= 9600; 
  lc.bCharFormat	= 0;
  lc.bParityType	= 0;
  lc.bDataBits	= 8;

  rcode = pacm->SetLineCoding(&lc);
  if (rcode) {
    ErrorMessage<uint8_t>(PSTR("SetLineCoding"), rcode);
  }
  return rcode;
}

///////////////////////////////////////////////////////////////////////////////
// 
// Globals
//
///////////////////////////////////////////////////////////////////////////////

// USB related variables;
USB          Usb;
ACMAsyncOper AsyncOper;
ACM          Acm(&Usb, &AsyncOper);

// operation will be modeled via a FSM state graph;
enum MACHINE_STATE { IDLE, SEND_START_CMD, RECEIVING, PROCESSING };
MACHINE_STATE state_current = IDLE;

// initial default state will be IDLE; from which, if user
// types in Serial monitor of Arduino IDE "go" will make the FSM 
// transition to state SEND_START_CMD;
bool received_go_from_user = false;

// command "start" will be sent to cdc-acm device to tell it to start sending the
// array of numbers;
uint8_t buffer_send[] = "start\n"; 
uint16_t bytes_send = 0;

// max USB packet size to receive is 64 bytes when working with “USB Host Shield Library 2.0” 
// library; this is a bit weird;
uint8_t packet_receive[16]; 
uint16_t bytes_rcvd = 16;

// we receive via USB bytes in packets of up to 16 bytes and keep appending them
// to the local buffer that will store 300 numbers in scientific format, each
// taking 10 bytes; each number is expected to be received followed by a space
// so, in total buffer_receive should have 300*11 = 3300 bytes reserved;
#define BUFFER_SIZE 1100
char data_buffer[1100];
int data_buff_i = 0; // general index;
int global_counter = 0; // keep track of all bytes received until receiving terminating '\0';

// temp buffer used for converting string of characters terminated with '\0' to 
// float number;
#define NUM_RECEIVED_TOTAL 100 // expected cuunt of scientific-format numbers from cdc-acm device;
#define NUM_CHARS_IN_SCI_NUMBER 10 // each number received from cdc-acm device occupies 10 chars; example: 8.5769E-01
char temp_buf[NUM_CHARS_IN_SCI_NUMBER + 1]; // +1 because we will always end with special NULL char '\0';

///////////////////////////////////////////////////////////////////////////////
// 
// setup
//
///////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  Serial.println("Start");

  if (Usb.Init() == -1) {
    Serial.println("OSCOKIRQ failed to assert");
  }
  delay(200);
}

///////////////////////////////////////////////////////////////////////////////
// 
// main
//
///////////////////////////////////////////////////////////////////////////////

void loop()
{
  Usb.Task();

  if (Acm.isReady()) {
    uint8_t rcode;

    switch (state_current) {

      case IDLE:
        // this is default initial state; 
        state_current = IDLE; // stay here until "go" is received from user in Serial terminal;
        received_go_from_user = false;
        while (!Serial.available()) {} // wait until something, anything is received;
        if (Serial.available()) {
          String receivedString = Serial.readStringUntil('\n'); // read until new line
          receivedString.trim(); // remove leading/trailing whitespace
          if (receivedString == "go") {
            received_go_from_user = true;
            state_current = SEND_START_CMD;
          }
        }
        break;

      case SEND_START_CMD:
        // send command "start" to cdc-acm device; command is 
        // hard coded buffer that stores command "start";
        bytes_send = strlen(buffer_send);
        rcode = Acm.SndData(bytes_send, buffer_send);
        if (rcode) {
          ErrorMessage<uint8_t>(PSTR("SndData"), rcode);
        }
        state_current = RECEIVING;
        break;

      case RECEIVING:
        state_current = RECEIVING; // will stay in RECEIVING state until '\0' is received;
        // reading packets from the cdc-acm device;
        // NOTE: next, we "request" 16 bytes to be read in; but, if you dig in
        // the code of the library “USB Host Shield Library 2.0”, we see that this
        // variable, bytes_rcvd, will be over-written with the actual number of bytes
        // received! it also cannot be larger than 64;
        bytes_rcvd = 16; 
        rcode = Acm.RcvData(&bytes_rcvd, packet_receive);
        if (rcode && rcode != hrNAK) {
          ErrorMessage<uint8_t>(PSTR("Ret"), rcode);
        }
        if (bytes_rcvd > 0) { // more than zero bytes received
          for (uint16_t i=0; i < bytes_rcvd; i++) {
            // () append received chars to global buffer receive;
            // as longs as there is still room in the buffer;
            if (data_buff_i < BUFFER_SIZE) {
              data_buffer[data_buff_i] = (char)packet_receive[i];
              //Serial.print((char)packet_receive[i]);
              if (packet_receive[i] == '\0') { // check if received byte is terminating char '\0';
                state_current = PROCESSING;
              }
              data_buff_i++;
            } else {
              // buffer is full; 
              state_current = PROCESSING;
            }
          }
        }
        break;

      case PROCESSING:
        // print all received numbers, convert from char to float first;
        // we should have received numbers, now stored as chars in data_buffer like this:
        // 8.5769E-01 9.1667E-01 7.0802E-01 ...
        // here, we take each group of 11 chars, replace ' ' with '\0' and pass
        // that to atof() to convert to float, which we then print;
        // for example, "8.5769E-01 " is taken and placed into temp_buf as "8.5769E-01\0";
        // NOTE: we replace ' ' with '\0' because we are paranoic and want to
        // pass to atof() what it expects as terminating character; see:
        // https://www.ibm.com/docs/en/i/7.4.0?topic=functions-atof-convert-character-string-float
        global_counter = 0;
        data_buff_i = 0;
        Serial.println("RECEIVED:");
        while (global_counter < NUM_RECEIVED_TOTAL) { // 100
          uint16_t j=0;
          for (j=0; j < NUM_CHARS_IN_SCI_NUMBER; j++) { // read 10 chars that form the sci-format number;
            temp_buf[j] = data_buffer[data_buff_i];
            data_buff_i++;
          }
          temp_buf[j] = '\0'; // add as 11th char the special char '\0'
          data_buff_i++; // skip over the 11th char that is ' ' in the actual data_buffer
          Serial.print( atof(temp_buf), 4 ); // print with 4 decimals;
          Serial.print(' ');
          global_counter++;
        }
        //Serial.println("RECEIVED:");
        //Serial.println(data_buffer);
        state_current = IDLE;
        break;

      default:
        break; // do nothing;
    }
  } // if (Acm.isReady()) 
}
