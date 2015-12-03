#include "mbed.h"
#include "IR.h"

 
// We need to use the 'raw' pin reading methods
// because timing is very important here and the digitalRead()
// procedure is slower!
//uint8_t IRpin = 2;
// Digital pin #2 is the same as Pin D2 see
#define IRpin_PIN      PIND
#define IRpin          5
 
// the maximum pulse we'll listen for - 65 milliseconds is a long time
#define MAXPULSE 65000
 
// what our timing resolution should be, larger is better
// as its more 'precise' - but too large and you wont get
// accurate timing
#define RESOLUTION 20
 
Serial pc(USBTX, USBRX);
/*
Serial device(p13, p14);  // tx, rx
PwmOut xmit(p19);
 */
// we will store up to 100 pulse pairs (this is -a lot-)
uint16_t pulses[100][2];  // pair is high and low pulse 
uint8_t currentpulse = 0; // index for pulses we're storing

void delayMicroseconds(int res) {
    wait(((float) res)/1000.0f);
}

void setup(void) {
  // Serial.begin(9600);
  // device.baud(2400);
  pc.printf("Ready to decode IR!\r\n");
}

void printpulses(void) {
  pc.printf("\n\r\n\rReceived: \n\rOFF \tON\r\n");
  for (uint8_t i = 0; i < currentpulse; i++) {
    pc.printf("delayMicroseconds("); 
    pc.printf("%d", pulses[i][0] * RESOLUTION);
    pc.printf("); "); 
    pc.printf("pulseIR(");
    pc.printf("%d", pulses[i][1] * RESOLUTION);
    pc.printf(");\r\n"); 
  }
}
 
void loop(void) {
  uint16_t highpulse, lowpulse;  // temporary storage timing
  highpulse = lowpulse = 0; // start out with no pulse length
 
 
//  while (digitalRead(IRpin)) { // this is too slow!
    // while (IRpin_PIN & (1 << IRpin)) {
  // if(device.readable()) 
    // pc.printf("%s",device.getc());
  /*
    while (device.readable()) {
        pc.putc(device.getc());

     // pin is still HIGH
 
     // count off another few microseconds
     highpulse++;
     delayMicroseconds(RESOLUTION);
 
     // If the pulse is too long, we 'timed out' - either nothing
     // was received or the code is finished, so print what
     // we've grabbed so far, and then reset
     if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
       printpulses();
       currentpulse=0;
       return;
     }
  }
  */
  // we didn't time out so lets stash the reading
  pulses[currentpulse][0] = highpulse;
 
  // same as above
  // while (! (IRpin_PIN & _BV(IRpin))) {
      /*
  while( device.readable() ) {
     // pin is still LOW
     lowpulse++;
     delayMicroseconds(RESOLUTION);
     if ((lowpulse >= MAXPULSE)  && (currentpulse != 0)) {
       printpulses();
       currentpulse=0;
       return;
     }
  }
  */
  pulses[currentpulse][1] = lowpulse;
 
  // we read one high-low pulse successfully, continue!
  currentpulse++;
}

int main() {
  setup();
  //IR Init
  uint8_t buf1[32];
  uint8_t buf2[32];
  int bitlength1 = -1;
  int bitlength2 = 0;
  char tempstr[3];
  RemoteIR::Format format;
  memset(buf1, 0x00, sizeof(buf1));
  memset(buf2, 0x00, sizeof(buf2));
  //END IR Init
  bool signal_received = false;
  while( true ) {
    while( bitlength1 < 0) {
    // if(device.readable() && !signal_received) {
                  // Receive the code
      bitlength1 = receive(&format, buf1, sizeof(buf1));
      if (bitlength1 < 0) {
        continue;
      }
      display_status("RECV", bitlength1);
      display_data(buf1, bitlength1);
      //display_format(format);
      signal_received = true;
    }
    while(true) {
      // RemoteIR::Format f = static_cast<RemoteIR::Format>(atoi(tuple_format));
      RemoteIR::Format f = format;
      bitlength2 = transmit(f, buf1, bitlength1);
      if (bitlength2 < 0) {
          continue;
      }
      display_status("TRAN", bitlength2);
      //display_data(buf1, bitlength1);
      //display_format(format);
      wait(3);
    }
    
  }
  return 0;
}