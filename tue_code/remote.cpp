
#define DIST_MAX 40 // Maximum distance in cm, anything above is ignored.
#define DIST_THRES 20 // Threshold for up/down commands. If higher, command is "up". If lower, "down".

#include "mbed.h"
#include "hcsr04.h"
#include "SI1143.h"

Serial pc(USBTX, USBRX); // tx, rx
HCSR04 usensor(p7,p6);
SI1143 sensor(p28, p27);

DigitalOut l1(LED1);
DigitalOut l2(LED2);
DigitalOut l3(LED3);
DigitalOut l4(LED4);

int value;

unsigned int dist;

const float period = 1.0;
const float power_execute_count = 3.0/period;
float power_count = 0;

bool flag = false;

Ticker power_ticker;
enum MODE {
    NONE = 0,
    CHANNEL = 1,
    POWER = 2,
    VOLUME = 3,

};
MODE mode = NONE;

void ExecutePowerCommand() {
    pc.printf("Power!");
    l4 = 1;
    l3 = 1;
}

void CheckForPower() {
    if(mode == POWER) {
        if(power_count >= power_execute_count) {
            ExecutePowerCommand();
        }
        ++power_count;
    } else {
        power_count = 0;
        l4 = 0;
        l3 = 0;
    }
}

Timer t;

int timer;

int handleDist(int dist)
{
  if (dist > DIST_THRES)
  {
    return 1;
  }
  else
  {
    return -1;
  }
}

int main()
{

  power_ticker.attach(&CheckForPower, period);
  l1 = 0;
  l2 = 0;
  l3 = 0;
  l4 = 0;
  int sense1,sense2,sense3;
    
  // Setup the baseline
  sensor.bias(1,5);
  wait(1);

  while (1) {
    sense1 = sensor.get_ps1(1);
    sense2 = sensor.get_ps2(1);
    sense3 = sensor.get_ps3(1);

    if (sense1 > 80 || sense2 > 80 || sense3 > 80)
    {
        if (sense1 > sense2 && sense1 > sense3)
        {
            l1=1;
            l2=0;
            l3=0;
            mode = CHANNEL;
        }
        
        else if(sense2 > sense1 && sense2 > sense3)
        {
            mode = POWER;
            l1=1;
            l2=1;
            l3=0;
        }
        
        else if(sense3 > sense1 && sense3 > sense2)
        {
            l1=0;
            l2=1;
            l3=0;
            mode = VOLUME;
        }
    }

    usensor.start();
    dist = usensor.get_dist_cm();

    //pc.printf("%ld \n\r", dist); //debug
    
    if (dist < DIST_MAX)
    {
        int d = 500; // first delay is longer for single volume change

        while (dist < DIST_MAX)
        {
          value = handleDist(dist); // is up or down?
          //doIR(CONTROL_VOL, value); // fire off IR signal
          if (value == 1){
              l4 = 1;
              wait_ms(100);
              l4 = 0;
          } else {
              l3= 1;
              wait_ms(100);
              l3 = 0;
          }
          usensor.start();
          wait_ms(d); // wait
          dist = usensor.get_dist_cm(); // check for hand again
          d = 100; // delays are shorter for quick multiple volume adjustment
        }
        wait_ms(500); // this stops accidental channel change after volume adjustment
    }
    wait_ms(50); // Short enough to detect all swipes.
  }
}
