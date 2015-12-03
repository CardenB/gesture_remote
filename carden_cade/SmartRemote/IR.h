//PROTOTYPES
#include "mbed.h"
#include "ReceiverIR.h"
#include "TransmitterIR.h"

int receive(RemoteIR::Format *format, uint8_t *buf, int bufsiz, int timeout = 100);
int transmit(RemoteIR::Format format, uint8_t *buf, int bitlength, int timeout = 100);
void display_status(char *status, int bitlength);
void display_format(RemoteIR::Format format);
void display_data(uint8_t *buf, int bitlength);