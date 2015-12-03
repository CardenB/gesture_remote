#include "mbed.h"
#include "IR.h"
#include "ReceiverIR.h"
#include "TransmitterIR.h"

ReceiverIR ir_rx(p15);
TransmitterIR ir_tx(p21);

/**
 * Receive.
 *
 * @param format Pointer to a format.
 * @param buf Pointer to a buffer.
 * @param bufsiz Size of the buffer.
 *
 * @return Bit length of the received data.
 */
int receive(RemoteIR::Format *format, uint8_t *buf, int bufsiz, int timeout) {
    int cnt = 0;
    while (ir_rx.getState() != ReceiverIR::Received) {
        cnt++;
        if (timeout < cnt) {
            return -1;
        }
    }
    return ir_rx.getData(format, buf, bufsiz * 8);
}

/**
 * Transmit.
 *
 * @param format Format.
 * @param buf Pointer to a buffer.
 * @param bitlength Bit length of the data.
 *
 * @return Bit length of the received data.
 */
int transmit(RemoteIR::Format format, uint8_t *buf, int bitlength, int timeout) {
    int cnt = 0;
    while (ir_tx.getState() != TransmitterIR::Idle) {
        cnt++;
        if (timeout < cnt) {
            return -1;
        }
    }
    return ir_tx.setData(format, buf, bitlength);
}

/**
 * Display a current status.
 */
void display_status(char *status, int bitlength) {

    printf("%-5.5s:%02d \n \r", status, bitlength);
}

/**
 * Display a format of a data.
 */
void display_format(RemoteIR::Format format) {

    switch (format) {
        case RemoteIR::UNKNOWN:
            printf("????????");
            break;
        case RemoteIR::NEC:
            printf("NEC     ");
            break;
        case RemoteIR::NEC_REPEAT:
            printf("NEC  (R)");
            break;
        case RemoteIR::AEHA:
            printf("AEHA    ");
            break;
        case RemoteIR::AEHA_REPEAT:
            printf("AEHA (R)");
            break;
        case RemoteIR::SONY:
            printf("SONY    ");
            break;
    }
}

/**
 * Display a data.
 *
 * @param buf Pointer to a buffer.
 * @param bitlength Bit length of a data.
 */
void display_data(uint8_t *buf, int bitlength) {
    //lcd.locate(0, 1);
    const int n = bitlength / 8 + (((bitlength % 8) != 0) ? 1 : 0);
    for (int i = 0; i < n; i++) {
        printf("%02X", buf[i]);
    }
    for (int i = 0; i < 8 - n; i++) {
        printf("--\n \r");
    }
    
}