#include "mbed.h"
#include "IR.h"

#include <string>

class Command {
public:
  Command();
  explicit Command(const std::string &name);
  bool LearnCommand(Serial &pc);
  bool TransmitCommand();

private:
  std::string command_name_;
  uint8_t cmd_buf_[32];
  RemoteIR::Format format_;
  bool learned_command_;
  int bitlength_;
  static const int kmax_fail_count_ = 3;
};



