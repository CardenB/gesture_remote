#include "command.h"

Command::Command() {}

Command::Command(const string &name) {
  memset(cmd_buf_, 0x00, sizeof(cmd_buf_));
  learned_command_ = false;
  bitlength_ = -1;
  command_name_ = name;
}

bool Command::LearnCommand(Serial &pc) {
  pc.printf("learning command: %s\r\n", command_name_.c_str());
  while (!learned_command_) {
    bitlength_ = receive(&format_, cmd_buf_, sizeof(cmd_buf_));
    if (bitlength_ > 0) {
      display_status("RECV", bitlength_);
      display_data(cmd_buf_, bitlength_);
      // display_format(format_);
      learned_command_ = true;
    }
  }
  return true;
}

bool Command::TransmitCommand() {
  int fail_count = 0;
  while (true) {
    int xmit_bitlen = transmit(format_, cmd_buf_, bitlength_);
    // Case where transmission fails.
    // Retry a set number of times.
    // If it fails the maximum number of times, return false.
    if (xmit_bitlen < 0) {
      if (fail_count == kmax_fail_count_) {
        return false;
      } else {
        // Continue waiting for timeout.
        fail_count++;
        wait(1);
      }
    } else {
      display_status("TRAN", xmit_bitlen);
      return true;
    }
  }
}

