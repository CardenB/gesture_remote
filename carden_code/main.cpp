#include "mbed.h"
#include "command.h"

Serial pc(USBTX, USBRX);
// Setup array of remote Commands.
const int num_commands = 5;
enum COMMANDS {
  POWER,
  CHANNEL_UP,
  CHANNEL_DOWN,
  VOLUME_UP,
  VOLUME_DOWN
};
const string com_names[num_commands] = {"power", "channel up", "channel down",
                                        "volume up", "volume down"};

Command coms[num_commands];

void delayMicroseconds(int res) { wait(((float)res) / 1000.0f); }

void setup(void) {
  // Serial.begin(9600);
  // device.baud(2400);
  pc.printf("Ready to decode IR!\r\n");

  for( int i = 0; i < num_commands; ++i) {
    coms[i] = Command(com_names[i]);
    coms[i].LearnCommand(pc);
  }
}

int main() {
  setup();
  // Main Loop.
  while (true) {
    for (int i = 0; i < num_commands; ++i) {
      coms[i].TransmitCommand();
      wait(3);
    }
  }
  return 0;
}


