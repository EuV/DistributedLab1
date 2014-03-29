/**
 * @file     ipc.h
 * @Author   Michael Kosyakov and Evgeniy Ivanov (ifmo.distributedclass@gmail.com)
 * @date     September, 2013
 * @brief    A simple IPC library for programming assignments
 *
 */

#ifndef IFMO_DISTRIBUTED_CLASS__IPC__H
#define IFMO_DISTRIBUTED_CLASS__IPC__H

#include <stddef.h>

// Might be common.h from any PA, please adjust include paths
#include "common.h"

/** A duplex connection to any process.
 *
 * Each instance of this structure should be used by single process only.
 */
typedef struct {
    int s_sendfd;
    int s_recvfd;
    local_id s_id; ///< ID of the process we connected to
} Connection;

/** Connections between single process and all other processes.
 *
 */
typedef struct Connector {
    /** Connection array of size s_nconnections.
     *
     * By convention connection[0] should be connected to parrent.
     * Contents of s_connections[s_self_id] is undefined.
     */
    Connection * s_connections;
    int s_nconnections;
    local_id s_self_id;
} Connector;

/** Message used by all IPC routines.
 *
 */
typedef struct {
    MessageHeader s_header;
    void * s_payload;
} Message;

//------------------------------------------------------------------------------

/** Helper function, automatically sets some Message fields.
 * 
 * Should initialize message length, type, payload and magic.
 */
void fill_message(Message * msg, MessageType type, void * payload,
                  size_t psize);

//------------------------------------------------------------------------------

/** Send message to process specified by id.
 *
 * @param self    Connector of process which sends the message.
 * @param dst     ID of receiver (self->s_connections[dst]).
 * @param msg     Message to send.
 */
int send_to(const Connector * self, local_id dst, const Message * msg);

/** Send multicast message.
 *
 * Sends msg to all s_connections of self including parrent, except self itself
 * (self->s_connections[self->s_self_id]). Stops on the first error.
 * 
 * @param self    Connector of process which send the message.
 * @param msg     Message to multicast.
 *
 * @return 0 on success, -1 otherwise.
 */
int send_all(const Connector * self, const Message * msg);

/** Receive a full message from process specified by id.
 *
 * Might block depending on underlying fd settings and when only part
 * of the message is available in the pipe.
 *
 * @param msg must be freed by client.
 * @return 0 on success, -1 on error (errno is set to EAGAIN if
 * O_NONBLOCK is set and no data is available for reading).
 */
int receive_from(const Connector * self, local_id from, Message ** msg);

//------------------------------------------------------------------------------

enum {
    BLOCKING = 0,
    NONBLOCKING = 1
};

/** Create fully connected topology to be used in IPC.
 *
 * Creates an array of Connectors, which form fully connected topology.
 * Each connector.s_self_id is set to the index in connectors array. Clients
 * should use these ids as local ids for each process.
 * Connector[0] should be used for parent process.
 * @see Connector
 *
 * @param connector to be freed by client.
 * @param n number of connectors to creat.
 * @param nonblocking if BLOCKING, then O_NONBLOCK should be set for read fds,
 * for write fds it depends on implementation.
 */
int creat_fully_connected_topology(Connector ** connectors, size_t n,
                                   int nonblocking);

#endif // IFMO_DISTRIBUTED_CLASS__IPC__H
