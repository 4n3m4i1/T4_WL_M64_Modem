#include "T4_WL_M64.h"
#include "T4_WL_M64_CONSTANTS.h"

/*
    This is bad, I know it's bad, it's 2AM and this
    needed to work weeks ago.
*/

#ifndef M64_SERIAL
#define M64_SERIAL      Serial3
#endif

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

uint8_t inline M64_SERIAL_PEEK(){
    return M64_SERIAL.peek();
}

uint8_t inline M64_SERIAL_READ(){
    return M64_SERIAL.read();
}

bool M64_READ_AND_MATCH_SOP(){
    bool retval = FALSE;
    uint8_t v__ = M64_SERIAL_READ();
    if(v__ == M64_MSG_0 && M64_SERIAL_READ() == M64_MSG_1) retval = TRUE;

    return retval;
}

uint8_t inline M64_SERIAL_AVAIL(){
    return M64_SERIAL.available();
}

uint8_t inline M64_SERIAL_AVAIL_4_WRITE(){
    return M64_SERIAL.availableForWrite();
}

void inline M64_SERIAL_SEND(uint8_t val){
    M64_SERIAL.write(val);
}

void inline M64_SERIAL_SEND_WC(){
    M64_SERIAL_SEND(M64_MSG_0);
    M64_SERIAL_SEND(M64_MSG_1);
}

void M64_SERIAL_SEND_PACKET(uint8_t len, uint8_t *data){
    for(uint8_t n = 0; n < len; n++) M64_SERIAL_SEND(data[n]);
}

bool M64_request(char command){
    bool retval = FALSE;
    if(M64_SERIAL_AVAIL_4_WRITE() > M64_MIN_RQ_LEN){
        M64_SERIAL_SEND_WC();
        M64_SERIAL_SEND(command);
        retval = TRUE;
    }
    return retval;
}


bool M64_set_modem_config(struct M64_MODEM_INST *a, uint8_t role, uint8_t channel){
    bool retval = FALSE;
    if(M64_SERIAL_AVAIL_4_WRITE() > (M64_MIN_RQ_LEN + 2)){
        M64_SERIAL_SEND_WC();
        M64_SERIAL_SEND(M64_CONGIG_SET);
        M64_SERIAL_SEND(role);
        M64_SERIAL_SEND(channel);

        a->role = role;
        a->channel = channel;

        retval = TRUE;
    }
    return retval;
}

// You should know if you're sending ASCII or bin
//  we'll only be working with binary
bool M64_send_packet(uint8_t size, uint8_t *payload){
    bool retval = FALSE;
    if((M64_SERIAL_AVAIL_4_WRITE() > (M64_MIN_RQ_LEN + 1 + size)) && size > 0){
        M64_SERIAL_SEND_WC();
        M64_SERIAL_SEND(M64_PACKET_TX);
        M64_SERIAL_SEND(size);
        M64_SERIAL_SEND_PACKET(size, payload);

        retval = TRUE;
    }
    return retval;
}


// This is the annoying bit
char M64_parser(struct M64_MODEM_INST *a){
    if(M64_SERIAL_AVAIL() > M64_MIN_RQ_LEN + 1){
        while(M64_SERIAL_PEEK() != M64_MSG_0) M64_SERIAL_READ();
        if(M64_READ_AND_MATCH_SOP()){
            switch(M64_SERIAL_READ()){
                case M64_VERSION:

                break;

                case M64_PAYLOAD_SIZE:

                break;

                case M64_CONFIG_GET:

                break;

                case M64_CONGIG_SET:

                break;

                case M64_QUEUE_LEN:

                break;

                case M64_QUEUE_FLUSH:

                break;

                case M64_DIAGNOSTIC:

                break;

                case M64_PACKET_TX:

                break;

                case M64_PACKET_RX:

                break;

                case M64_MALFORMED_RQ:

                break;

                case M64_BAD_CHECKSUM:

                break;
            }
        }
    }


}