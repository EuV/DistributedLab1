/* IFMO Distributed Class, autumn 2013
 *
 * pa1.h header file
 * Students must not modify this file!
 */

#ifndef IFMO_DISTRIBUTED_CLASS_PA1__PA1__H
#define IFMO_DISTRIBUTED_CLASS_PA1__PA1__H

#include "common.h"
#include "ipc.h"

/* %5d should be substituted with PID, %1d - local id, e.g.:
 * Process 1 (pid 12341, parent 12340) has STARTED\n
 */
static const char * log_started_fmt =
    "Process %1d (pid %5d, parent %5d) has STARTED\n";

static const char * log_received_all_started_fmt =
    "Process %1d received all STARTED messages\n";

static const char * log_done_fmt =
    "Process %1d has DONE its work\n";

static const char * log_received_all_done_fmt =
    "Process %1d received all DONE messages\n";

#endif // IFMO_DISTRIBUTED_CLASS_PA1__PA1__H
