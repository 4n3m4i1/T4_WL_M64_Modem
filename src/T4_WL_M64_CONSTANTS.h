
#define M64_ROLE_CONTROLLER 'a'
#define M64_ROLE_LISTENER   'b'

// binary to char converter
#define M64_CHANNEL(a)      ( (a < '0') ? a + '0' : a )

#define M64_BAUD            115200

#define M64_MSG_0           'w'
#define M64_MSG_1           'c'

#define M64_RS_MSG_1        'r'

#define M64_TERM            '\n'

#define M64_VERSION         'v'
#define M64_PAYLOAD_SIZE    'n'
#define M64_CONFIG_GET      'c'
#define M64_CONFIG_SET      's'
#define M64_QUEUE_LEN       'l'
#define M64_QUEUE_FLUSH     'f'
#define M64_DIAGNOSTIC      'd'
#define M64_PACKET_TX       'q'

#define M64_PACKET_RX       'p'

#define M64_MALFORMED_RQ    '?'
#define M64_BAD_CHECKSUM    '!'

#define M64_NO_MESSAGE      '\0'

#define M64_ACK             'a'
#define M64_NACK            'n'

#define M64_LINK_OK         'y'
#define M64_LINK_BAD        'n'


#define M64_VALID_MAJOR_VER 1
#define M64_MAX_PAYLOAD_SIZE    8

#define M64_MIN_RQ_LEN      3

#define M64_FIELD_DELIM     ','
#define M64_EODF            '*'
// Checksum at end of every response, *xx format
#define M64_CHECKSUM_LEN        3
//                             Data Bytes + Checksum Len + delimiters
#define M64_VER_RES_LEN         (3 + M64_CHECKSUM_LEN + 3 + 2)
#define M64_PAY_RES_LEN         (1 + M64_CHECKSUM_LEN + 1 + 2)
#define M64_CFG_RES_LEN         (2 + M64_CHECKSUM_LEN + 2 + 2)
#define M64_CFS_RES_LEN         (1 + M64_CHECKSUM_LEN + 1 + 2)
#define M64_TXQ_RES_LEN         (1 + M64_CHECKSUM_LEN + 1 + 2)
#define M64_FLQ_RES_LEN         (1 + M64_CHECKSUM_LEN + 1 + 2)
#define M64_DIA_RES_LEN         (7 + M64_CHECKSUM_LEN + 4 + 2)
#define M64_TXP_RES_LEN         (1 + M64_CHECKSUM_LEN + 1 + 2)
#define M64_RXP_RES_LEN         (1 + M64_CHECKSUM_LEN + 1 + 2) // implicit + size
#define M64_MAL_REQ_LEN         (1 + M64_CHECKSUM_LEN + 2)      // I think? This is not documented well
#define M64_CHKSUM_ERR_LEN      (1 + M64_CHECKSUM_LEN + 2)