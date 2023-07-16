#ifndef T4_WL_M64_H
#define T4_WL_M64_H

#include <stdint.h>
#include "T4_WL_M64_CONSTANTS.h"


struct M64_MODEM_INST
{
    uint8_t     comm_in_progress = 0;   // used if parse begins while RXing
    uint8_t     comm_msg_type;      // Actively being RXd message type
    
    uint8_t     protocol_ver_major;
    uint8_t     protocol_ver_minor;
    uint8_t     protocol_ver_patch;
    uint8_t     supported_payload_size = 8;
    char        role;
    uint8_t     channel;
    char        last_comm_type;         // predict next response
    char        last_set_okay;
    char        last_tx_okay;
    uint32_t    tx_q_len;               // dumb thing has 1M of buffer...
    char        link;
    uint16_t    packet_ct;
    uint16_t    packet_loss_ct;
    float       BER;
    uint8_t     rx_q[M64_MAX_PAYLOAD_SIZE];
    uint8_t     last_rx_len;
};

void inline M64_CLEAR_SER_BUFF();
uint8_t inline M64_SERIAL_PEEK();
uint8_t inline M64_SERIAL_READ();
uint8_t inline M64_SERIAL_AVAIL();
uint8_t inline M64_SERIAL_AVAIL_4_WRITE();
void inline M64_SERIAL_SEND(uint8_t val);
void inline M64_SERIAL_SEND_WC();
void M64_SERIAL_SEND_PACKET(struct M64_MODEM_INST *a, uint8_t len, uint8_t *data);

bool M64_request(struct M64_MODEM_INST *a, char command);

bool M64_set_modem_config(struct M64_MODEM_INST *a, uint8_t role, uint8_t channel);

bool M64_send_packet(struct M64_MODEM_INST *a, uint8_t size, uint8_t *payload);

// So we're going to use U32 types to hold 4 chars
//  might be slower but the rest of this is a trainwreck so whatever
uint8_t M64_fill_u32_to_delim(uint32_t *a, uint8_t delim);
void inline M64_deal_with_checksum(struct M64_MODEM_INST *a);
// Str -> int cvt, no negatives, all int
uint16_t M64_TEENYFAST_ATOI(uint32_t a);

// Returns the message type decoded, if any
char M64_parser(struct M64_MODEM_INST *a);


bool M64_setup(struct M64_MODEM_INST *a, uint8_t channel, char role);

uint32_t SKETCHY_ATOI(char *str, uint16_t len);
char M64_Events(struct M64_MODEM_INST *a);
char M64_parser_2(char *rvals, struct M64_MODEM_INST *a);

#endif