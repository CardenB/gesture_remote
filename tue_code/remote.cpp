#define DIST_MAX 40 // Maximum distance in cm, anything above is ignored.
// Threshold for up/down commands.
// If higher, command is "up". If lower, "down".
#define DIST_THRES 20

#include "mbed.h"
#include "rtos.h"
#include "hcsr04.h"
#include "SI1143.h"

Serial pc(USBTX, USBRX); // tx, rx
HCSR04 usensor(p7, p6);
SI1143 sensor(p28, p27);

DigitalOut l1(LED1);
DigitalOut l2(LED2);
DigitalOut l3(LED3);
DigitalOut l4(LED4);

int value;

unsigned int dist;
int sense1, sense2, sense3;

const float period = 1.0;
const float power_execute_count = 3.0 / period;
float power_count = 0;
Mutex led_mut;

bool flag = false;

Ticker power_ticker;
enum MODE {
  NONE = 0,
  CHANNEL = 1,
  POWER = 2,
  VOLUME = 3,

};
MODE mode = NONE;

void set_led(uint8_t led_vals) {
  led_mut.lock();
  l1 = led_vals & 1;
  led_vals = led_vals >> 1;
  l2 = led_vals & 1;
  led_vals = led_vals >> 1;
  l3 = led_vals & 1;
  led_vals = led_vals >> 1;
  l4 = led_vals & 1;
  led_mut.unlock();
}

void ExecutePowerCommand() {
  led_mut.lock();
  pc.printf("Power!");
  led_mut.unlock();
  set_led(15);
}

void CheckForPower() {
  if (mode == POWER) {
    if (power_count >= power_execute_count) {
      ExecutePowerCommand();
    }
    ++power_count;
  } else {
    power_count = 0;
    set_led(0);
  }
}

Timer t;

int timer;


int handleDist(int dist) {
  if (dist > DIST_THRES) {
    return 1;
  } else {
    return -1;
  }
}

void parallax_thread(void const *args) {
  while (true) {
    sense1 = sensor.get_ps1(1);
    sense2 = sensor.get_ps2(1);
    sense3 = sensor.get_ps3(1);

    if (sense1 > 80 || sense2 > 80 || sense3 > 80) {
      if (sense1 > sense2 && sense1 > sense3) {
        set_led(1);
        mode = CHANNEL;
      }

      else if (sense2 > sense1 && sense2 > sense3) {
        mode = POWER;
        set_led(3);
      }

      else if (sense3 > sense1 && sense3 > sense2) {
        set_led(2);
        mode = VOLUME;
      }
    }
    Thread::wait(50);
  }
}

void ultrasound_thread(void const *args) {
  while (true) {
    usensor.start();
    dist = usensor.get_dist_cm();
    if (dist < DIST_MAX) {
      int d = 500; // first delay is longer for single volume change

      while (dist < DIST_MAX) {
        value = handleDist(dist); // is up or down?
        // doIR(CONTROL_VOL, value); // fire off IR signal
        if (value == 1) {
          set_led(8);
          Thread::wait(100);
          set_led(0);
        } else {
          set_led(4);
          Thread::wait(100);
          set_led(0);
        }
        usensor.start();
        Thread::wait(d);              // wait
        dist = usensor.get_dist_cm(); // check for hand again
        d = 100; // delays are shorter for quick multiple volume adjustment
      }
      // this stops accidental channel change after volume adjustment
      Thread::wait(500);
    }
    Thread::wait(50); // Short enough to detect all swipes.
  }
}

int main() {

  power_ticker.attach(&CheckForPower, period);
  set_led(0);
  // Setup the baseline
  sensor.bias(1, 5);
  wait(1);
  Thread mode_checker(parallax_thread);
  Thread range_checker(ultrasound_thread);

  while (1) {
    // spin.
  }
}
