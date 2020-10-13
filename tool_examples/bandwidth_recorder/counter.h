#pragma once

#include <signal.h>
#include <stdalign.h>

_Static_assert(sizeof(sig_atomic_t) == sizeof(int),
               "sig_atomic_t has to be int for MPI to work!");
_Static_assert(alignof(sig_atomic_t) == alignof(int),
               "sig_atomic_t has to be int for MPI to work!");

#define COUNTERS_LEN 21
int _Atomic counters[COUNTERS_LEN] = {0};

#define I8_SEND_COUNT counters[0]
#define I16_SEND_COUNT counters[1]
#define I32_SEND_COUNT counters[2]
#define I64_SEND_COUNT counters[3]
#define FLOAT_SEND_COUNT counters[4]
#define DOUBLE_SEND_COUNT counters[5]
#define INT_SEND_COUNT counters[6]

#define I8_RECV_COUNT counters[7]
#define I16_RECV_COUNT counters[8]
#define I32_RECV_COUNT counters[9]
#define I64_RECV_COUNT counters[10]
#define FLOAT_RECV_COUNT counters[11]
#define DOUBLE_RECV_COUNT counters[12]
#define INT_RECV_COUNT counters[13]

#define I8_RDC_COUNT counters[14]
#define I16_RDC_COUNT counters[15]
#define I32_RDC_COUNT counters[16]
#define I64_RDC_COUNT counters[17]
#define FLOAT_RDC_COUNT counters[18]
#define DOUBLE_RDC_COUNT counters[19]
#define INT_RDC_COUNT counters[20]

char *COUNTER_NAMES[COUNTERS_LEN] = {
    "8-bit Intergers sent ",
    "16-bit Intergers sent",
    "32-bit Intergers sent",
    "64-bit Intergers sent",
    "Floats sent          ",
    "Doubles sent         ",
    "Integers sent        ",
    "8-bit Intergers received ",
    "16-bit Intergers received",
    "32-bit Intergers received",
    "64-bit Intergers received",
    "Floats received          ",
    "Doubles received         ",
    "Integers received        ",
    "8-bit Intergers reduced ",
    "16-bit Intergers reduced",
    "32-bit Intergers reduced",
    "64-bit Intergers reduced",
    "Floats reduced          ",
    "Doubles reduced         ",
    "Integers reduced        ",
};
