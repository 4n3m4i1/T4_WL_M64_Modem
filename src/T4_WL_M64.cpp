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

void inline M64_CLEAR_SER_BUFF(){
#ifdef M64_CLEAR_BUFFER_ON_RX
    M64_SERIAL.clear();
#endif
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

uint8_t M64_fill_u32_to_delim(uint32_t *a, uint8_t delim){
    uint8_t val = 0;
    while(M64_SERIAL_PEEK() != delim){
        *a |= M64_SERIAL_READ() << (val << 3);
        val += 1;
    }
    // normalize so fast atoi can handle well
    // basically LSB should always be in byte 3, we only handle up to 4 char int
    //   or a range of 0 - 9999
    *a <<= ((sizeof(uint32_t) - val) << 3);    

    M64_SERIAL_READ();  // Consume delimeter
    return val;
}

void inline M64_deal_with_checksum(struct M64_MODEM_INST *a){
    M64_SERIAL_READ();  // discard checksum
    M64_SERIAL_READ();
    a->comm_in_progress = FALSE;
    a->comm_msg_type    = 0x00;
}

uint16_t M64_TEENYFAST_ATOI(uint32_t a){        // range 0 - 9999
    uint16_t retval = ((a) & 0xF - '0') * 1000;
    retval += ((a >> 8) & 0xF - '0')    * 100;
    retval += ((a >> 16) & 0xF - '0')   * 10;
    retval += (a & 0xF - '0')             ;
    return retval;
}


// This is the annoying bit
char M64_parser(struct M64_MODEM_INST *a){
    char retval = '\0';

    if(M64_SERIAL_AVAIL() > M64_MIN_RQ_LEN + 1 || a->comm_in_progress == TRUE){
        if(!a->comm_in_progress){
            while(M64_SERIAL_PEEK() != M64_MSG_0) M64_SERIAL_READ();
            if(M64_READ_AND_MATCH_SOP()){
                a->comm_in_progress     = TRUE;
                a->comm_msg_type        = M64_SERIAL_READ();    // Read valid cmd after SOP match
            } else {
                
            }
        }
        
        
        if(a->comm_in_progress){
            switch(a->comm_msg_type){
                case M64_VERSION: 
                {
                    if(M64_SERIAL_AVAIL() >= M64_VER_RES_LEN){
                        uint32_t field_0;
                        
                        M64_SERIAL_READ();      // Get rid of first comma
                        // Protocol major
                        M64_fill_u32_to_delim(&field_0, M64_FIELD_DELIM);
                        a->protocol_ver_major = (uint8_t)M64_TEENYFAST_ATOI(field_0);
                        // Minor
                        M64_fill_u32_to_delim(&field_0, M64_FIELD_DELIM);
                        a->protocol_ver_minor = (uint8_t)M64_TEENYFAST_ATOI(field_0);
                        // Patch
                        M64_fill_u32_to_delim(&field_0, M64_EODF);
                        a->protocol_ver_patch = (uint8_t)M64_TEENYFAST_ATOI(field_0);

                        M64_deal_with_checksum(a);  /// cleans up
                        retval  = M64_VERSION;
                        M64_CLEAR_SER_BUFF();
                    }
                    break;
                }
                    
                case M64_PAYLOAD_SIZE:
                {
                    if(M64_SERIAL_AVAIL() >= M64_PAY_RES_LEN){
                        uint32_t field_0;
                        M64_SERIAL_READ();      // nuke first comma

                        M64_fill_u32_to_delim(&field_0, M64_EODF);
                        a->supported_payload_size = (uint8_t)M64_TEENYFAST_ATOI(field_0);

                        M64_deal_with_checksum(a);
                        retval  = M64_PAYLOAD_SIZE;
                        M64_CLEAR_SER_BUFF();
                    }
                    break;
                }

                case M64_CONFIG_GET:
                {
                    if(M64_SERIAL_AVAIL() >= M64_CFG_RES_LEN){
                        M64_SERIAL_READ();      // nuke first comma

                        a->role = M64_SERIAL_READ();
                        M64_SERIAL_READ();      // skip comma 2

                        a->channel = M64_SERIAL_READ() - '0';
                        M64_SERIAL_READ();      // skip EODF

                        M64_deal_with_checksum(a);
                        retval = M64_CONFIG_GET;
                        M64_CLEAR_SER_BUFF();
                    }
                    break;
                }

                case M64_CONFIG_SET:
                {
                    if(M64_SERIAL_AVAIL() >= M64_CFS_RES_LEN){
                        M64_SERIAL_READ();  // skip first comma blah blah
                        a->last_set_okay = M64_SERIAL_READ();
                        
                        M64_deal_with_checksum(a);
                        retval = M64_CONFIG_SET;
                        M64_CLEAR_SER_BUFF();
                    }

                    break;
                }
                

                case M64_QUEUE_LEN:
                {
                    if(M64_SERIAL_AVAIL() >= M64_TXQ_RES_LEN){
                        uint32_t field_0;
                        M64_SERIAL_READ();      // guess what it does.. go ahead

                        M64_fill_u32_to_delim(&field_0, M64_EODF);
                        a->tx_q_len = (uint8_t)M64_TEENYFAST_ATOI(field_0);

                        M64_deal_with_checksum(a);
                        retval = M64_CONFIG_GET;
                        M64_CLEAR_SER_BUFF();
                    }
                    break;
                }

                case M64_QUEUE_FLUSH:
                {
                    if(M64_SERIAL_AVAIL() >= M64_FLQ_RES_LEN){
                        M64_SERIAL_READ();  // skip first comma blah blah
                        a->last_set_okay = M64_SERIAL_READ();
                        
                        M64_deal_with_checksum(a);
                        retval = M64_CONFIG_SET;
                        M64_CLEAR_SER_BUFF();
                    }
                    break;
                }

                case M64_DIAGNOSTIC:
                {
                    if(M64_SERIAL_AVAIL() >= M64_DIA_RES_LEN){
                        M64_SERIAL_READ();  //... I should make this implicit

                        a->link = M64_SERIAL_READ();
                        M64_SERIAL_READ();

                        uint32_t field_0;
                        
                        M64_fill_u32_to_delim(&field_0, M64_FIELD_DELIM);
                        a->packet_ct = (uint8_t)M64_TEENYFAST_ATOI(field_0);

                        M64_fill_u32_to_delim(&field_0, M64_FIELD_DELIM);
                        a->packet_loss_ct = (uint8_t)M64_TEENYFAST_ATOI(field_0);
                        

                        // float strtof(const char* str, char** endptr)
                        char rvi__[16] = {0x00};
                        for(uint16_t n = 0; n < 16; n++){
                            if(M64_SERIAL_PEEK() == M64_EODF){
                                M64_SERIAL_READ();
                                break;
                            } 
                            else {
                                rvi__[n] = M64_SERIAL_READ();
                            }
                        }
                        a->BER = strtof(rvi__, nullptr);

                        M64_deal_with_checksum(a);
                        retval = M64_DIAGNOSTIC;
                        M64_CLEAR_SER_BUFF();
                    }
                    break;
                }

                case M64_PACKET_TX:
                {
                    if(M64_SERIAL_AVAIL() >= M64_TXP_RES_LEN){
                        M64_SERIAL_READ();  // skip first comma blah blah
                        a->last_set_okay = M64_SERIAL_READ();
                        
                        M64_deal_with_checksum(a);
                        retval = M64_PACKET_TX;
                        M64_CLEAR_SER_BUFF();
                    }
                    break;
                }

                case M64_PACKET_RX:
                {
                    if(M64_SERIAL_AVAIL() >= ){

                        
                    }
                    break;
                }

                case M64_MALFORMED_RQ:
                {
                    if(M64_SERIAL_AVAIL() >= ){

                        
                    }
                    break;
                }

                case M64_BAD_CHECKSUM:
                {
                    if(M64_SERIAL_AVAIL() >= ){

                        
                    }
                    break;
                }

                default:
                {
                    if(M64_SERIAL_AVAIL() >= ){

                        
                    }
                    break;
                }
            }
        }
    }

    return retval;
}



char M64_parser_2(struct M64_MODEM_INST *a){
    if(M64_SERIAL_AVAIL());
}