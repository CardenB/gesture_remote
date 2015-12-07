
// Defines for control functions
#define CONTROL_CH 1 // Channel change
#define CONTROL_VOL 2 // Volume
#define CONTROL_POW 3 // Power

#define CONTROL_UP 1
#define CONTROL_DOWN -1

#define DIST_MAX 30 // Maximum distance in inches, anything above is ignored.
#define DIST_DOWN 15 // Threshold for up/down commands. If higher, command is "up". If lower, "down".
#define DIST_POW 8 // Threshold for power command, lower than = power on/off
#include "mbed.h"
#include "hcsr04.h"

Serial pc(USBTX, USBRX); // tx, rx
HCSR04 usensor(p7,p6);

Timer t;
int value;

DigitalOut l1(LED1);
DigitalOut l2(LED2);
DigitalOut l3(LED3);
DigitalOut l4(LED4);
unsigned long timer = t.read_ms();
unsigned int dist;

int handleDist(int dist)
{
  if (dist > DIST_DOWN)
  {
    return CONTROL_UP;
  }
  else
  {
    return CONTROL_DOWN;
  }
}

int main()
{
  
  while (1) {
    if (timer && timer < (t.read_ms() - 5000) && (t.read_ms() > 5000)) {
        l4 = 1;
        wait_ms(300);
        l4 = 0;
        timer = false;
    }
    usensor.start();
    dist = usensor.get_dist_cm();
    l1 = 0;
    l2 = 0;
    l3 = 0;
    l4 = 0;
    //pc.printf("%ld \n\r", dist); //debug
    
    // If less than max inches away, act
    if (dist < DIST_MAX)
    {
        if (dist < DIST_POW)
        {

          if (timer)
          {
            //doIR(CONTROL_POW, 0);
            l1 = 1;
            wait_ms(300);
            l1 = 0;
            timer = false;
            wait_ms(2000); // don't want to be sending this more than once. 2 second delay
          }
          else
          {
            //Serial.println("power flag set");
            timer = t.read_ms();
            wait_ms(500);
          }
        }
        else // is volume or channel
        {
          value = handleDist(dist);
          // wait half a second
          usensor.start();
          wait_ms(300);
          if (usensor.get_dist_cm() > DIST_MAX)
          {
            if (value == 1){
                l2 = 1;
                wait_ms(300);
                l2 = 0;
            } else {
                l2 = 1;
                l1 = 1;
                wait_ms(300);
                l2 = 0;
                l1 = 0;
            }
            //doIR(CONTROL_CH, value); // swipe
          }
          else
          {
            int d = 500; // first delay is longer for single volume change

            while (dist < DIST_MAX)
            {
              value = handleDist(dist); // is up or down?
              //doIR(CONTROL_VOL, value); // fire off IR signal
              if (value == 1){
                  l3 = 1;
                  usensor.start();
                  wait_ms(300);
                  l3 = 0;
              } else {
                  l4 = 1;
                  usensor.start();
                  wait_ms(300);
                  l4 = 0;
              }
              wait_ms(d); // wait
              dist = usensor.get_dist_cm(); // check for hand again
              d = 100; // delays are shorter for quick multiple volume adjustment
            }
            wait_ms(500); // this stops accidental channel change after volume adjustment
          }
        }
    }
        
    wait_ms(25); // Short enough to detect all swipes.
  }
}


