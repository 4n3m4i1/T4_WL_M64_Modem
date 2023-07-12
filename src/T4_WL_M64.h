#ifndef T4_WL_M64_H
#define T4_WL_M64_H

#include <stdint.h>
#include "T4_WL_M64_CONSTANTS.h"


struct M64_MODEM_INST
{
    uint8_t     protocol_ver_major;
    uint8_t     protocol_ver_minor;
    uint8_t     protocol_ver_patch;
    uint8_t     supported_payload_size;
    char        role;
    uint8_t     channel;
    bool        last_set_okay;
    bool        last_tx_okay;
    uint8_t     tx_q_len;
    char        link;
    uint16_t    packet_ct;
    uint16_t    packet_loss_ct;
    float       BER;
    uint8_t     rx_q[M64_MAX_PAYLOAD_SIZE];
    uint8_t     last_rx_len;
};

uint8_t inline M64_SERIAL_PEEK();
uint8_t inline M64_SERIAL_READ();
uint8_t inline M64_SERIAL_AVAIL();
uint8_t inline M64_SERIAL_AVAIL_4_WRITE();
void inline M64_SERIAL_SEND(uint8_t val);
void inline M64_SERIAL_SEND_WC();
void M64_SERIAL_SEND_PACKET(uint8_t len, uint8_t *data);

bool M64_request(char command);

bool M64_set_modem_config(struct M64_MODEM_INST *a, uint8_t role, uint8_t channel);

bool M64_send_packet(uint8_t size, uint8_t *payload);

// Returns the message type decoded, if any
char M64_parser(struct M64_MODEM_INST *a);

#endif