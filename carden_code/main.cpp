#include "mbed.h"
#include "command.h"

Serial pc(USBTX, USBRX);
// Setup array of remote Commands.
const int num_commands = 1;
Command coms[num_commands];

void delayMicroseconds(int res) { wait(((float)res) / 1000.0f); }

void setup(void) {
  // Serial.begin(9600);
  // device.baud(2400);
  pc.printf("Ready to decode IR!\r\n");
  coms[0] = Command("Power Command");
  coms[0].LearnCommand(pc);
}

int main() {
  setup();
  // Main Loop.
  while (true) {
    for (int i = 0; i < num_commands; ++i) {
      coms[0].TransmitCommand();
      wait(3);
    }
  }
  return 0;
}
