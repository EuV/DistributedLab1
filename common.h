/* IFMO Distributed Class, autumn 2013
 *
 * common.h header file for PA1
 * Students must not modify this file!
 */

#ifndef IFMO_DISTRIBUTED_CLASS_PA1__COMMON__H
#define IFMO_DISTRIBUTED_CLASS_PA1__COMMON__H

#include <stdint.h>

typedef uint8_t local_id;

typedef struct {
    uint32_t    s_magic;        // magic signature, must be MESSAGE_MAGIC
    uint8_t     s_len;          // total length of the message including this header
    uint8_t     s_type;
    uint8_t     s_reserved[10];
} MessageHeader;

typedef enum {
    STARTED = 0,
    DONE = 1
} MessageType;

enum {
    MAX_MESSAGE_LEN = 255,
    MESSAGE_HEADER_LEN = sizeof(MessageHeader),
    MESSAGE_MAGIC = 0xAFAF
};

enum {
    PARENT_ID = 0
};

#endif // IFMO_DISTRIBUTED_CLASS_PA1__COMMON__H
